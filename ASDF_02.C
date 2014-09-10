/* *************************************************
   *  ASD: Automatic char shap determinator        *
   *  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  *
   *  created : 03/26/1999 (m/d/y)                 *
   *  version : v1.0                               *
   *  compiler: Borland C++ v3.1                   *
   *  coder   : elias bachaalany                   *
   *  source  : ASDF.C                             *
   *************************************************
*/

//Includes//////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Defenitions////////////////////////////////////////////////////////////////
#define maxstrlen             250                // max len string can have
#define minstrlen 	      25                 // min len string can have
#define btfile                "bt.txt"           // behavior file name.
#define btecount              100                // behavior table elements count
#define strcount              16                 // how many buffers needed
///Error Codes///////////////////////////////////////////////////////////////
#define errnomem               -1
#define errbtnotfound          -2

//My Structures//////////////////////////////////////////////////////////////
typedef struct               //  strint can hold a num. val and a string.
{
  unsigned long val;        // val
  char str[minstrlen];      // str
}tstrint;

typedef struct tbtable
{
  tstrint left;  // left behavior field
  tstrint right; // right behavior field
  tstrint both;  // both behavior field
  tstrint none;  // none behavior field
  tstrint skip;  // skip behavior field
} tbtable;

typedef struct tswitches
{
  char autodesc;  // make desc. out of NONE field for right,left,both,skip
}tswitches;

// Global vars defenitions///////////////////////////////////////////////////
 FILE *file;                     // file var declaration
 char *s[strcount];              // buffers.
 tbtable *tbe[btecount];         // table elements.
 tswitches options;              // options
/////////////////////////////////////////////////////////////////////////////

// removes leading spaces from a string.
char *ltrim(char *s)
{
 while ((*s==' ') || (*s==9))
			     s++;
 return s;
}
// removes trailing spaces from a string.
char *rtrim(char *s)
{
 char *pos;
 pos = s + strlen(s)-1;
 while ((*pos==' ') || (*pos==9))
				 pos--;
 *(pos+1) = 0;
 return s;
}

int arrayalloc(char *ptr[],int count,int size1)
{
 // ptr = pointer to allocate to
 // count = elements to count
 // size1 = 1 element count
 int i;
 ptr[0] = malloc(count*size1);
 if (ptr[0]==NULL)
		  return(errnomem);
 for (i=1;i<count;i++)
 {
  ptr[i] = ptr[0] + (i*size1);      // ptr[i] = tbe[main_addr] + (i*size1)
				    // ptr[0] = 1000 , size1=100
				    // ptr[1] = 1000 + 1*100 (=1100)
				    // ptr[2] = 1000 + 2*100 (=1200)
				    // etc...
 }
 memset(ptr[0],0,size1*count);
 return 0;
}

int readbtfile(void)
{
 int i,j,count;
 file = fopen(btfile,"rt"); // open behavior table file.
 if (file==NULL) { // file not found ?
		  return(errbtnotfound);
		 }

 count = 0; // clear counter.

 // process files lines loop.
 while (!feof(file)) // loop until end of file
 {
   fscanf(file,"%[^\n]\n",s[0]); // read one line in file.

   strcpy(s[0],rtrim(ltrim(s[0]))); // remove leading & trailing spaces

   if ((*s[0]==';') || (!strcmp(s[0],"")))            // is it a comment line?
					     continue; // yes? skip it

   sscanf(s[0],"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]\n",s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[8],s[9],s[10]);
   //           L    ,R    ,B    ,N    ,S    ,D-L  ,D-R  ,D-B  ,D-N  ,D-S      L    R    B    N    S    DL   DR   DB   DN   DS

   for (j=1;j<10;j++) // clean values from spaces
     strcpy(s[j],ltrim(rtrim(s[j])));

  // get values
  tbe[j]->left.val  = atoi(s[1]);
  tbe[j]->right.val = atoi(s[2]);
  tbe[j]->both.val  = atoi(s[3]);
  tbe[j]->none.val  = atoi(s[4]);
  tbe[j]->skip.val  = atoi(s[5]);

  // get descriptions
  strcpy(tbe[j]->none.str,s[9]);

  if (options.autodesc) { // is automatic desc. opt on? yes go ahead
			  s[9][1]='l';
			  strcpy(tbe[j]->left.str,s[9]);

			  s[9][1]='r';
			  strcpy(tbe[j]->right.str,s[9]);

			  s[9][1]='b';
			  strcpy(tbe[j]->both.str,s[9]);

			  s[9][1]='s';
			  strcpy(tbe[j]->skip.str,s[9]);
			} else
			{ // no autodesc? ok,read it from file
			  strcpy(tbe[j]->left.str,s[6]);
			  strcpy(tbe[j]->right.str,s[7]);
			  strcpy(tbe[j]->both.str,s[8]);
			  strcpy(tbe[j]->skip.str,s[10]);
			};
 count++;
 };
 fclose(file); // close file.
 return count;
}
/////////////////////////////////////////////////////////////////////////////
void main(int argc,char *argv[])
{
 int i,j;

 // setup options.
 options.autodesc = 1;

 // allocate tbe
 j = arrayalloc(tbe,btecount,sizeof(tbtable));

 // allocate strings
 j = arrayalloc(s,strcount,maxstrlen);

 // read bt file
 j = readbtfile();
 printf("%d entries in behavior table file\n",j);

 // free buffers.
 free(tbe[0]); // free tbe buf.
 free(s); // free str buf.
}
/////////////////////////////////////////////////////////////////////////////