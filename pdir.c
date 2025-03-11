#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "include/aparse.h"

#define DEFAULT_FMT "%Y_%m_%d_"

int main(int argc, const char* const* argv)
{
  ap* par = ap_init(argv[0]);
  const char *name = NULL, *fmt = DEFAULT_FMT;
  int rv, dry_run = 0;
  char out[PATH_MAX];
  time_t current_time_posix;
  struct tm* current_time;
  ap_pos(par, "NAME");
  ap_type_str(par, &name);
  ap_help(par, "name of project");
  ap_opt(par, 'f', "format");
  ap_type_str(par, &fmt);
  ap_help(par, "format as passed to strftime(3), default " DEFAULT_FMT);
  ap_opt(par, 'd', "dry-run");
  ap_type_flag(par, &dry_run);
  ap_help(
      par, "don't actually create the directory, print actions to be taken");
  ap_opt(par, 'h', "help");
  ap_type_help(par);
  ap_help(par, "show this help text and exit");
  rv = ap_parse(par, argc - 1, argv + 1);
  if (rv == AP_ERR_EXIT) {
    exit(0);
  } else if (rv != AP_ERR_NONE) {
    goto done;
  }
  current_time_posix = time(NULL);
  if (current_time_posix == (time_t)-1) {
    perror("time");
    rv = 1;
    goto done;
  }
  current_time = localtime(&current_time_posix);
  if (current_time == NULL) {
    perror("localtime");
    rv = 1;
    goto done;
  }
  if (!strftime(out, sizeof(out), fmt, current_time)) {
    perror("strftime");
    rv = 1;
    goto done;
  }
  if (!name) {
    fprintf(stderr, "expected name for project directory\n");
    rv = 1;
    goto done;
  }
  strncat(out, name, sizeof(out) - strlen(out) - 1);
  if (dry_run) {
    printf("mkdir(\"%s\")", out);
  } else if (mkdir(out, umask(0777)) == -1) {
    perror("mkdir");
    rv = 1;
    goto done;
  }
  ap_destroy(par);
done:
  return rv;
}
