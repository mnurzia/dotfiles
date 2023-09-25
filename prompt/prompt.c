#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <unistd.h>

const char *get_msystem(void) {
  static int detected = -1;
  static const char *msystem = NULL;
  if (detected == -1) {
    msystem = getenv("MSYSTEM");
    detected = !!msystem;
  }
  return msystem;
}

int is_msys(void) { return !!get_msystem(); }

int is_wsl(void) {
  static int detected = -1;
  if (detected == -1) {
    struct utsname u;
    if (uname(&u))
      return detected = 0;
    detected = !!strstr(u.release, "microsoft");
  }
  return detected;
}

#define SEG_SEP "\xee\x82\xb0"
#define SEG_SEP_MINOR "\xee\x82\xb1"
#define BRANCH_ICON "\xee\x9c\xa5"
#define WINDOWS_ICON "\xee\x98\xaa"

int last_seg_bg = -1;

int term_width = 80;
int term_height = 24;

int short_fmt = 0;

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
  seg(15, 236);
  printf(" %s@%s ", buf2, buf);
  free(buf);
  free(buf2);
}

void print_wd(void) {
  char *buf = malloc(PATH_MAX);
  char *home = getenv("HOME");
  char *wd = getcwd(buf, PATH_MAX);
  char *obuf = calloc(PATH_MAX, 1);
  const char *windows_drive = NULL;
  if (!wd) {
    wd = "<unlinked>";
  }
  if ((is_wsl() && strstr(wd, "/mnt/") == wd && wd[5] &&
       (wd[6] == '/' || !wd[6]) && (windows_drive = wd + 5)) ||
      (is_msys() && wd[1] && (wd[2] == '/' || !wd[2]) &&
       (windows_drive = wd + 1))) {
    obuf[0] = toupper(*windows_drive);
    strcat(obuf, ":");
    strcat(obuf, windows_drive + 1);
    {
      char *i = obuf;
      while (*i) {
        if (*i == '/')
          *i = '\\';
        i++;
      }
    }
  } else {
    if (strncmp(home, wd, strlen(home)) == 0) {
      strcat(obuf, "~");
      wd += strlen(home);
    }
    strcat(obuf, wd);
  }
  seg(15, 234);
  if (!short_fmt) {
    printf(" %s ", obuf);
  } else {
    char *last_slash = strrchr(obuf, windows_drive ? '\\' : '/');
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

void print_exit(int exit_code) {
  seg(15, 9);
  printf(" %i ", exit_code);
}

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

void print_msystem(void) {
  const char *msystem = NULL;
  const char *short_msystem = NULL;
  if (!(msystem = get_msystem()))
    return;
  if (!strcmp(msystem, "CLANG32"))
    short_msystem = "C32";
  else if (!strcmp(msystem, "CLANG64"))
    short_msystem = "C64";
  else if (!strcmp(msystem, "CLANGARM64"))
    short_msystem = "CA64";
  else if (!strcmp(msystem, "MINGW32"))
    short_msystem = "M32";
  else if (!strcmp(msystem, "MINGW64"))
    short_msystem = "M64";
  else if (!strcmp(msystem, "MSYS")) {
  } else if (!strcmp(msystem, "UCRT64"))
    short_msystem = "U64";
  else
    short_msystem = "*";
  if (!short_msystem)
    return;
  seg(15, 234);
  printf(" " WINDOWS_ICON " %s ", short_msystem);
}

void print_git_branch(void) {
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
      if (strlen(print_from) >= 16) {
        /* trim branch if too long */
        print_from[13] = '.', print_from[14] = '.', print_from[15] = '.',
        print_from[16] = '\0';
      }
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
  (void)(argc);
  get_dims();
  short_fmt = term_width < 80 || getenv("PROMPT_COMPACT");
  sscanf(argv[1], "%i", &exit_code);
  seg_begin();
  print_msystem();
  if (!short_fmt) {
    print_login_hostname();
  }
  print_wd();
  print_git_branch();
  if (exit_code != 0) {
    print_exit(exit_code);
  }
  seg_user_color();
  print_hash();
  seg_end();
  return 0;
}
