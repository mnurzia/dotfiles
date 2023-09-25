#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define PL_FULL "\xEE\x82\xB0" /* U+E0B0 */
#define PL_LINE "\xEE\x82\xB1" /* U+E0B1 */

int main(void) {
  char *tty_name = ttyname(STDIN_FILENO);
  time_t t;
  struct tm *tmp;
  char timestr[256] = "<no time>";
  char *shell = getenv("SHELL");
  if (!tty_name) {
    tty_name = "<no tty>";
  }
  t = time(NULL);
  tmp = localtime(&t);
  strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tmp);
  shell = shell ? strrchr(shell, '/') + 1 : "<no shell>";
  printf("\x1b[48;5;236m\x1b[38;5;15m %s " PL_LINE " %s " PL_LINE " %s "
         "\x1b[0m\x1b[38;5;236m" PL_FULL "\x1b[0m\n",
         timestr, shell, tty_name);
  return 0;
}
