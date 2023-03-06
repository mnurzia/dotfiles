#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int
main()
{
  char* tty_name = ttyname(STDIN_FILENO);
  if (!tty_name) {
    tty_name = "<no tty>";
  }
  time_t t;
  struct tm* tmp;
  t = time(NULL);
  tmp = localtime(&t);
  char timestr[256] = "<no time>";
  strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tmp);
  char* shell = getenv("SHELL");
  shell = shell ? strrchr(shell, '/') + 1 : "<no shell>";
  printf("\x1b[48;5;236m %s \ue0b1 %s \ue0b1 %s "
         "\x1b[0m\x1b[38;5;236m\ue0b0\x1b[0m\n",
         timestr,
         shell,
         tty_name);
  return 0;
}
