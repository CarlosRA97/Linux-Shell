
/*============================================================*/
/* lee un caracter sin esperar a '\n' y sin eco */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include "ampliacion.h"
char getch()
{
 int shell_terminal = STDIN_FILENO;
 struct termios conf;
 struct termios conf_new;
 char c;

 tcgetattr(shell_terminal,&conf); /* leemos la configuracion actual */
 conf_new = conf;

 conf_new.c_lflag &= (~(ICANON|ECHO)); /* configuramos sin buffer ni eco */
 conf_new.c_cc[VTIME] = 0;
 conf_new.c_cc[VMIN] = 1;

 tcsetattr(shell_terminal,TCSANOW,&conf_new); /* establecer configuracion */

 c = getc(stdin); /* leemos el caracter */

 tcsetattr(shell_terminal,TCSANOW,&conf); /* restauramos la configuracion */
 return c;
} 