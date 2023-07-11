#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#define SEG_SEP "\xee\x82\xb0"
#define SEG_SEP_MINOR "\xee\x82\xb1"
#define BRANCH_ICON "\xee\x9c\xa5"

int last_seg_bg = -1;

int term_width = 80;
int term_height = 24;

void put_fg(int fg) { printf("\\[\x1b[38;5;%im\\]", fg); }

void put_bg(int bg) { printf("\\[\x1b[48;5;%im\\]", bg); }

void put_sep(void) { printf(SEG_SEP); }

void put_sep_minor(void) { printf(SEG_SEP_MINOR); }

void clear(void) { printf("\\[\x1b[0m\\]"); }

void seg_begin(void) { clear(); }

/* Output segment with foreground and background. */
void seg(int fg, int bg) {
  if (last_seg_bg != -1) {
    put_fg(last_seg_bg);
    put_bg(bg);
    put_sep();
  }
  put_fg(fg);
  put_bg(bg);
  last_seg_bg = bg;
}

void seg_minor(int fg, int bg, int seg_col) {
  if (last_seg_bg != -1) {
    put_fg(seg_col);
    put_bg(bg);
    put_sep_minor();
  }
  put_fg(fg);
  put_bg(bg);
  last_seg_bg = bg;
}

void seg_end(void) {
  clear();
  put_fg(last_seg_bg);
  put_sep();
  clear();
  printf(" ");
  fflush(stdout);
}

void print_login_hostname(void) {
  char *buf;
  char *buf2;
  struct passwd pwd;
  struct passwd *out;
  buf = malloc(80);
  buf2 = malloc(160);
  *buf = '\0';
  getpwuid_r(getuid(), &pwd, buf2, 160, &out);
  gethostname(buf, 80);
  {
    char *c = buf;
    while (*c) {
      if (*c == '.') {
        *c = '\0';
      }
      c++;
    }
  }
  printf(" %s@%s ", buf2, buf);
  free(buf);
  free(buf2);
}

void print_wd(int short_fmt) {
  char *buf = malloc(PATH_MAX);
  char *home = getenv("HOME");
  char *wd = getcwd(buf, PATH_MAX);
  char *obuf = malloc(PATH_MAX);
  *obuf = '\0';
  if (!wd) {
    wd = "<unlinked>";
  }
  if (strncmp(home, wd, strlen(home)) == 0) {
    strcat(obuf, "~");
    wd += strlen(home);
    strcat(obuf, wd);
  } else {
    strcat(obuf, wd);
  }
  if (!short_fmt) {
    printf(" %s ", obuf);
  } else {
    char *last_slash = strrchr(obuf, '/');
    if (!last_slash) {
      printf(" %s ", obuf);
    } else if (last_slash == obuf && !*(last_slash + 1)) {
      /* "/" */
      printf(" %s ", obuf);
    } else {
      printf(" %s ", last_slash + 1);
    }
  }
}

void print_exit(int exit_code) { printf(" %i ", exit_code); }

void seg_user_color(void) {
  char *color = getenv("PROMPT_USER_COLORS");
  int fg, bg;
  if (getuid() == 0) {
    fg = 15;
    bg = 229;
  } else if (color) {
    sscanf(color, "%i, %i", &fg, &bg);
  } else {
    fg = 15;
    bg = 99;
  }
  seg(fg, bg);
}

void print_hash(void) {
  if (getuid() == 0) {
    printf(" # ");
  } else {
    printf(" $ ");
  }
}

void print_git_branch(int short_fmt) {
  char *wd_buf = malloc(PATH_MAX);
  char *git_buf = malloc(PATH_MAX);
  char *wd = getcwd(wd_buf, PATH_MAX);
  int git_rank = 0;
  char head_buf[256] = "";
  /* suffix with / to normalize loop */
  strcat(wd_buf, "/");
  if (!wd) {
    return;
  }
  while (*wd) {
    struct stat sb;
    /* copy path to temp */
    strcpy(git_buf, wd_buf);
    /* copy .git */
    strcpy(git_buf + (wd - wd_buf) + 1, ".git");
    if (stat(git_buf, &sb) == 0 && S_ISDIR(sb.st_mode)) {
      struct stat hb;
      strcat(git_buf, "/HEAD");
      if (stat(git_buf, &hb) == 0 && S_ISREG(hb.st_mode)) {
        /* read HEAD */
        int hfd = open(git_buf, O_RDONLY);
        if (hfd != -1) {
          read(hfd, head_buf, 256);
          close(hfd);
        }
        git_rank++;
      }
    }
    /* advance to next dir */
    do {
      wd++;
    } while (*wd && *wd != '/');
  }
  if (head_buf[0]) {
    char *last_slash = strrchr(head_buf, '/');
    char *print_from = head_buf;
    char *nl = strchr(head_buf, '\n');
    if (last_slash) {
      print_from = last_slash + 1;
    }
    if (nl) {
      *nl = '\0';
    }
    seg_minor(15, 234, 239);
    if (git_rank > 1) {
      printf(" %i" BRANCH_ICON " ", git_rank);
    } else {
      printf(" " BRANCH_ICON " ");
    }
    if (!short_fmt) {
      printf("%s ", print_from);
    }
  }
}

void get_dims(void) {
  struct winsize sz;
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &sz) == -1) {
    return;
  }
  term_width = sz.ws_col;
  term_height = sz.ws_row;
}

int main(int argc, char **argv) {
  int exit_code;
  int short_fmt;
  (void)(argc);
  get_dims();
  short_fmt = term_width < 80;
  sscanf(argv[1], "%i", &exit_code);
  seg_begin();
  if (!short_fmt) {
    seg(15, 236);
    print_login_hostname();
  }
  seg(15, 234);
  print_wd(short_fmt);
  print_git_branch(short_fmt);
  if (exit_code != 0) {
    seg(15, 9);
    print_exit(exit_code);
  }
  seg_user_color();
  print_hash();
  seg_end();
  return 0;
}
