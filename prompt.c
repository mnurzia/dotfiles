#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <pwd.h>
#include <stdarg.h>
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

#define BRANCH_ICON  "\xee\x9c\xa5"
#define WINDOWS_ICON "\xee\x98\xaa"
#define GEAR_ICON    "\xf3\xb0\x92\x93"
#define PYTHON_ICON  "\xee\x98\x86"

#define C_GRAY        245
#define C_LIGHTBLUE   117
#define C_LIGHTGRAY   250
#define C_LIGHTPURPLE 105
#define C_LIGHTRED    196
#define C_PURPLE      69
#define C_RED         9
#define C_WHITE       15
#define C_YELLOW      228

#define SIFY(x) #x
#define FG(x)   "\x1b[38;5;" SIFY(x) "m"
#define FG_FMT  "\x1b[38;5;%im"
#define ITAL    "\x1b[3m"
#define CLR     "\x1b[0m"
#define SEP     " "

int term_width = 80;
int term_height = 24;

int short_fmt = 0;

typedef enum escape_pref { ESCAPE_RAW, ESCAPE_BASH, ESCAPE_ZSH } escape_pref;
escape_pref escape_mode = ESCAPE_RAW;

/* for convenience, we preprocess fmt strings passed to printf, this type is
 * useful to us in that process */
typedef struct str {
  char* ptr;
  size_t size, len;
} str;

/* append n bytes to str */
void appendn(str* s, const char* a, size_t sz)
{
  while (sz--) {
    if (s->size == s->len) {
      s->size = s->size ? s->size * 2 : 1;
      s->ptr = realloc(s->ptr, s->size);
    }
    s->ptr[s->len++] = *(a++);
  }
}

/* append null-terminated string to str */
void append(str* s, const char* a) { appendn(s, a, strlen(a)); }

/* printf() but escapes SGR escape sequences */
void out(const char* fmt, ...)
{
  va_list args;
  str prompt = {0};
  while (*fmt) {
    if (*fmt == '\x1b') {
      if (escape_mode == ESCAPE_BASH)
        append(&prompt, "\\[");
      else if (escape_mode == ESCAPE_ZSH)
        append(&prompt, "%%{");
      while (*fmt != 'm') {
        appendn(&prompt, fmt, 1);
        fmt++;
      }
      appendn(&prompt, fmt, 1);
      if (escape_mode == ESCAPE_BASH)
        append(&prompt, "\\]");
      else if (escape_mode == ESCAPE_ZSH)
        append(&prompt, "%%}");
    } else {
      appendn(&prompt, fmt, 1);
    }
    fmt++;
  }
  appendn(&prompt, "", 1);
  va_start(args, fmt);
  vprintf(prompt.ptr, args);
  va_end(args);
  free(prompt.ptr);
}

/* finish prompt */
void end(void)
{
  out(CLR);
  fflush(stdout);
}

/* detect shell for escaping */
void detect_shell(const char* in_shell)
{
  if (!strcmp(in_shell, "bash"))
    escape_mode = ESCAPE_BASH;
  else if (!strcmp(in_shell, "zsh"))
    escape_mode = ESCAPE_ZSH;
  else if (!strcmp(in_shell, "raw"))
    escape_mode = ESCAPE_RAW;
  else if (!strcmp(in_shell, "auto")) {
    const char* shell_env = getenv("SHELL");
    if (!shell_env)
      escape_mode = ESCAPE_RAW;
    else if (strstr(shell_env, "bash"))
      escape_mode = ESCAPE_BASH;
    else if (strstr(shell_env, "zsh"))
      escape_mode = ESCAPE_ZSH;
    else
      escape_mode = ESCAPE_RAW;
  }
}

/* Get value of MSYSTEM environment variable. If unavailable, return NULL. */
const char* get_msystem(void)
{
  static int detected = -1;
  static const char* msystem = NULL;
  if (detected == -1) {
    msystem = getenv("MSYSTEM");
    detected = !!msystem;
  }
  return msystem;
}

/* Are we running on MSYS? */
int is_msys(void) { return !!get_msystem(); }

/* Are we running on WSL? */
int is_wsl(void)
{
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

/* print login name and hostname, up until first . */
void print_login_hostname(void)
{
  char *hostname_buf, *username_buf, *first_dot, *color;
  struct passwd pwd, *pwd_out;
  int hostname_fg = C_LIGHTPURPLE;
  hostname_buf = malloc(1024);
  username_buf = malloc(1024);
  *hostname_buf = '\0';
  getpwuid_r(getuid(), &pwd, username_buf, 1024, &pwd_out);
  gethostname(hostname_buf, 1024);
  if ((first_dot = strchr(hostname_buf, '.')))
    *first_dot = '\0';
  color = getenv("PROMPT_USER_COLORS");
  if (color)
    sscanf(color, "%i", &hostname_fg);
  out(FG(C_WHITE) "%s" FG(C_GRAY) "@" FG_FMT "%s" CLR SEP, username_buf,
      hostname_fg, hostname_buf);
  free(hostname_buf);
  free(username_buf);
}

/* print working directory intelligently, shortening as much as possible */
void print_wd(void)
{
  char *path = malloc(PATH_MAX), *win_drv = NULL, current_path[PATH_MAX],
       *prefix = path, *next;
  DIR* current_dir = NULL;
  int should_shorten = short_fmt;
  if (!getcwd(path, PATH_MAX)) {
    out(FG(C_RED) "<unlinked>" CLR SEP);
    return;
  } else if (
      (is_wsl() && strstr(path, "/mnt/") == path && path[5] &&
       (path[6] == '/' || !path[6])) ||
      (is_msys() && path[1] && (path[2] == '/' || !path[2]))) {
    /* wsl or msys: backwards slashes, print drive letter */
    path = win_drv + 1;
    out(FG(C_LIGHTGRAY) "%c:", *win_drv);
  } else {
    /* attempt to match $HOME with string prefix */
    char* home = getenv("HOME");
    if (!strncmp(home, path, strlen(home))) {
      path = path + strlen(home);
      out(FG(C_LIGHTGRAY) "~");
    }
  }
  strcpy(current_path, prefix);
  while (*path) {
    const char* max_prefix = should_shorten ? path + 1 : NULL;
    struct dirent* ent;
    assert(*path == '/'); /* path is normalized (all segments start with '/') */
    path++; /* pop slash */
    memset(current_path, 0, sizeof(current_path));
    memcpy(current_path, prefix, path - prefix);
    if (should_shorten) {
      current_dir = opendir(current_path);
      if (!current_dir) {
        /* unable to open directory */
        out(FG(C_RED) "!!" CLR SEP);
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
    next = strchr(path, '/');
    if (!next)
      next = path + strlen(path);
    out(FG(C_GRAY) "%s", (win_drv ? "\\" : "/"));
    {
      size_t num_remaining;
      if (should_shorten && (num_remaining = next - max_prefix) > 1) {
        /* print shortened segment */
        char remain_buf[128] = {0};
        char num_buf[64] = {0}, *num_buf_ptr = num_buf;
        static const char* sup[] = {
            /* unicode superscripts 0-9 */
            "\xe2\x81\xb0", "\xc2\xb9",     "\xc2\xb2",     "\xc2\xb3",
            "\xe2\x81\xb4", "\xe2\x81\xb5", "\xe2\x81\xb6", "\xe2\x81\xb7",
            "\xe2\x81\xb8", "\xe2\x81\xb9"};
        sprintf(num_buf, "%u", (unsigned)num_remaining);
        while (*num_buf_ptr) {
          strcat(remain_buf, sup[*num_buf_ptr - '0']);
          num_buf_ptr++;
        }
        out(FG(C_WHITE) "%.*s" FG(C_GRAY) ITAL "%s" CLR,
            (int)(max_prefix - path), path, remain_buf);
      } else
        /* print normal segment */
        out(FG(C_WHITE) "%.*s" FG(C_GRAY), (int)(next - path), path);
    }
    path = next;
  }
  out(CLR SEP);
}

/* print exit status of previous command */
void print_exit_status(int exit_code)
{
  out(FG(C_RED) "%i" CLR SEP, exit_code);
}

/* print # or $ depending on whether or not we're root */
void print_hash(void)
{
  int is_root = !getuid();
  out(FG_FMT "%s" CLR SEP, is_root ? FG(C_LIGHTRED) : FG(C_WHITE),
      is_root ? "#" : "$");
}

/* if on mingw/msys2, print the value of the MSYS environment variable */
void print_msystem(void)
{
  const char* msystem = NULL;
  const char* short_msystem = NULL;
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
  out(FG(C_PURPLE) WINDOWS_ICON "%s" CLR SEP, short_msystem);
}

/* if in a python venv, print its name */
void print_venv(void)
{
  const char* venv = getenv("VIRTUAL_ENV_PROMPT");
  if (!venv)
    return;
  out(FG(C_YELLOW) PYTHON_ICON "%s" CLR SEP, short_fmt ? "" : venv);
}

/* if background jobs are running, print the amount of them */
void print_job_count(int job_count)
{
  if (job_count)
    out(FG(C_PURPLE) GEAR_ICON "%i" CLR SEP, job_count);
}

/* print the currently checked-out git branch */
void print_git_branch(void)
{
  char *wd = malloc(PATH_MAX), *wd_buf = wd;
  char* path_buf = malloc(PATH_MAX);
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
      out(FG(C_LIGHTGRAY) "%i" CLR, git_rank);
    if (!short_fmt) {
      if (strlen(output) >= 16)
        /* trim branch if too long */
        output[13] = '.', output[14] = '.', output[15] = '.', output[16] = '\0';
    } else
      output = "";
    out(FG(C_GRAY) BRANCH_ICON FG(C_LIGHTBLUE) "%s" CLR SEP, output);
  }
done:
  free(wd_buf);
  free(path_buf);
}

/* get the dimensions of the calling terminal */
void get_dims(void)
{
  struct winsize sz;
  if (ioctl(STDIN_FILENO, TIOCGWINSZ, &sz) == -1)
    return;
  term_width = sz.ws_col, term_height = sz.ws_row;
}

void print_mode(const char* mode)
{
  int color = C_LIGHTBLUE;
  const char* text = "";
  if (!strcmp(mode, "main"))
    color = C_LIGHTBLUE, text = "INSERT";
  else if (!strcmp(mode, "vicmd"))
    color = C_LIGHTPURPLE, text = "COMMAND";
  else if (!strcmp(mode, "viopp"))
    color = C_LIGHTRED, text = "O-PENDING";
  else if (!strcmp(mode, "visual"))
    color = C_LIGHTGRAY, text = "VISUAL";
  else if (!strcmp(mode, "isearch"))
    color = C_LIGHTRED, text = "ISEARCH";
  out(FG_FMT "%s", color, text);
}

int main(int argc, const char* const* argv)
{
  int status = 0;
  int exit_code = 0;
  int job_count = 0;
  ap* parser;
  const char* shell = "auto";
  const char* prompt_type = "left";
  const char* mode = "main";

  parser = ap_init(argv[0]);

  ap_opt(parser, 'p', "prompt");
  ap_type_str(parser, &prompt_type);
  ap_help(parser, "prompt to generate: options are left, right");
  ap_metavar(parser, "PROMPT");

  ap_opt(parser, 'x', "exit-status");
  ap_type_int(parser, &exit_code);
  ap_help(parser, "exit status of previous command");
  ap_metavar(parser, "EXITSTATUS");

  ap_opt(parser, 'j', "job-count");
  ap_type_int(parser, &job_count);
  ap_help(parser, "number of background jobs");
  ap_metavar(parser, "JOBCOUNT");

  ap_opt(parser, 's', "shell");
  ap_type_str(parser, &shell);
  ap_help(parser, "shell mode for escaping: options are bash, zsh, raw, auto");
  ap_metavar(parser, "SHELL");

  ap_opt(parser, 'm', "mode");
  ap_type_str(parser, &mode);
  ap_help(parser, "editing mode");
  ap_metavar(parser, "MODE");

  ap_opt(parser, 'h', "help");
  ap_type_help(parser);
  ap_help(parser, "show this help text");

  if ((status = ap_parse(parser, argc - 1, argv + 1))) {
    status = status != AP_ERR_EXIT;
    goto done;
  }
  get_dims();
  detect_shell(shell);
  short_fmt = term_width < 80 || getenv("PROMPT_COMPACT");
  if (!strcmp(prompt_type, "left")) {
    print_msystem();
    print_venv();
    print_job_count(job_count);
    if (!short_fmt)
      print_login_hostname();
    print_wd();
    print_git_branch();
    if (exit_code != 0)
      print_exit_status(exit_code);
    print_hash();
  } else if (!strcmp(prompt_type, "right")) {
    print_mode(mode);
  }
  end();
done:
  return status;
}
