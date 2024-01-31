#include <assert.h>
#include <ctype.h>
#include <dirent.h>
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

#define BRANCH_ICON "\xee\x9c\xa5"
#define WINDOWS_ICON "\xee\x98\xaa"
#define GEAR_ICON "\xf3\xb0\x92\x93"

#define C_WHITE 15
#define C_GRAY 245
#define C_LIGHTGRAY 250
#define C_RED 9
#define C_PURPLE 69
#define C_LIGHTBLUE 117
#define C_LIGHTRED 196

#define SIFY(x) #x
#define FG(x) "\\[\x1b[38;5;" SIFY(x) "m\\]"
#define FG_FMT "\\[\x1b[38;5;%im\\]"
#define ITAL "\\[\x1b[3m\\]"
#define CLR "\\[\x1b[0m\\]"
#define SEP " "

int term_width = 80;
int term_height = 24;

int short_fmt = 0;

void print_sep(void) { printf(SEP); }

void end(void) {
  printf(CLR);
  fflush(stdout);
}

void print_login_hostname(void) {
  char *hostname_buf, *username_buf, *first_dot, *color;
  struct passwd pwd, *out;
  int hostname_fg = 105;
  hostname_buf = malloc(1024);
  username_buf = malloc(1024);
  *hostname_buf = '\0';
  getpwuid_r(getuid(), &pwd, username_buf, 1024, &out);
  gethostname(hostname_buf, 1024);
  if ((first_dot = strchr(hostname_buf, '.')))
    *first_dot = '\0';
  color = getenv("PROMPT_USER_COLORS");
  if (color)
    sscanf(color, "%i", &hostname_fg);
  printf(FG(C_WHITE) "%s" FG(C_GRAY) "@" FG_FMT "%s" CLR SEP, username_buf,
         hostname_fg, hostname_buf);
  free(hostname_buf);
  free(username_buf);
}

void print_wd(void) {
  char *path = malloc(PATH_MAX), *win_drv = NULL, current_path[PATH_MAX],
       *prefix = path, *next, *currp = current_path;
  DIR *current_dir = NULL;
  int should_shorten = short_fmt;
  if (!getcwd(path, PATH_MAX)) {
    printf(FG(C_RED) "<unlinked>" CLR SEP);
    return;
  } else if ((is_wsl() && strstr(path, "/mnt/") == path && path[5] &&
              (path[6] == '/' || !path[6])) ||
             (is_msys() && path[1] && (path[2] == '/' || !path[2]))) {
    /* wsl or msys: backwards slashes, print drive letter */
    path = win_drv + 1;
    printf(FG(C_LIGHTGRAY) "%c:", *win_drv);
  } else {
    /* attempt to match $HOME with string prefix */
    char *home = getenv("HOME");
    if (!strncmp(home, path, strlen(home))) {
      path = path + strlen(home);
      printf(FG(C_LIGHTGRAY) "~");
    }
  }
  strcpy(current_path, prefix);
  currp += path - prefix;
  while (*path) {
    const char *max_prefix = should_shorten ? path + 1 : NULL;
    struct dirent *ent;
    assert(*path == '/'); /* path is normalized (all segments start with '/') */
    *(currp++) = *(path++); /* pop slash */
    *currp = '\0';
    if (should_shorten) {
      current_dir = opendir(current_path);
      if (!current_dir) {
        /* unable to open directory */
        printf(FG(C_RED) "!!" CLR SEP);
        return;
      }
      while ((ent = readdir(current_dir))) {
        size_t i = 0;
        while (1) {
          int name_end = !path[i] || path[i] == '/';
          int entry_end = !ent->d_name[i];
          if (name_end && entry_end)
            /* strings equal */
            break;
          else if (name_end)
            /* name shorter than entry */
            break;
          else if (entry_end || tolower(path[i]) != tolower(ent->d_name[i])) {
            /* entry is a prefix of name */
            max_prefix = path + i + 1 > max_prefix ? path + i + 1 : max_prefix;
            break;
          }
          i++;
        }
      }
    }
    next = (next = strrchr(path, '/')) ? next : path + strlen(path);
    printf(FG(C_GRAY) "%s", (win_drv ? "\\" : "/"));
    {
      size_t num_remaining;
      if (should_shorten && (num_remaining = next - max_prefix) > 2)
        /* print shortened segment */
        printf(FG(C_WHITE) "%.*s" FG(C_GRAY) ITAL "~%u" CLR,
               (int)(max_prefix - path), path, (unsigned)num_remaining);
      else
        /* print normal segment */
        printf(FG(C_WHITE) "%.*s" FG(C_GRAY), (int)(next - path), path);
    }
    path = next;
  }
  printf(CLR SEP);
}

void print_exit_status(int exit_code) {
  printf(FG(C_RED) "%i" CLR SEP, exit_code);
}

void print_hash(void) {
  printf("%s" CLR SEP, (getuid() == 0) ? FG(C_LIGHTRED) "#" : FG(C_WHITE) "$");
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
  printf(FG(C_PURPLE) "%s" WINDOWS_ICON CLR SEP, short_msystem);
}

void print_job_count(int job_count) {
  if (job_count)
    printf(FG(C_PURPLE) "%i" GEAR_ICON CLR SEP, job_count);
}

void print_git_branch(void) {
  char *wd = malloc(PATH_MAX), *wd_buf = wd;
  char *path_buf = malloc(PATH_MAX);
  int git_rank = 0;
  char head_ptr_buf[256] = {0};
  assert(wd);
  if (!getcwd(wd, PATH_MAX))
    goto done;
  /* suffix with / to normalize loop */
  strcat(wd, "/");
  while (*wd) {
    struct stat dir_stat_buf;
    /* copy path to temp */
    strcpy(path_buf, wd_buf);
    /* suffix path with .git */
    strcpy(path_buf + (wd - wd_buf) + 1, ".git");
    if (stat(path_buf, &dir_stat_buf) == 0 && S_ISDIR(dir_stat_buf.st_mode)) {
      struct stat head_stat_buf;
      /* suffix path with HEAD */
      strcat(path_buf, "/HEAD");
      if (stat(path_buf, &head_stat_buf) == 0 &&
          S_ISREG(head_stat_buf.st_mode)) {
        /* HEAD is readable, attempt to open it and read its pointer */
        int hfd = open(path_buf, O_RDONLY);
        if (hfd != -1) {
          read(hfd, head_ptr_buf, 256);
          close(hfd);
        }
        git_rank++;
      }
    }
    do {
      /* advance to next dir: read until next / */
      wd++;
    } while (*wd && *wd != '/');
  }
  if (head_ptr_buf[0]) {
    /* if we found any branch */
    char *last_slash = strrchr(head_ptr_buf, '/'), *output = head_ptr_buf, *nl;
    if ((nl = strchr(head_ptr_buf, '\n')))
      /* remove newlines from */
      *nl = '\0';
    if (last_slash)
      /* strip refs/.../... */
      output = last_slash + 1;
    if (git_rank > 1)
      printf(FG(C_LIGHTGRAY) "%i" CLR, git_rank);
    if (!short_fmt) {
      if (strlen(output) >= 16)
        /* trim branch if too long */
        output[13] = '.', output[14] = '.', output[15] = '.', output[16] = '\0';
    } else
      output = "";
    printf(FG(C_GRAY) BRANCH_ICON FG(C_LIGHTBLUE) "%s" CLR SEP, output);
  }
done:
  free(wd_buf);
  free(path_buf);
}

void get_dims(void) {
  struct winsize sz;
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &sz) == -1)
    return;
  term_width = sz.ws_col, term_height = sz.ws_row;
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
  if (!short_fmt)
    print_login_hostname();
  print_wd();
  print_git_branch();
  if (exit_code != 0)
    print_exit_status(exit_code);
  print_hash();
  end();
  return 0;
}
