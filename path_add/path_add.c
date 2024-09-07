#include "aparse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char* const* argv)
{
  ap* par = ap_init(argv[0]);
  const char *env_var = "PATH", *env_var_contents = NULL;
  const char *part = NULL, *part_next = NULL, *part_new = NULL;
  int ret = 0;
  ap_opt(par, 'e', "env-var");
  ap_type_str(par, &env_var);

  ap_pos(par, "PART");
  ap_type_str(par, &part_new);

  ap_opt(par, 'h', "help");
  ap_type_help(par);

  if ((ret = ap_parse(par, argc - 1, argv + 1)))
    goto error;

  env_var_contents = getenv(env_var);
  if (!env_var_contents) {
    perror("unable to read environment variable");
    ret = 1;
    goto error;
  }

  if (!part_new) {
    perror("unspecified part");
    ret = 1;
    goto error;
  }
  printf("export PATH=\"");
  part = env_var_contents;
  while (part && *part != '\0') {
    part_next = strchr(part, ':');
    if (part_next == NULL)
      part_next = part + strlen(part);
    if (strlen(part_new) == (size_t)(part_next - part) &&
        !memcmp(part, part_new, (size_t)(part_next - part)))
      goto done;
    part = (*part_next == ':') ? part_next + 1 : part_next;
  }
  printf("%s:", part_new);
done:
  printf("%s\"", env_var_contents);
error:
  ap_destroy(par);
  return ret;
}
