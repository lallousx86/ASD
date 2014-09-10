/* *************************************************
   *  ASD: Automatic char shap determinator        *
   *  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  *
   *  created : 03/26/1999 (m/d/y)                 *
   *  version : v1.0                               *
   *  compiler: Borland C++ v3.1                   *
   *  coder   : elias bachaalany                   *
   *  source  : ASDF.C                             *
   ************************************************* */

//Includes//////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//General Defenitions////////////////////////////////////////////////////////
#define maxstrlen             250                // max len string can have
#define minstrlen 	      25                 // min len string can have
#define btfile                "bt.txt"           // behavior file name.
#define rlfile                "rl.txt"
#define btecount              100                // behavior table elements count
#define strcount              16                 // how many buffers needed
/////////////////////////////////////////////////////////////////////////////
#define rltypecount           2                  // rules types count (char/nonchar)
#define rlchar                0                  // char type code
#define rlnchar               1                  // none char type code
#define rlmaxdef              2                  // max default vals can type have
/////////////////////////////////////////////////////////////////////////////
#define poscount              5                  // positions count
#define posleft               0                  // left
#define posright              posleft+1          // right
#define posboth               posleft+2          // both
#define posnone               posleft+3          // none
#define posskip               posleft+4          // skip
///Error Codes///////////////////////////////////////////////////////////////
#define errnomem               -1                // not enough memory.
#define errfilenotfound        -2                // file not found.
#define errnotpos              -3                // not a valid position
//My Structures//////////////////////////////////////////////////////////////
typedef int trules[poscount][rltypecount][rlmaxdef]; // rules type
//                L/R/B/N/S  CHAR/NCHAR   DESICION/
typedef struct               //  strint can hold a num. val and a string.
{
  unsigned long val;        // val
  char str[minstrlen];      // str
}tstrint;

typedef struct
{
  tstrint pos[poscount];
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
 trules    rules;                // rules vars.
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
		  return(errfilenotfound);
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

  // bug!!
  j = count;
  // get values
  tbe[j]->pos[posleft].val  = atoi(s[1]);
  tbe[j]->pos[posright].val = atoi(s[2]);
  tbe[j]->pos[posboth].val  = atoi(s[3]);
  tbe[j]->pos[posnone].val  = atoi(s[4]);
  tbe[j]->pos[posskip].val  = atoi(s[5]);

  // get descriptions
  strcpy(tbe[j]->pos[posnone].str,s[9]);
  strcpy(tbe[j]->pos[posskip].str,s[10]);

  if (options.autodesc) { // is automatic desc. opt on? yes go ahead

			  if(tbe[j]->pos[posskip].val)
			      { strcpy(s[9],tbe[j]->pos[posskip].str);
				strcpy(tbe[j]->pos[posnone].str,s[10]);} // fill none becoz its a skip char.

			  s[9][1]='l';
			  strcpy(tbe[j]->pos[posleft].str,s[9]);

			  s[9][1]='r';
			  strcpy(tbe[j]->pos[posright].str,s[9]);

			  s[9][1]='b';
			  strcpy(tbe[j]->pos[posboth].str,s[9]);

			  s[9][1]='s';
			  strcpy(tbe[j]->pos[posskip].str,s[9]);
			} else
			{ // no autodesc? ok,read it from file
			  strcpy(tbe[j]->pos[posleft].str,s[6]);
			  strcpy(tbe[j]->pos[posright].str,s[7]);
			  strcpy(tbe[j]->pos[posboth].str,s[8]);
			  strcpy(tbe[j]->pos[posskip].str,s[10]);
			};

 count++;
 };
 fclose(file); // close file.
 return count;
}

int chartopos(char ch)
{
 int j = errnotpos;
 switch (ch)
  {
	  case 'L' : j = posleft;     break;
	  case 'R' : j = posright;    break;
	  case 'B' : j = posboth;     break;
	  case 'N' : j = posnone;     break;
	  case 'S' : j = posskip;     break;
  };
 return j;
}
/////////////////////////////////////////////////////////////////////////////
int main(int argc,char *argv[])
{
 const *lines[] =
		 {
		   "«⁄ÿ‰« «·›—’… ·‰‰’  Ì« \"ﬂ«—„Ì«\"",
		   "«‰   ‰’  »«·„—…",
		   "·”  «Â „° Õ›Ÿ  –·ﬂ ⁄‰ ŸÂ— ﬁ·»",
		   "‰Â«Ì… «Õ ›«·° «·⁄«·„ „·Ì¡ »«·”Õ—",
		   "«‰Â ›Ì \"«· Ì» \" « ‘⁄— »«·ﬁÊ…ø",
		   "·Ê «’€Ì  ·–·ﬂ"
		 };
 int i,j,k,count;

 // setup options.
 options.autodesc = 1;

 ////////////////////////////////////////////////////////////////////////////

 j = arrayalloc(s,strcount,maxstrlen);

 memset(rules,0,sizeof(trules));

 file = fopen(rlfile,"rt"); // open rules file
 if (file==NULL) // file not found.
		 return (errfilenotfound);
 i = j = k = count = 0;
 while (!feof(file))
 {
   fscanf(file,"%[^\n]\n",s[0]); // read one line in file.

   strcpy(s[0],strupr(rtrim(ltrim(s[0])))); // remove leading & trailing spaces

   if ((*s[0]==';') || (!strcmp(s[0],"")))            // is it a comment line?
					     continue; // yes? skip it
   strcpy(s[1],strtok(s[0],"'=")); // first occurence;type
   strcpy(s[2],strtok(NULL,"="));  // NULL=next char;pos
   strcpy(s[3],strtok(NULL,"="));  // NULL=next char;decision/all
   strcpy(s[4],strtok(s[3],"/"));  // decision 1
   strcpy(s[5],strtok(NULL,"/"));  // decision 2
      {
	// get type
	if (!strcmp(s[1],"CHAR"))
	 i = 0;
	   else
	 i = 1;
	// get POS
	j = chartopos(*s[2]);

	rules[j][i][0] = chartopos(*s[4]); // get def1
	rules[j][i][1] = chartopos(*s[5]); // get def2
      }
 }
 fclose(file);
 free(s);
 return 0;
 ////////////////////////////////////////////////////////////////////////////

 // allocate tbe
 j = arrayalloc(tbe,btecount,sizeof(tbtable));

 // allocate strings
 j = arrayalloc(s,strcount,maxstrlen);

 // read bt file
 count = readbtfile();
 printf("%d entries in behavior table file\n",count);

 // free buffers.
 free(tbe[0]); // free tbe buf.
 free(s); // free str buf.

 return 0;
}
/////////////////////////////////////////////////////////////////////////////