#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>

#define APARSE_IMPLEMENTATION
#include <aparse.h>

/* Get value of MSYSTEM environment variable. If unavailable, return NULL. */
const char *get_msystem(void) {
  static int detected = -1;
  static const char *msystem = NULL;
  if (detected == -1) {
    msystem = getenv("MSYSTEM");
    detected = !!msystem;
  }
  return msystem;
}

/* Are we running on MSYS? */
int is_msys(void) { return !!get_msystem(); }

/* Are we running on WSL? */
int is_wsl(void) {
  static int detected = -1;
  if (detected == -1) {
    struct utsname u;
    if (uname(&u))
      return detected = 0;
    /* if "microsoft" in release name, good chance we're WSL */
    detected = !!strstr(u.release, "microsoft");
  }
  return detected;
}

#define SEG_SEP "\xee\x82\xb0"
#define SEG_SEP_MINOR "\xee\x82\xb1"
#define BRANCH_ICON "\xee\x9c\xa5"
#define WINDOWS_ICON "\xee\x98\xaa"
#define GEAR_ICON "\xf3\xb0\x92\x93"

int last_seg_bg = -1;

int term_width = 80;
int term_height = 24;

int short_fmt = 0;

void fg(int fg) { printf("\\[\x1b[38;5;%im\\]", fg); }

void bg(int bg) { printf("\\[\x1b[48;5;%im\\]", bg); }

void clear(void) { printf("\\[\x1b[0m\\]"); }

void end(void) {
  clear();
  printf(" ");
  fflush(stdout);
}

void print_sep(void) { printf(" "); }

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
  clear();
  fg(15);
  printf("%s", buf2);
  fg(245);
  printf("@");
  fg(105);
  printf("%s", buf);
  free(buf);
  free(buf2);
  clear();
  print_sep();
}

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
    {
      char *out = obuf + strlen(obuf);
      const char *in = windows_drive + 1;
      while (*in) {
        if (*in == '/') {
          *(out++) = '\\';
          *(out++) = '\\';
        } else
          *(out++) = *in;
        in++;
      }
    }
  } else {
    if (strncmp(home, wd, strlen(home)) == 0) {
      strcat(obuf, "~");
      wd += strlen(home);
    }
    strcat(obuf, wd);
  }
  fg(15);
  if (!short_fmt) {
    print_path(obuf);
  } else {
    char *last_slash = strrchr(obuf, windows_drive ? '\\' : '/');
    if (!last_slash) {
      print_path(obuf);
    } else if (last_slash == obuf && !*(last_slash + 1)) {
      /* "/" */
      print_path(obuf);
    } else {
      print_path(last_slash + 1);
    }
  }
  print_sep();
  clear();
}

void print_exit(int exit_code) {
  fg(9);
  printf("%i ", exit_code);
}

void print_hash(void) {
  clear();
  if (getuid() == 0) {
    fg(196);
    printf("#");
  } else {
    fg(15);
    printf("$");
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
  fg(135);
  printf("%s" WINDOWS_ICON " ", short_msystem);
}

void print_job_count(int job_count) {
  if (job_count) {
    fg(69);
    printf("%i" GEAR_ICON " ", job_count);
  }
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
    if (git_rank > 1) {
      fg(250);
      printf("%i", git_rank);
    }
    fg(243);
    printf(BRANCH_ICON);
    if (!short_fmt) {
      fg(117);
      if (strlen(print_from) >= 16) {
        /* trim branch if too long */
        print_from[13] = '.', print_from[14] = '.', print_from[15] = '.',
        print_from[16] = '\0';
      }
      printf("%s", print_from);
    }
    clear();
    printf(" ");
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

int main(int argc, const char *const *argv) {
  int exit_code = 0;
  int job_count = 0;
  ap *parser;
  (void)(argc);
  parser = ap_init(argv[0]);
  ap_opt(parser, 'x', "exit-status");
  ap_type_int(parser, &exit_code);
  ap_help(parser, "exit status of previous command");
  ap_opt(parser, 'j', "job-count");
  ap_type_int(parser, &job_count);
  ap_help(parser, "number of background jobs");
  ap_parse(parser, argc - 1, argv + 1);
  get_dims();
  short_fmt = term_width < 80 || getenv("PROMPT_COMPACT");
  print_msystem();
  print_job_count(job_count);
  if (!short_fmt) {
    print_login_hostname();
  }
  print_wd();
  print_git_branch();
  if (exit_code != 0) {
    print_exit(exit_code);
  }
  print_hash();
  end();
  return 0;
}
