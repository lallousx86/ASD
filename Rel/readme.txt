*************************************************
*  ASD: Automatic char shape determinator       *
*  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  *
*  created : 03/30/1999 (m/d/y)                 *
*  version : v1.0                               *
*  coder   : elias bachaalany                   *
*************************************************

help
====
 program [ /x:(option char) ] [/x:(option char)] [ . . . ]

 option char list:
 -----------------
   'a'                  auto description (0 or 1)
   'd'                  debug level (1=posdebug,2=desdebug,4=valdebug)
   'o'                  print debug info (0 or 1)
   'h'                  display this help (must be 1)
   'f'                  input file name (eg: /f:filename.txt)
   'p'                  output file name (eg: /p:out.txt)
 examples:
 ---------
   program /f:mytext.txt /p:mytext.out /o:0 /a:0 /d:0
           ^^            ^^            ^^   ^^   ^^
           `-in file name `-output file |    |   |
                         not output <--/     |   |
                    don't auto describe <---/    |
                          no debugging info <---/

more examples:
---------------
 to use a simple:
   asd /f:myfile.txt /p:myfile.out

