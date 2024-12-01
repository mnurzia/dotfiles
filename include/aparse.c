#include "aparse.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AP_ARG_FLAG_OPT 1
#define AP_ARG_FLAG_REQUIRED 2
#define AP_ARG_FLAG_SUB 4
#define AP_ARG_FLAG_COALESCE 8
#define AP_ARG_FLAG_DESTRUCTOR 16

typedef struct ap_arg ap_arg;

struct ap_arg {
  int flags;
  ap_arg *next;         /* next argument in list */
  ap_cb cb;             /* callback */
  const char *metavar;  /* metavar */
  const char *help;     /* help text */
  const char *opt_long; /* long opt */
  char opt_short;       /* if short option, option character */
  void *user;           /* user pointer */
  void *user1;          /* second user pointer (convenient) */
};

/* parent linked list used in argument search order */
typedef struct ap_parent ap_parent;
struct ap_parent {
  ap *par;
  ap_parent *next;
};

/* subparser linked list used in subparser search order */
typedef struct ap_sub ap_sub;
struct ap_sub {
  const char *identifier; /* command name */
  ap *par;
  ap_sub *next;
};

/* argument parser */
struct ap {
  const ap_ctxcb *ctxcb;   /* context callbacks (replicated in subparsers) */
  const char *progname;    /* argv[0] */
  ap_arg *args;            /* argument list */
  ap_arg *args_tail;       /* end of argument list */
  ap_arg *current;         /* current arg under modification */
  ap_parent *parents;      /* parent list for argument search order */
  const char *description; /* help description */
  const char *epilog;      /* help epilog */
};

void *ap_cb_malloc(ap *parser, size_t n) {
  return parser->ctxcb->malloc ? parser->ctxcb->malloc(parser->ctxcb->uptr, n)
                               : malloc(n);
}

void ap_cb_free(ap *parser, void *ptr) {
  parser->ctxcb->free ? parser->ctxcb->free(parser->ctxcb->uptr, ptr)
                      : free(ptr);
}

void *ap_cb_realloc(ap *parser, void *ptr, size_t n) {
  return parser->ctxcb->realloc
             ? parser->ctxcb->realloc(parser->ctxcb->uptr, ptr, n)
             : realloc(ptr, n);
}

int ap_cb_out(ap *parser, const char *text, size_t n) {
  return parser->ctxcb->out
             ? parser->ctxcb->out(parser->ctxcb->uptr, text, n)
             : (fwrite(text, 1, n, stdout) < n ? AP_ERR_IO : AP_ERR_NONE);
}

int ap_out(ap *par, const char *text) {
  return ap_cb_out(par, text, strlen(text));
}

int ap_outc(ap *par, char text) { return ap_cb_out(par, &text, 1); }

int ap_feed(ap *par) { return ap_out(par, "\n"); }

int ap_cb_err(ap *parser, const char *text, size_t n) {
  return parser->ctxcb->err
             ? parser->ctxcb->err(parser->ctxcb->uptr, text, n)
             : (fwrite(text, 1, n, stderr) < n ? AP_ERR_IO : AP_ERR_NONE);
}

int ap_err(ap *par, const char *text) {
  return ap_cb_err(par, text, strlen(text));
}

static const ap_ctxcb ap_default_ctxcb = {NULL, NULL, NULL, NULL, NULL, NULL};

ap *ap_init(const char *progname) {
  ap *out;
  return (ap_init_full(&out, progname, NULL) == AP_ERR_NONE) ? out : NULL;
}

int ap_init_full(ap **out, const char *progname, const ap_ctxcb *pctxcb) {
  ap *par;
  pctxcb = pctxcb ? pctxcb : &ap_default_ctxcb;
  par = pctxcb->malloc ? pctxcb->malloc(pctxcb->uptr, sizeof(ap))
                       : malloc(sizeof(ap));
  if (!par)
    return AP_ERR_NOMEM;
  par->ctxcb = pctxcb;
  par->progname = progname;
  par->args = NULL;
  par->args_tail = NULL;
  par->current = NULL;
  par->parents = NULL;
  *out = par;
  return AP_ERR_NONE;
}

void ap_destroy(ap *par) {
  while (par->args) {
    ap_arg *prev = par->args;
    if (prev->flags & AP_ARG_FLAG_SUB) {
      ap_sub *sub = (ap_sub *)prev->user;
      while (sub) {
        ap_sub *prev_sub = sub;
        ap_destroy(prev_sub->par);
        sub = sub->next;
        ap_cb_free(par, prev_sub);
      }
    } else if (prev->flags & AP_ARG_FLAG_DESTRUCTOR) {
      ap_cb_data data = {0};
      data.destroy = 1;
      data.parser = par;
      prev->cb(prev->user, &data);
    }
    par->args = par->args->next;
    ap_cb_free(par, prev);
  }
  ap_cb_free(par, par);
}

void ap_description(ap *par, const char *description) {
  par->description = description;
}

void ap_epilog(ap *par, const char *epilog) { par->epilog = epilog; }

int ap_begin(ap *par) {
  ap_arg *next = (ap_arg *)ap_cb_malloc(par, sizeof(ap_arg));
  if (!next)
    return AP_ERR_NOMEM;
  memset(next, 0, sizeof(*next));
  if (!par->args) /* first argument, initialize list */
    par->args = next, par->args_tail = next;
  else /* next argument, link to end */
    par->args_tail->next = next, par->args_tail = next;
  par->current = next;
  return AP_ERR_NONE;
}

int ap_pos(ap *par, const char *metavar) {
  int err = 0;
  if ((err = ap_begin(par)))
    return err;
  par->current->flags = 0;
  par->current->metavar = metavar;
  return 0;
}

int ap_opt(ap *par, char short_opt, const char *long_opt) {
  int err = 0;
  if ((err = ap_begin(par)))
    return err;
  /* if this fails, you didn't specify a short or long opt */
  assert(short_opt != 0 || long_opt != NULL);
  par->current->flags = AP_ARG_FLAG_OPT;
  par->current->opt_short = short_opt;
  par->current->opt_long = long_opt;
  return 0;
}

void ap_check_arg(ap *par) {
  /* if this fails, you forgot to call ap_pos or ap_opt */
  assert(par->current);
}

void ap_help(ap *par, const char *help) {
  ap_check_arg(par);
  par->current->help = help;
}

void ap_metavar(ap *par, const char *metavar) {
  ap_check_arg(par);
  par->current->metavar = metavar;
}

void ap_type_sub(ap *par, const char *metavar, int *out_idx) {
  ap_check_arg(par);
  /* if this fails, you called ap_type_sub() twice */
  assert(!(par->current->flags & AP_ARG_FLAG_SUB));
  par->current->flags |= AP_ARG_FLAG_SUB;
  par->current->metavar = metavar;
  par->current->user1 = (void *)out_idx;
}

int ap_sub_add(ap *par, const char *name, ap **subpar) {
  int err = 0;
  ap_sub *sub = ap_cb_malloc(par, sizeof(ap_sub));
  ap_check_arg(par);
  if (!sub)
    return AP_ERR_NOMEM;
  if ((err = ap_init_full(subpar, NULL, par->ctxcb))) {
    ap_cb_free(par, sub);
    return err;
  }
  sub->identifier = name;
  sub->next = par->current->user;
  sub->par = *subpar;
  par->current->user = sub;
  return 0;
}

void ap_end(ap *par) {
  ap_check_arg(par);
  if (par->current->flags & AP_ARG_FLAG_SUB)
    /* if this fails, then you didn't add any subparsers with ap_sub_add() */
    assert(par->current->user);
  else
    /* if this fails, there wasn't an ap_type_xxx call on the argument */
    assert(!!par->current->cb);
  par->current = NULL;
}

void ap_type_custom(ap *par, ap_cb callback, void *user) {
  ap_check_arg(par);
  par->current->cb = callback;
  par->current->user = user;
}

void ap_custom_dtor(ap *par, int enable) {
  int flag = enable * AP_ARG_FLAG_DESTRUCTOR;
  ap_check_arg(par);
  assert(enable == 0 || enable == 1);
  par->current->flags = (par->current->flags & ~AP_ARG_FLAG_DESTRUCTOR) | flag;
}

int ap_flag_cb(void *uptr, ap_cb_data *pdata) {
  int *out = (int *)uptr;
  (void)(pdata);
  *out = 1;
  return 0;
}

void ap_type_flag(ap *par, int *out) {
  ap_type_custom(par, ap_flag_cb, (void *)out);
}

int ap_int_cb(void *uptr, ap_cb_data *pdata) {
  if (!sscanf(pdata->arg, "%i", (int *)uptr))
    return ap_arg_error(pdata, "invalid integer argument");
  return pdata->arg_len;
}

void ap_type_int(ap *par, int *out) {
  ap_type_custom(par, ap_int_cb, (void *)out);
}

int ap_str_cb(void *uptr, ap_cb_data *pdata) {
  const char **out = (const char **)uptr;
  if (!pdata->arg)
    return ap_arg_error(pdata, "expected an argument");
  *out = pdata->arg;
  return pdata->arg_len;
}

void ap_type_str(ap *par, const char **out) {
  ap_type_custom(par, ap_str_cb, (void *)out);
}

typedef struct ap_enum {
  const char **choices;
  int *out;
  char *metavar;
} ap_enum;

int ap_enum_cb(void *uptr, ap_cb_data *pdata) {
  ap_enum *e = (ap_enum *)uptr;
  const char **cur;
  int i;
  if (pdata->destroy) {
    ap_cb_free(pdata->parser, e->metavar);
    ap_cb_free(pdata->parser, e);
    return AP_ERR_NONE;
  }
  for (cur = e->choices, i = 0; *cur; cur++, i++) {
    if (!strcmp(*cur, pdata->arg)) {
      *e->out = i;
      return pdata->arg_len;
    }
  }
  return ap_arg_error(pdata, "invalid choice for argument");
}

int ap_type_enum(ap *par, int *out, const char **choices) {
  ap_enum *e;
  /* don't pass NULL in */
  assert(choices);
  /* you must pass at least one choice */
  assert(choices[0]);
  e = ap_cb_malloc(par, sizeof(ap_enum));
  if (!e)
    return AP_ERR_NOMEM;
  memset(e, 0, sizeof(*e));
  e->choices = choices;
  e->out = out;
  {
    /* build metavar */
    size_t mvs = 2;
    const char **cur;
    char *metavar_ptr;
    for (cur = choices; *cur; cur++) {
      /* make space for comma + length of string */
      mvs += (mvs != 2) + strlen(*cur);
    }
    e->metavar = malloc(sizeof(char) * mvs + 1);
    if (!e->metavar) {
      ap_cb_free(par, e);
      return AP_ERR_NOMEM;
    }
    metavar_ptr = e->metavar;
    *(metavar_ptr++) = '{';
    for (cur = choices; *cur; cur++) {
      size_t l = strlen(*cur);
      if (metavar_ptr != e->metavar + 1)
        *(metavar_ptr++) = ',';
      memcpy(metavar_ptr, *cur, l);
      metavar_ptr += l;
    }
    *(metavar_ptr++) = '}';
    *(metavar_ptr++) = '\0';
  }
  ap_type_custom(par, ap_enum_cb, (void *)e);
  ap_metavar(par, e->metavar);
  ap_custom_dtor(par, 1);
  return AP_ERR_NONE;
}

int ap_help_cb(void *uptr, ap_cb_data *pdata) {
  int err;
  (void)uptr;
  (void)pdata;
  if ((err = ap_show_help(pdata->parser)))
    return err;
  return AP_ERR_EXIT;
}

void ap_type_help(ap *par) { ap_type_custom(par, ap_help_cb, NULL); }

int ap_version_cb(void *uptr, ap_cb_data *pdata) {
  int err;
  if ((err = ap_out(pdata->parser, (const char *)uptr)))
    return err;
  if ((err = ap_feed(pdata->parser)))
    return err;
  return AP_ERR_EXIT;
}

void ap_type_version(ap *par, const char *version) {
  ap_type_custom(par, ap_version_cb, (void *)version);
}

typedef struct ap_parser {
  int argc;
  const char *const *argv;
  int idx;
  int arg_idx;
  int arg_len;
} ap_parser;

void ap_parser_init(ap_parser *ctx, int argc, const char *const *argv) {
  ctx->argc = argc;
  ctx->argv = argv;
  ctx->idx = 0;
  ctx->arg_idx = 0;
  ctx->arg_len = (ctx->idx == ctx->argc) ? 0 : (int)strlen(ctx->argv[ctx->idx]);
}

void ap_parser_advance(ap_parser *ctx, int amt) {
  if (!amt)
    return;
  /* if this fails, you tried to run the parser backwards. this isn't possible
   * at the moment. */
  assert(amt > 0);
  /* if this fails, you tried to get more chars after exhausting input args. */
  assert(ctx->idx < ctx->argc);
  /* if this fails, you asked for too many characters from the same argument. */
  assert(amt <= (ctx->arg_len - ctx->arg_idx));
  ctx->arg_idx += amt;
  if (ctx->arg_idx == ctx->arg_len) {
    ctx->idx++;
    ctx->arg_idx = 0;
    ctx->arg_len =
        (ctx->idx == ctx->argc) ? 0 : (int)strlen(ctx->argv[ctx->idx]);
  }
}

const char *ap_parser_cur(ap_parser *ctx) {
  return (ctx->idx == ctx->argc || ctx->arg_idx == ctx->arg_len)
             ? NULL
             : ctx->argv[ctx->idx] + ctx->arg_idx;
}

int ap_parse_internal(ap *par, ap_parser *ctx);

int ap_parse_internal_part(ap *par, ap_arg *arg, ap_parser *ctx) {
  int cb_ret, cb_sub_idx = 0;
  if (!(arg->flags & AP_ARG_FLAG_SUB)) {
    ap_cb_data cbd = {0};
    do {
      cbd.arg = ap_parser_cur(ctx);
      cbd.arg_len = cbd.arg ? ctx->arg_len : 0;
      cbd.idx = cb_sub_idx++;
      cbd.more = 0;
      cbd.reserved = arg;
      cbd.parser = par;
      cb_ret = arg->cb(arg->user, &cbd);
      if (cb_ret < 0)
        /* callback encountered error in parse */
        return cb_ret;
      /* callbacks should always only parse up to end of string */
      assert(cb_ret <= cbd.arg_len);
      cbd.idx++;
      ap_parser_advance(ctx, cb_ret);
    } while (cbd.more);
  } else {
    ap_sub *sub = arg->user;
    /* if this fails, then you forgot to call ap_sub_add */
    assert(sub);
    if (!sub->identifier) {
      /* immediately trigger parsing */
      return ap_parse_internal(sub->par, ctx);
    } else {
      const char *cmp = ap_parser_cur(ctx);
      if (!cmp)
        return AP_ERR_PARSE;
      while (sub) {
        if (!strcmp(sub->identifier, cmp))
          goto found;
        sub = sub->next;
      }
      /* (error) couldn't find subparser */
      return AP_ERR_PARSE;
    found:
      ap_parser_advance(ctx, strlen(sub->identifier));
      return ap_parse_internal(sub->par, ctx);
    }
  }
  return AP_ERR_NONE;
}

ap_arg *ap_find_next_positional(ap_arg *arg) {
  while (arg && (arg->flags & AP_ARG_FLAG_OPT)) {
    arg = arg->next;
  }
  return arg;
}

int ap_parse_internal(ap *par, ap_parser *ctx) {
  int err;
  ap_arg *next_positional = ap_find_next_positional(par->args);
  while (ctx->idx < ctx->argc) {
    if (ap_parser_cur(ctx)[0] == '-' &&
        (ap_parser_cur(ctx)[1] && ap_parser_cur(ctx)[1] != '-')) {
      /* optional "-O..." */
      int saved_idx = ctx->idx;
      ap_parser_advance(ctx, 1);
      while (ctx->idx == saved_idx && ap_parser_cur(ctx) &&
             *ap_parser_cur(ctx)) {
        /* accumulate chained short opts */
        ap_arg *search = par->args;
        char opt_short = *ap_parser_cur(ctx);
        while (search) {
          if ((search->flags & AP_ARG_FLAG_OPT) &&
              search->opt_short == opt_short) {
            /* found arg with matching short opt */
            /* step over option char */
            ap_parser_advance(ctx, 1);
            if ((err = ap_parse_internal_part(par, search, ctx)) < 0)
              return err;
            /* if this fails, your callback advanced to the next argument, but
             * did not fully consume that argument */
            assert(ctx->idx != saved_idx ? !ctx->arg_idx : 1);
            break;
          }
          search = search->next;
        }
        if (!search)
          /* arg not found */
          return AP_ERR_PARSE;
        /* arg found and parsing must continue */
        continue;
      }
    } else if (ap_parser_cur(ctx)[0] == '-' && ap_parser_cur(ctx)[1] == '-' &&
               ap_parser_cur(ctx)[2]) {
      /* long optional "--option..."*/
      ap_arg *search = par->args;
      ap_parser_advance(ctx, 2);
      while (search) {
        if ((search->flags & AP_ARG_FLAG_OPT) &&
            !strcmp(search->opt_long, ap_parser_cur(ctx))) {
          /* found arg with matching long opt */
          int prev_idx = ctx->idx;
          /* step over long opt name */
          ap_parser_advance(ctx, (int)strlen(search->opt_long));
          if ((err = ap_parse_internal_part(par, search, ctx)) < 0)
            return err;
          /* if this fails, your callback did not consume every character of the
           * argument (it returned a value less than the argument length) */
          assert(ctx->idx != prev_idx);
          break;
        }
        search = search->next;
      }
      if (!search)
        /* arg not found */
        return AP_ERR_PARSE;
      /* arg found and parsing must continue */
      continue;
    } else if (!next_positional) {
      /* no more positional args */
      return AP_ERR_PARSE;
    } else {
      /* positional, includes "-" and "--" and "" */
      int part_ret = 0, prev_idx = ctx->idx;
      if ((part_ret = ap_parse_internal_part(par, next_positional, ctx)) < 0)
        return part_ret;
      /* if this fails, your callback did not consume every character of the
       * argument (it returned a value less than the argument length) */
      assert(ctx->idx != prev_idx);
      next_positional = ap_find_next_positional(next_positional->next);
    }
  }
  if (next_positional)
    return AP_ERR_PARSE;
  return AP_ERR_NONE;
}

int ap_parse(ap *par, int argc, const char *const *argv) {
  ap_parser parser;
  ap_parser_init(&parser, argc, argv);
  return ap_parse_internal(par, &parser);
}

typedef int (*ap_print_func)(ap *par, const char *string);

int ap_usage(ap *par, ap_print_func out) {
  /* print usage without a newline */
  int err = AP_ERR_NONE;
  ap_arg *arg = par->args;
  char short_opt[2] = {0, 0};
  assert(par->progname);
  if ((err = out(par, "usage: ")))
    return err;
  if ((err = out(par, par->progname)))
    return err;
  {
    /* coalesce short args */
    int any = 0;
    for (arg = par->args; arg; arg = arg->next) {
      if (!((arg->flags & AP_ARG_FLAG_OPT) &&
            (arg->flags & AP_ARG_FLAG_COALESCE) && arg->opt_short))
        continue;
      if (!(any++) && (err = out(par, " [-")))
        return err;
      short_opt[0] = arg->opt_short;
      if ((err = out(par, short_opt)))
        return err;
    }
    if (any && (err = out(par, "]")))
      return err;
  }
  {
    /* print options and possibly metavars */
    for (arg = par->args; arg; arg = arg->next) {
      if (!((arg->flags & AP_ARG_FLAG_OPT) &&
            !((arg->flags & AP_ARG_FLAG_COALESCE) && arg->opt_short)))
        continue;
      assert(arg->opt_long || arg->opt_short);
      short_opt[0] = arg->opt_short;
      if (/* short argument */ arg->opt_short &&
          ((err = out(par, " [-")) || (err = out(par, short_opt))))

        return err;
      else if (
          /* long argument */ !arg->opt_short &&
          ((err = out(par, " [--")) || (err = out(par, arg->opt_long))))
        return err;
      if (arg->metavar &&
          ((err = out(par, " ")) || (err = out(par, arg->metavar))))
        return err;
      if ((err = out(par, "]")))
        return err;
    }
  }
  {
    /* print positionals */
    for (arg = par->args; arg; arg = arg->next) {
      if (arg->flags & AP_ARG_FLAG_OPT)
        continue;
      assert(arg->metavar);
      if ((err = out(par, " ")) || (err = out(par, arg->metavar)))
        return err;
    }
  }
  return err;
}

int ap_show_argspec(ap *par, ap_arg *arg, ap_print_func out, int with_metavar) {
  int err;
  if (arg->flags & AP_ARG_FLAG_OPT) {
    /* optionals */
    char short_opt[2] = {0, 0};
    short_opt[0] = arg->opt_short;
    if (arg->opt_short &&
        ((err = out(par, "-")) || (err = out(par, short_opt))))
      return err;
    if (with_metavar && arg->metavar &&
        ((err = out(par, " ")) || (err = out(par, arg->metavar))))
      return err;
    if (arg->opt_long && arg->opt_short && (err = out(par, ", ")))
      return err;
    if (arg->opt_long &&
        ((err = out(par, "--")) || (err = out(par, arg->opt_long))))
      return err;
    if (with_metavar && arg->metavar &&
        ((err = out(par, " ")) || (err = out(par, arg->metavar))))
      return err;
  } else if ((err = out(par, arg->metavar)))
    /* positionals */
    return err;
  return AP_ERR_NONE;
}

int ap_show_usage(ap *par) { return ap_usage(par, ap_out); }

int ap_show_help(ap *par) {
  int err = AP_ERR_NONE;
  if ((err = ap_show_usage(par)))
    return err;
  if ((err = ap_feed(par)))
    return err;
  if (par->description &&
      ((err = ap_feed(par)) || (err = ap_out(par, par->description)) ||
       (err = ap_feed(par))))
    return err;
  {
    int any = 0;
    ap_arg *arg;
    for (arg = par->args; arg; arg = arg->next) {
      /* positional arguments */
      if (arg->flags & AP_ARG_FLAG_OPT)
        continue;
      if (!(any++) && ((err = ap_feed(par)) ||
                       (err = ap_out(par, "positional arguments:")) ||
                       (err = ap_feed(par))))
        return err;
      if ((err = ap_out(par, "  ")) ||
          (err = ap_show_argspec(par, arg, ap_out, 1)) || (err = ap_feed(par)))
        return err;
      if (arg->help && ((err = ap_out(par, "    ")) ||
                        (err = ap_out(par, arg->help)) || (err = ap_feed(par))))
        return err;
    }
    any = 0;
    for (arg = par->args; arg; arg = arg->next) {
      /* optional arguments */
      if (!(arg->flags & AP_ARG_FLAG_OPT))
        continue;
      assert(arg->opt_long || arg->opt_short);
      if (!(any++) &&
          ((err = ap_feed(par)) || (err = ap_out(par, "optional arguments:")) ||
           (err = ap_feed(par))))
        return err;
      if ((err = ap_out(par, "  ")) ||
          (err = ap_show_argspec(par, arg, ap_out, 1)) || (err = ap_feed(par)))
        return err;
      if (arg->help && ((err = ap_out(par, "    ")) ||
                        (err = ap_out(par, arg->help)) || (err = ap_feed(par))))
        return err;
    }
  }
  if (par->epilog && ((err = ap_feed(par)) ||
                      (err = ap_out(par, par->epilog)) || (err = ap_feed(par))))
    return err;
  return err;
}

int ap_error_prefix(ap *par) {
  int err;
  if ((err = ap_usage(par, ap_err)))
    return err;
  if ((err = ap_err(par, "\n")) || (err = ap_err(par, par->progname)) ||
      (err = ap_err(par, ": ")) || (err = ap_err(par, "error: ")))
    return err;
  return err;
}

int ap_error(ap *par, const char *error_string) {
  int err;
  if ((err = ap_error_prefix(par)) || (err = ap_err(par, error_string)) ||
      (err = ap_err(par, "\n")))
    return err;
  return AP_ERR_NONE;
}

int ap_arg_error(ap_cb_data *cbd, const char *error_string) {
  int err;
  ap *par = cbd->parser;
  ap_arg *arg = cbd->reserved;
  if ((err = ap_error_prefix(par)) || (err = ap_err(par, "argument ")) ||
      (err = ap_show_argspec(par, arg, ap_err, 0)) ||
      (err = ap_err(par, ": ")) || (err = ap_err(par, error_string)) ||
      (err = ap_err(par, "\n")))
    return err;
  return AP_ERR_PARSE;
}
