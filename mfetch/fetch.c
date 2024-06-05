#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define GRAY "\x1b[38;5;245m"
#define WHITE "\x1b[38;5;15m"

void fg(int fg) { printf("\x1b[38;5;%im", fg); }

void bg(int bg) { printf("\x1b[48;5;%im", bg); }

void clear(void) { printf("\x1b[0m"); }

void print_path(const char *path) {
  const char *last;
  if (*path == '~') {
    fg(245);
    printf("%c", *(path++));
    clear();
  }
  last = path;
  while (1) {
    if (*path == '/' || !*path) {
      printf("%.*s", (unsigned int)(path - last), last);
      if (!*path)
        return;
      fg(245);
      printf("%c", *(path++));
      fg(15);
      last = path;
    } else {
      path++;
    }
  }
}

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
  strftime(timestr, sizeof(timestr),
           "%Y" GRAY "-" WHITE "%m" GRAY "-" WHITE "%d %H" GRAY ":" WHITE
           "%M" GRAY ":" WHITE "%S",
           tmp);
  shell = shell ? strrchr(shell, '/') + 1 : "<no shell>";
  fg(15);
  printf("%s %s ", timestr, shell);
  print_path(tty_name);
  printf("\n");
  clear();
  return 0;
}
