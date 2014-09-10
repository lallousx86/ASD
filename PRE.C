#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

void main(void)
{
 int i;
 const char *about =
   "*************************************************\n" \
   "*  ASD: Automatic char shap determinator        *\n" \
   "*  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  *\n" \
   "*  created : 03/26/1999 (m/d/y)                 *\n" \
   "*  version : v1.0                               *\n" \
   "*  coder   : elias bachaalany                   *\n" \
   "*************************************************\n";

 for (i=0;i<50;i++)
  { sound(i*10); delay(10); }
 nosound();
 clrscr();

 printf("%s",s1);
}