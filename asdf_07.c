/* *************************************************
   *  ASD: Automatic char shape determinator       *
   *  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  *
   *  created : 03/26/1999 (m/d/y)                 *
   *  version : v1.0                               *
   *  compiler: C/C++ compilers                    *
   *  coder   : elias bachaalany                   *
   *  source  : ASDF.C                             *
   *************************************************
   changes
   ========
      11:00AM 03/31/1999 - fixed a bug in parameters
			   and accepting infile/outfile

    */

//Includes//////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

//General Defenitions////////////////////////////////////////////////////////
#define maxstrlen             250                // max len string can have
#define minstrlen 	      25                 // min len string can have
#define btfile                "bt.txt"           // behavior file name.
#define rlfile                "rl.txt"           // rules file
#define prfile                "pre.txt"          // pre-procession file
#define btecount              100                // behavior table elements count
#define strcount              16                 // how many buffers needed
#define maxprestrlen          10                 // max len a pre-str can have
#define maxprecount           100                // max pre elements
//Rules//////////////////////////////////////////////////////////////////////
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
#define errnotintbe            -4                // code not in tbe
//Options////////////////////////////////////////////////////////////////////
#define optdebugpos           1                  // output position string
#define optdebugdes           2                  // output description str
#define optdebugval           4                  // output code directly
//My Structures//////////////////////////////////////////////////////////////
typedef int trules[poscount][rltypecount][rlmaxdef]; // rules type
//                L/R/B/N/S  CHAR/NCHAR   DESICION/
typedef struct               //  strint can hold a num. val and a string.
{
  unsigned long val;        // val
  char str[minstrlen];      // str
} tstrint;

typedef struct              // pre-pro chars (where 2chars=1char)
{
  char str[maxprestrlen];  // val1
  char code;               // reduced to 1char = code
}tpre;

typedef struct
{
  tstrint pos[poscount];
} tbtable;

typedef struct tswitches
{
  char autodes;    // make desc. out of NONE field for right,left,both,skip (def:1)
  char debuglevel; // debug level (def:4)
  char outdebug;   // output debug info? (def:1)
  char help;       // if help is 1 all other options are ignored
} tswitches;

// Global vars defenitions///////////////////////////////////////////////////
 FILE       *file,*file1;             // file var declaration
 char       *s[strcount];             // buffers.
 tbtable    *tbe[btecount];           // table elements.
 tswitches  options;                  // options
 trules     rules;                    // rules vars.
 tpre       *pre[maxprecount];        // pre elements
//Buffers////////////////////////////////////////////////////////////////////
const char *about_buf =
   "*************************************************\n" \
   "*  ASD: Automatic char shape determinator       *\n" \
   "*  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  *\n" \
   "*  created : 03/30/1999 (m/d/y)                 *\n" \
   "*  version : v1.0                               *\n" \
   "*  coder   : elias bachaalany                   *\n" \
   "*************************************************\n";
const char *help_buf =
	"help\n" \
	"====\n" \
	" program [ /x:(option char) ] [/x:(option char)] [ . . . ]\n\n" \
	" option char list:\n" \
	" -----------------\n" \
	"   'a'                  auto description (0 or 1)\n" \
	"   'd'                  debug level (1=posdebug,2=desdebug,4=valdebug)\n" \
	"   'o'                  print debug info (0 or 1)\n" \
	"   'h'                  display this help (must be 1)\n" \
	"   'f'                  input file name (eg: /f:filename.txt)\n" \
	"   'p'                  output file name (eg: /p:out.txt)\n" \
	" examples:\n" \
	" ---------\n" \
	"   program /f:mytext.txt /p:mytext.out /o:0 /a:0 /d:0\n" \
	"           ^^            ^^            ^^   ^^   ^^\n" \
	"           `-in file name `-output file |    |   |\n" \
	"                         not output <--/     |   |\n" \
	"                    don't auto describe <---/    |\n" \
	"                          no debugging info <---/\n";
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
 count = size1*count;
 memset(ptr[0],0,count);  // clear all allocated arrs.
 return count;            // return size of allocated mem.
}

int chartopos(char ch) // char to position
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

char postochar(int pos) // position to char
{
 int j = errnotpos;
 switch (pos)
  {
	  case posleft :    j = 'L';     break;
	  case posright:    j = 'R';     break;
	  case posboth :    j = 'B';     break;
	  case posnone :    j = 'N';     break;
	  case posskip :    j = 'S';     break;
  };
 return j;
}

int readbtfile(void) // read behavior table file
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

  if (options.autodes) { // is automatic desc. opt on? yes go ahead

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

int readprefile(void) // read pre file
{
 int count;

 memset(pre[0],0,maxprecount*sizeof(tpre)); // clear all pre vars.

 file = fopen(prfile,"rt");
 if (file==NULL)
		return (errfilenotfound);
 count = 0;
 while (!feof(file))
 {
  fscanf(file,"%[^\n]\n",s[0]); // read one line in file.

  if ((*s[0]==';') || (!strcmp(s[0],"")))            // is it a comment line?
					     continue; // yes? skip it
  sscanf(s[0],"%[^=]=%[^\n]\n",s[1],s[2]);
  strcpy(pre[count]->str,s[1]);
  pre[count]->code = s[2][0];
  count++;
 };
 fclose(file);
 return count;
}

int readrlfile(void) // read rules file
{
 int i,j,count;

 memset(rules,0,sizeof(trules)); // clear rules.

 file = fopen(rlfile,"rt"); // open rules file
 if (file==NULL) // file not found.
		 return (errfilenotfound);

 i = j = count = 0;
 while (!feof(file))
 {
   fscanf(file,"%[^\n]\n",s[0]); // read one line in file.

   strcpy(s[0],strupr(rtrim(ltrim(s[0])))); // remove leading & trailing spaces

   if ((*s[0]==';') || (!strcmp(s[0],"")))            // is it a comment line?
					     continue; // yes? skip it

   s[1][0] = s[2][0] = s[3][0] = s[4][0] = 0;
   sscanf(s[0],"%[^=]=%[^=]=%[^/]/%[^\n]",s[1],s[2],s[3],s[4]);
   //           TYPE = POS = DES1/ DES2
   // get type
   if (!strcmp(s[1],"CHAR"))
      i = 0;
     else
      i = 1;
   // get POS
   j = chartopos(*s[2]);

   rules[j][i][0] = chartopos(*s[3]); // get def1
   rules[j][i][1] = chartopos(*s[4]); // get def2
   count++; // increase count
 };
 fclose(file);
 return(count);
}

int chartotbeidx(int code) // char code to index in tbe
{
 int i,j;

 j = errnotintbe;
 for (i=0;i<btecount;i++)
 {
  if ((char)tbe[i]->pos[posnone].val == code)
      {	j = i;
	break; }
 }
 return j;
}

int ischarnchar(int idx) // is tbe[idx] a char or skip ?
{
 // returns '0' for char and '1' for nchar
 return( (tbe[idx]->pos[posnone].val==0) || (idx<0) );
}

void processcmdline(int argc,char *argv[],
		    int base,int optcharcount,int saveloc,
		    char *optcharlist,char *optformat)
{
  // this procedure uses 's' as for returning everything
  // argc = same one as for main()
  // argv[] = also same one as for main()
  // base = s[base] where u get 1st val
  // optcharcount = option char list len (eg:"ado")
  // saveloc = where is last char that seperate constant from var
  // optcharlist = option char list (eg:"ado")
  // optformat = format of option (eg:"/x:%s")
 int i,k,saver;
 char *ptr;

 memset(s[0],0,maxstrlen*strcount); // clear strings.

 for (i=1;i<argc;i++)   // read all params.
  { strcat(s[0],argv[i]); strcat(s[0]," "); }

 k = base + optcharcount;
 strcpy(s[k],optcharlist);
 strcpy(s[k+1],optformat);
 for (i=0;i<optcharcount;i++)
 {
   saver         = s[k+1][saveloc]; // save var begining
   s[k+1][saveloc]   = 0;           // strip vars and leave const.
   s[k+1][saveloc-2] = s[k][i];   // put opt char
   ptr = strstr(s[0],s[k+1]); // find option
   s[k+1][saveloc]   = saver;           // restore var begining
   if (ptr!=NULL)          // option exist?
     sscanf(s[i+1],s[k+1],strcpy(s[i+1],ptr)); // ok,process it
 }
 // fill up options
 options.autodes = (*s[1]=='1');
 options.debuglevel = atoi(s[2]);
 options.outdebug = (*s[3]=='1');
 options.help     = (*s[4]=='1');
}

void asd_line(char *line)
{
 int i,a,b,c,d,e,f;
 // s[0] = line itself
 // s[1] = alt. line
 memset(s[0],0,maxstrlen*6); // clear all str buffers. (6 coz i'm using almost only 5)

 strcpy(s[0]+1,line);        // get line
 s[0][0] = ' ';              // 1st char in line is skip
 s[0][strlen(s[0])] = ' ';   // last char is skip also

 s[1][0] = 'S'; // 1st char in alt.str is skip

 // a      self index
 // b      char to pos alt.str-1
 // c      is char or skip line+1
 // d      decision #1
 // e      decision #2
 // f      choosen decision
 for (i=1;i<strlen(s[0])-1;i++) // process each char in line ,loop.
  {
   a = chartotbeidx(s[0][i]); // get self index

   if (a==errnotintbe) { // me not in tbe? yes,consider as skip
			 s[1][i] = 'S'; // put decision in alt.str
			 strcat(s[2],"(skip)");
			 s[3][i-1] = s[0][i]; // put it's code
			 continue; };

   b = chartopos(s[1][i-1]); // before char-1 in alt.str

   c = ischarnchar(chartotbeidx(s[0][i+1])); // next char+1

   d = rules[b][c][0]; // decision 1
   e = rules[b][c][1]; // decision 2

   f = (tbe[a]->pos[d].val==0) ? e : d; // decision1 not valid choose 2nd

   s[1][i] = postochar(f); // put next char in alt.str

				    // s[1] equ pos_str debug
   strcat(s[2],tbe[a]->pos[f].str); // s[2] equ description debug
   s[3][i-1] = tbe[a]->pos[f].val;  // s[3] equ true values
  }
  // clean s[1][0] and s[1][length(s[1])]
  strncpy(s[1],s[1]+1,strlen(s[1]));

  if (options.debuglevel & optdebugpos)
       { strcat(s[4],s[1]); strcat(s[4],"\n"); }
  if (options.debuglevel & optdebugdes)
       { strcat(s[4],s[2]); strcat(s[4],"\n"); }
  if (options.debuglevel & optdebugval)
       { strcat(s[4],s[3]); strcat(s[3],"\n"); }
  if (options.outdebug) // print debug info?
			printf("%s\n",s[4]); // go ahead
  strcpy(line,s[3]); // update line,returning true values.
}

void pre_line(char *line,int precount)
{
 int i,j;
 char *ptr;

 for (i=0;i<precount;i++)
 {
  memset(s[0],0,maxstrlen); // clear s[0]

  ptr = strstr(line,pre[i]->str); // found ?
  if (ptr==NULL)
		 continue;   // no continue
  j = ptr-line;             // yes? get len from start to foundpos

  strncpy(s[0],line,j);     // s[0] = copyfrom(start -> len)
  s[0][j] = pre[i]->code;   // s[0][len] = new code
  strcat(s[0],ptr+strlen(pre[i]->str)); // append to s[0] from len to end of line
  strcpy(line,s[0]); // update line
 };
}

unsigned char filexist(char *filename)
{
 _asm   push ds
 _asm 	mov  ax,0x3D00      // ah = open file,al = read only mode
 _asm 	lds  dx,filename    // ds:dx = file name
 _asm 	int  0x21           // call dos
 _asm   pushf               // push flags,i only need carry flag.
 _asm 	jnc  close_file
 _asm 	jmp  bye
    close_file:
 _asm   push ax
 _asm   pop  bx
 _asm   mov ax,0x3E00
 _asm   int 0x21
     bye:
 _asm   popf
 _asm   db 0x0F,0x92,0xC0    //setb al = set carry value to al
 _asm 	pop  ds
 return(!_AL);
}

void show_help(void) // help show proc.
{
 printf(help_buf);
}

void show_about(void)
{
 int i;
 printf("%s\n",about_buf);
 for (i=0;i<50;i++)
  { sound(i*10); delay(2); }
 nosound();
}
/////////////////////////////////////////////////////////////////////////////
int main(int argc,char *argv[])
{
 int i,j,count,precount;

 char infile[79],outfile[79];

 // clears the screen
 clrscr();
 // show about
 show_about();
 // allocate strings
 j = arrayalloc(s,strcount,maxstrlen);
 if (j==errnomem)
  { cprintf(" þ Failed to allocate memory for internal buffers\n\x0D");
    return(j);}
     else
   cprintf(" þ Allocating %d byte(s) for internal buffers\n\x0D",j);

 // process command line
 // 'a'   : auto description
 // 'd'   : debug level
 // 'o'   : output debug info
 // 'h'   : help
 // 'f'   : in file name
 // 'p'   : out file name
 processcmdline(argc,argv,1,6,3,"adohfp","/x:%s");
 strcpy(infile,s[5]); // save input file name
 strcpy(outfile,s[6]); // save output file name
 if ((options.help) || (argc<2))
  { show_help();
    goto l_frees;};

 if (!strcmp(infile,""))
  { printf("infile not found\n");
    return(0); };
 if (!strcmp(outfile,""))
  { printf("outfile not assigned\n");
    return(0); };

 // allocate pre(s)
 j = arrayalloc(pre,maxprecount,sizeof(tpre));
 if (j==errnomem)
  { cprintf(" þ Failed to allocate memory for pre buffers\n\x0D");
    return(j);}
     else
   cprintf(" þ Allocating %d byte(s) for pre\n\x0D",j);

 // allocate tbe
 j = arrayalloc(tbe,btecount,sizeof(tbtable));
 if (j==errnomem)
  {cprintf(" þ Failed to allocate memory for behavior table elements\n\x0D");
   return(j);}
     else
   cprintf(" þ Allocating %d byte(s) for behavior table elements\n\x0D",btecount*sizeof(tbtable));

 // read pre file
 precount = readprefile();
 if (precount==errfilenotfound)
   { cprintf(" þ Failed to open pre file '%s'\n\x0D",prfile);
     return(precount); }
   else
    cprintf(" * %d entries in pre file\n\x0D",precount);

 // read bt file
 count = readbtfile();
 if (count==errfilenotfound)
   {cprintf(" þ Failed to open behavior file '%s'\n\x0D",btfile);
    return(count);}
   else
    cprintf(" * %d entries in behavior table file\n\x0D",count);

 // read rule file
 count = readrlfile();
 if (count==errfilenotfound)
   {cprintf(" þ Failed to open rules file '%s'\n\x0D",rlfile);
    return(count);}
   else
    cprintf(" * %d rule(s) in rules set file\n\x0D",count);

/////////////////////////////////////////////////////////////////////////////

 if (!filexist(infile))
  { printf("file not found %s\n",infile);
    return(errfilenotfound); }
 file = fopen(infile,"rt");
 file1= fopen(outfile,"wt");
 while (!feof(file))
 {
  /*
  fscanf(file,"%[^\n]\n",infile);
  fscanf(file,"%[^\n]\n",infile);
  fscanf(file,"%[^\n]\n",infile);
  fscanf(file,"%[^\n]\n",infile);
  gets
  */
  /*
  fgets(infile,sizeof(infile),file); // num.
  fputs(infile,file1);
  */
  fgets(infile,sizeof(infile),file); // fr.1
  pre_line(infile,precount);
  asd_line(infile);
  fputs(infile,file1);
  /*
  fgets(infile,sizeof(infile),file); // fr1.
  fputs(infile,file1);

  fgets(infile,sizeof(infile),file); // fr2.
  fputs(infile,file1);

  if (file1!=NULL) fprintf(file1,"%s\n",infile);
  */
 };
 fclose(file);
 fclose(file1);
/////////////////////////////////////////////////////////////////////////////
 // free buffers.
 l_ret:
 l_freetbe:
 free(tbe[0]); // free tbe buf.
 l_frees:
 free(s[0]); // free str buf.
 l_freepre:
 free(pre[0]); // free pre buf

 return 0;
}
/////////////////////////////////////////////////////////////////////////////