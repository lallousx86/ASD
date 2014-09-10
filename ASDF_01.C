/* *************************************************
   *  ASD: Automatic arabic char Shap Determinator *
   *  created : 03/26/1999 (m/d/y)                 *
   *  version : v1.0                               *
   *  compiler: Borland C++ v3.1                   *
   *  coded by: elias bachaalany                   *
   *                                               *
   *************************************************
*/

//Includes//////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Defenitions////////////////////////////////////////////////////////////////
#define maxstrlen             300               // max len string can have
#define minstrlen 	      30               // min len string can have
#define btfile                "bt.txt"        // behavior file name.
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
};
/////////////////////////////////////////////////////////////////////////////

// removes leading spaces from a string.
char *ltrim(char *s)
{
 while (*s==' ') s++;
 return s;
}
// removes trailing spaces from a string.
char *rtrim(char *s)
{
 char *pos;
 pos = s + strlen(s)-1;
 while (*pos==' ') pos--;
 *(pos+1) = 0;
 return s;
}

void main(int argc,char *argv[])
{
 #define bufcount 16
 FILE *file; // file var declaration
 int  i,j;   // some loop vars.
// char *s1,*s2,*s3,*s4,*s5,*s6,*s7,*s8,*s9,*s10,*s11; // some buffers.
 char *s[bufcount];

 // allocate mem. for main buff.
 s[0] = (char *) malloc(maxstrlen*bufcount);
 if (s[0]==NULL) { // mem not allocated?
		  printf("Not enough memory . . . malloc() failed-(%d)\n",errnomem);
		  exit(errnomem);
		 }

 // setup the reset of buffers.
 for (i=1;i<(bufcount-1);i++)
 {
  s[i] = s[0] + (maxstrlen*i); // buf[i] = main_buf_addr + (maxstrlen*i)
			       // this will do:
			       // buf[1] = main_buf + 300*1
			       // buf[2] = main_buf + 300*2
			       // etc....
  memset(s[i],0/*i+'A'*/,maxstrlen); // clear buf[i]
 }

 file = fopen(btfile,"rt"); // open behavior table file.
 if (file==NULL) { // file not found ?
		  printf("Error opening file %s-(%d)\n",btfile,errbtnotfound);
		  exit(errbtnotfound);
		 }

 // process files lines loop.
 while (!feof(file)) // loop until end of file
 {
   fscanf(file,"%[^\n]\n",s[0]); // read one line in file.

   strcpy(s[0],rtrim(ltrim(s[0]))); // remove leading & trailing spaces

   if (*s[0]==';')               // is it a comment line?
		continue;      // yes? skip it

   sscanf(s[0],"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]\n",s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[8],s[9],s[10]);
   //           L    ,R    ,B    ,N    ,S    ,D-L  ,D-R  ,D-B  ,D-N  ,D-S      L  R  B  N  S  DL DR DB DN  DS
 }
 fclose(file); // close file.

 // free main buffer.
 free:
 free(s[0]); // free main buf.
}