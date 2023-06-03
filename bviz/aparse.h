#ifdef __cplusplus
extern "C" {
#endif
#if !defined(APARSE_H)
#define APARSE_H

/* Set to 1 in order to define malloc(), free(), and realloc() replacements. */
#if !defined(APARSE_USE_CUSTOM_ALLOCATOR)
#include <stdlib.h>
#define APARSE_MALLOC malloc
#define APARSE_REALLOC realloc
#define APARSE_FREE free
#endif

/* bits/types/size */
/* desc */
/* cppreference */
#if !defined(APARSE_SIZE_TYPE)
#include <stdlib.h>
#define APARSE_SIZE_TYPE size_t
#endif

/* bits/util/debug */
/* Set to 1 in order to override the setting of the NDEBUG variable. */
#if !defined(APARSE_DEBUG)
#define APARSE_DEBUG 0
#endif

/* bits/hooks/assert */
/* desc */
/* cppreference */
#if !defined(APARSE_ASSERT)
#include <assert.h>
#define APARSE_ASSERT assert
#endif

/* bits/util/exports */
/* Set to 1 in order to define all symbols with static linkage (local to the
 * including source file) as opposed to external linkage. */
#if !defined(APARSE_STATIC)
#define APARSE_STATIC 0
#endif

/* bits/types/char */
/* desc */
/* cppreference */
#if !defined(APARSE_CHAR_TYPE)
#define APARSE_CHAR_TYPE char
#endif

/* bits/types/size */
typedef APARSE_SIZE_TYPE aparse_size;

/* bits/util/debug */
#if !APARSE_DEBUG
#if defined(NDEBUG)
#if NDEBUG == 0
#define APARSE_DEBUG 1
#else
#define APARSE_DEBUG 0
#endif
#endif
#endif

/* bits/hooks/assert */
#ifndef APARSE__HOOKS_ASSERT_INTERNAL_H
#define APARSE__HOOKS_ASSERT_INTERNAL_H
#if defined(APARSE__COVERAGE) || !APARSE_DEBUG
#undef APARSE_ASSERT
#define APARSE_ASSERT(e) ((void)0)
#endif

#endif /* APARSE__HOOKS_ASSERT_INTERNAL_H */

/* bits/util/exports */
#if !defined(APARSE__SPLIT_BUILD)
#if APARSE_STATIC
#define APARSE_API static
#else
#define APARSE_API extern
#endif
#else
#define APARSE_API extern
#endif

/* bits/util/null */
#define APARSE_NULL 0

/* bits/types/char */
#if !defined(APARSE_CHAR_TYPE)
#define APARSE_CHAR_TYPE char
#endif

typedef APARSE_CHAR_TYPE aparse_char;

/* aparse */
#include <stddef.h>
#define APARSE_ERROR_NONE 0
#define APARSE_ERROR_NOMEM -1
#define APARSE_ERROR_PARSE -2
#define APARSE_ERROR_OUT -3
#define APARSE_ERROR_INVALID -4
#define APARSE_ERROR_SHOULD_EXIT -5
#define APARSE_SHOULD_EXIT APARSE_ERROR_SHOULD_EXIT

/* Syntax validity table: */
/* Where `O` is the option in question, `s` is a subargument to `O`, `P` is
 * another option (could be the same option), and `o` is the long option form
 * of `O`. */
/* |  args       | -O  | -O=s | -OP | -Os | -O s | --o | --o=s | --o s |
 * | ----------- | --- | ---- | --- | --- | ---- | --- | ----- | ----- |
 * | 2+          |     |      |     |     | *    |     |       | *     |
 * | 1           |     | *    |     | *   | *    |     | *     | *     |
 * | 0           | *   |      | *   |     |      | *   |       |       |
 * | <0_OR_1>    | *   | *    | *   | *   | *    | *   | *     | *     |
 * | <0_OR_1_EQ> | *   | *    | *   |     |      |     | *     |       |
 * | <0_OR_MORE> | *   | *    | *   | *   | *    | *   | *     | *     |
 * | <1_OR_MORE> |     | *    |     | *   | *    |     | *     | *     | */
typedef enum aparse_nargs {
    /* Parse either zero or 1 subarguments. */
    APARSE_NARGS_0_OR_1 = -1, /* Like regex '?' */
    /* Parse either zero or 1 subarguments, but only allow using '='. */
    APARSE_NARGS_0_OR_1_EQ = -2, /* Like regex '?' */
    /* Parse zero or more subarguments. */
    APARSE_NARGS_0_OR_MORE = -3, /* Like regex '*' */
    /* Parse one or more subarguments. */
    APARSE_NARGS_1_OR_MORE = -4 /* Like regex '+' */
} aparse_nargs;

typedef int aparse_error;
typedef struct aparse__state aparse__state;

typedef struct aparse_state {
    aparse__state* state;
} aparse_state;

typedef aparse_error (*aparse_out_cb)(
  void* user, const char* buf, aparse_size buf_size);
typedef aparse_error (*aparse_custom_cb)(
  void* user, aparse_state* state, int sub_arg_idx, const char* text,
  aparse_size text_size);

APARSE_API aparse_error aparse_init(aparse_state* state);
APARSE_API void
aparse_set_out_cb(aparse_state* state, aparse_out_cb out_cb, void* user);
APARSE_API void aparse_destroy(aparse_state* state);

APARSE_API aparse_error
aparse_add_opt(aparse_state* state, char short_opt, const char* long_opt);
APARSE_API aparse_error aparse_add_pos(aparse_state* state, const char* name);
APARSE_API aparse_error aparse_add_sub(aparse_state* state);

APARSE_API void aparse_arg_help(aparse_state* state, const char* help_text);
APARSE_API void aparse_arg_metavar(aparse_state* state, const char* metavar);

APARSE_API void aparse_arg_type_bool(aparse_state* state, int* out);
APARSE_API void
aparse_arg_type_store_int(aparse_state* state, int value, int* out);
APARSE_API void
aparse_arg_type_str(aparse_state* state, const char** out, aparse_size* out_size);
APARSE_API void aparse_arg_type_int(aparse_state* state, int* out);
APARSE_API void aparse_arg_type_help(aparse_state* state);
APARSE_API void aparse_arg_type_version(aparse_state* state);
APARSE_API void aparse_arg_type_custom(
  aparse_state* state, aparse_custom_cb cb, void* user, aparse_nargs nargs);

/* APARSE_API void aparse_arg_int_multi(aparse_state* state, int** out, aparse_size*
 * out_size) */
/* APARSE_API void aparse_arg_str_multi(aparse_state* state, const char** out,
 * aparse_size** size_out, aparse_size* num) */

APARSE_API int aparse_sub_add_cmd(
  aparse_state* state, const char* name, aparse_state** subcmd);

APARSE_API aparse_error
aparse_parse(aparse_state* state, int argc, const char* const* argv);

#if defined(APARSE_IMPLEMENTATION)
/* bits/math/implies */
#define APARSE__IMPLIES(a, b) (!(a) || (b))

/* bits/util/exports */
#if !defined(APARSE__SPLIT_BUILD)
#define APARSE_INTERNAL static
#else
#define APARSE_INTERNAL extern
#endif

#if !defined(APARSE__SPLIT_BUILD)
#define APARSE_INTERNAL_DATA_DECL static
#define APARSE_INTERNAL_DATA static
#else
#define APARSE_INTERNAL_DATA_DECL extern
#define APARSE_INTERNAL_DATA 
#endif

/* bits/util/preproc/token_paste */
#define APARSE__PASTE_0(a, b) a ## b
#define APARSE__PASTE(a, b) APARSE__PASTE_0(a, b)

/* bits/util/static_assert */
#define APARSE__STATIC_ASSERT(name, expr) char APARSE__PASTE(aparse__, name)[(expr)==1]

/* bits/util/ntstr/len */
APARSE_INTERNAL aparse_size aparse__slen(const aparse_char* s);

/* bits/util/unused */
#define APARSE__UNUSED(x) ((void)(x))

/* aparse */
#include <stdio.h>
typedef struct aparse__arg aparse__arg;

typedef struct aparse__arg_opt {
    char short_opt;
    const char* long_opt;
    aparse_size long_opt_size;
} aparse__arg_opt;

typedef struct aparse__sub aparse__sub;

typedef struct aparse__arg_sub {
    aparse__sub* head;
    aparse__sub* tail;
} aparse__arg_sub;

typedef struct aparse__arg_pos {
    const char* name;
    aparse_size name_size;
} aparse__arg_pos;

typedef union aparse__arg_contents {
    aparse__arg_opt opt;
    aparse__arg_sub sub;
    aparse__arg_pos pos;
} aparse__arg_contents;

enum aparse__arg_type {
    /* Optional argument (-o, --o) */
    APARSE__ARG_TYPE_OPTIONAL,
    /* Positional argument */
    APARSE__ARG_TYPE_POSITIONAL,
    /* Subcommand argument */
    APARSE__ARG_TYPE_SUBCOMMAND
};

typedef aparse_error (*aparse__arg_parse_cb)(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size);
typedef void (*aparse__arg_destroy_cb)(aparse__arg* arg);

typedef union aparse__arg_callback_data_2 {
    void* plain;
    aparse_custom_cb custom_cb;
    int int_val;
} aparse__arg_callback_data_2;

struct aparse__arg {
    enum aparse__arg_type type;
    aparse__arg_contents contents;
    const char* help;
    aparse_size help_size;
    const char* metavar;
    aparse_size metavar_size;
    aparse_nargs nargs;
    int required;
    int was_specified;
    aparse__arg* next;
    aparse__arg_parse_cb callback;
    aparse__arg_destroy_cb destroy;
    void* callback_data;
    aparse__arg_callback_data_2 callback_data_2;
};

#define APARSE__STATE_OUT_BUF_SIZE 128

typedef struct aparse__state_root {
    char out_buf[APARSE__STATE_OUT_BUF_SIZE];
    aparse_size out_buf_ptr;
    const char* prog_name;
    aparse_size prog_name_size;
} aparse__state_root;

struct aparse__state {
    aparse__arg* head;
    aparse__arg* tail;
    const char* help;
    aparse_size help_size;
    aparse_out_cb out_cb;
    void* user;
    aparse__state_root* root;
    int is_root;
};

struct aparse__sub {
    const char* name;
    aparse_size name_size;
    aparse__state subparser;
    aparse__sub* next;
};

APARSE_INTERNAL void aparse__arg_init(aparse__arg* arg);
APARSE_INTERNAL void aparse__arg_destroy(aparse__arg* arg);
#if 0
APARSE_INTERNAL void aparse__state_init_from(aparse__state* state, aparse__state* other);
#endif
APARSE_INTERNAL void aparse__state_init(aparse__state* state);
APARSE_INTERNAL void aparse__state_destroy(aparse__state* state);
APARSE_INTERNAL void aparse__state_set_out_cb(
  aparse__state* state, aparse_out_cb out_cb, void* user);
APARSE_INTERNAL void aparse__state_reset(aparse__state* state);
APARSE_INTERNAL aparse_error aparse__state_add_opt(
  aparse__state* state, char short_opt, const char* long_opt);
APARSE_INTERNAL aparse_error
aparse__state_add_pos(aparse__state* state, const char* name);
APARSE_INTERNAL aparse_error aparse__state_add_sub(aparse__state* state);

APARSE_INTERNAL void aparse__state_check_before_add(aparse__state* state);
APARSE_INTERNAL void aparse__state_check_before_modify(aparse__state* state);
APARSE_INTERNAL void aparse__state_check_before_set_type(aparse__state* state);
APARSE_INTERNAL aparse_error aparse__state_flush(aparse__state* state);
APARSE_INTERNAL aparse_error aparse__state_out(aparse__state* state, char out);
APARSE_INTERNAL aparse_error
aparse__state_out_s(aparse__state* state, const char* s);
APARSE_INTERNAL aparse_error
aparse__state_out_n(aparse__state* state, const char* s, aparse_size n);

APARSE_INTERNAL void aparse__arg_bool_init(aparse__arg* arg, int* out);
APARSE_INTERNAL void
aparse__arg_store_int_init(aparse__arg* arg, int val, int* out);
APARSE_INTERNAL void aparse__arg_int_init(aparse__arg* arg, int* out);
APARSE_INTERNAL void
aparse__arg_str_init(aparse__arg* arg, const char** out, aparse_size* out_size);
APARSE_INTERNAL void aparse__arg_help_init(aparse__arg* arg);
APARSE_INTERNAL void aparse__arg_version_init(aparse__arg* arg);
APARSE_INTERNAL void aparse__arg_custom_init(
  aparse__arg* arg, aparse_custom_cb cb, void* user, aparse_nargs nargs);
APARSE_INTERNAL void aparse__arg_sub_init(aparse__arg* arg);

APARSE_API aparse_error
aparse__parse_argv(aparse__state* state, int argc, const char* const* argv);

APARSE_INTERNAL aparse_error aparse__error_begin(aparse__state* state);
APARSE_INTERNAL aparse_error
aparse__error_begin_arg(aparse__state* state, const aparse__arg* arg);
APARSE_INTERNAL aparse_error
aparse__error_unrecognized_arg(aparse__state* state, const char* arg);
APARSE_INTERNAL aparse_error
aparse__error_unrecognized_short_arg(aparse__state* state, char short_opt);
APARSE_INTERNAL aparse_error
aparse__error_quote(aparse__state* state, const char* text, aparse_size text_size);
APARSE_INTERNAL aparse_error aparse__error_usage(aparse__state* state);
APARSE_INTERNAL aparse_error
aparse__error_print_short_opt(aparse__state* state, const aparse__arg* arg);
APARSE_INTERNAL aparse_error
aparse__error_print_long_opt(aparse__state* state, const aparse__arg* arg);
APARSE_INTERNAL aparse_error
aparse__error_print_sub_args(aparse__state* state, const aparse__arg* arg);

#endif /* APARSE_IMPLEMENTATION */
#if defined(APARSE_IMPLEMENTATION)
/* bits/types/char */
APARSE__STATIC_ASSERT(aparse__char_is_one_byte, sizeof(aparse_char) == 1);

/* bits/util/ntstr/len */
APARSE_INTERNAL aparse_size aparse__slen(const aparse_char* s) {
    aparse_size sz = 0;
    while (*s) {
        sz++;
        s++;
    }
    return sz;
}

/* aparse */
APARSE_API aparse_error aparse_init(aparse_state* state) {
    state->state = (aparse__state*)APARSE_MALLOC(sizeof(aparse__state));
    if (state->state == APARSE_NULL) {
        return APARSE_ERROR_NOMEM;
    }
    aparse__state_init(state->state);
    state->state->root =
      (aparse__state_root*)APARSE_MALLOC(sizeof(aparse__state_root));
    if (state->state->root == APARSE_NULL) {
        return APARSE_ERROR_NOMEM;
    }
    state->state->root->out_buf_ptr = 0;
    state->state->root->prog_name = APARSE_NULL;
    state->state->root->prog_name_size = APARSE_NULL;
    state->state->is_root = 1;
    return APARSE_ERROR_NONE;
}

APARSE_API void aparse_destroy(aparse_state* state) {
    aparse__state_destroy(state->state);
    if (state->state != APARSE_NULL) {
        APARSE_FREE(state->state);
    }
}

APARSE_API void
aparse_set_out_cb(aparse_state* state, aparse_out_cb out_cb, void* user) {
    aparse__state_set_out_cb(state->state, out_cb, user);
}

APARSE_API aparse_error
aparse_add_opt(aparse_state* state, char short_opt, const char* long_opt) {
    aparse__state_check_before_add(state->state);
    return aparse__state_add_opt(state->state, short_opt, long_opt);
}

APARSE_API aparse_error aparse_add_pos(aparse_state* state, const char* name) {
    aparse__state_check_before_add(state->state);
    return aparse__state_add_pos(state->state, name);
}

APARSE_API aparse_error aparse_add_sub(aparse_state* state) {
    aparse__state_check_before_add(state->state);
    return aparse__state_add_sub(state->state);
}

APARSE_API void aparse_arg_help(aparse_state* state, const char* help_text) {
    aparse__state_check_before_modify(state->state);
    state->state->tail->help = help_text;
    if (help_text != APARSE_NULL) {
        state->state->tail->help_size = aparse__slen(help_text);
    }
}

APARSE_API void aparse_arg_metavar(aparse_state* state, const char* metavar) {
    aparse__state_check_before_modify(state->state);
    state->state->tail->metavar = metavar;
    if (metavar != APARSE_NULL) {
        state->state->tail->metavar_size = aparse__slen(metavar);
    }
}

APARSE_API void aparse_arg_type_bool(aparse_state* state, int* out) {
    aparse__state_check_before_set_type(state->state);
    aparse__arg_bool_init(state->state->tail, out);
}

APARSE_API void aparse_arg_type_store_int(aparse_state* state, int val, int* out) {
    aparse__state_check_before_set_type(state->state);
    aparse__arg_store_int_init(state->state->tail, val, out);
}

APARSE_API void aparse_arg_type_int(aparse_state* state, int* out) {
    aparse__state_check_before_set_type(state->state);
    aparse__arg_int_init(state->state->tail, out);
}

APARSE_API void
aparse_arg_type_str(aparse_state* state, const char** out, aparse_size* out_size) {
    aparse__state_check_before_set_type(state->state);
    aparse__arg_str_init(state->state->tail, out, out_size);
}

APARSE_API void aparse_arg_type_help(aparse_state* state) {
    aparse__state_check_before_set_type(state->state);
    aparse__arg_help_init(state->state->tail);
}

APARSE_API void aparse_arg_type_version(aparse_state* state) {
    aparse__state_check_before_set_type(state->state);
    aparse__arg_version_init(state->state->tail);
}

APARSE_API void aparse_arg_type_custom(
  aparse_state* state, aparse_custom_cb cb, void* user, aparse_nargs nargs) {
    aparse__state_check_before_set_type(state->state);
    aparse__arg_custom_init(state->state->tail, cb, user, nargs);
}

APARSE_API aparse_error
aparse_parse(aparse_state* state, int argc, const char* const* argv) {
    aparse_error err = APARSE_ERROR_NONE;
    if (argc == 0) {
        return APARSE_ERROR_INVALID;
    } else {
        state->state->root->prog_name = argv[0];
        state->state->root->prog_name_size =
          aparse__slen(state->state->root->prog_name);
        err = aparse__parse_argv(state->state, argc - 1, argv + 1);
        if (err == APARSE_ERROR_PARSE) {
            if ((err = aparse__state_flush(state->state))) {
                return err;
            }
            return APARSE_ERROR_PARSE;
        } else if (err == APARSE_ERROR_SHOULD_EXIT) {
            if ((err = aparse__state_flush(state->state))) {
                return err;
            }
            return APARSE_ERROR_SHOULD_EXIT;
        } else {
            return err;
        }
    }
}

/* aparse */
APARSE_INTERNAL void aparse__arg_init(aparse__arg* arg) {
    arg->type = (enum aparse__arg_type)0;
    arg->help = APARSE_NULL;
    arg->help_size = 0;
    arg->metavar = APARSE_NULL;
    arg->metavar_size = 0;
    arg->nargs = (aparse_nargs)0;
    arg->required = 0;
    arg->was_specified = 0;
    arg->next = APARSE_NULL;
    arg->callback = APARSE_NULL;
    arg->destroy = APARSE_NULL;
    arg->callback_data = APARSE_NULL;
    arg->callback_data_2.plain = APARSE_NULL;
}

APARSE_INTERNAL void aparse__arg_destroy(aparse__arg* arg) {
    if (arg->destroy != APARSE_NULL) {
        arg->destroy(arg);
    }
}

APARSE_INTERNAL void aparse__arg_bool_destroy(aparse__arg* arg);
APARSE_INTERNAL aparse_error aparse__arg_bool_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size);

APARSE_INTERNAL void aparse__arg_bool_init(aparse__arg* arg, int* out) {
    arg->nargs = APARSE_NARGS_0_OR_1_EQ;
    arg->callback = aparse__arg_bool_cb;
    arg->callback_data = (void*)out;
    arg->destroy = aparse__arg_bool_destroy;
}

APARSE_INTERNAL void aparse__arg_bool_destroy(aparse__arg* arg) {
    APARSE__UNUSED(arg);
}

APARSE_INTERNAL aparse_error aparse__arg_bool_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size) {
    aparse_error err = APARSE_ERROR_NONE;
    int* out = (int*)arg->callback_data;
    APARSE__UNUSED(state);
    APARSE__UNUSED(sub_arg_idx);
    if (text == APARSE_NULL) {
        *out = 1;
        return APARSE_ERROR_NONE;
    } else if (text_size == 1 && *text == '0') {
        *out = 0;
        return APARSE_ERROR_NONE;
    } else if (text_size == 1 && *text == '1') {
        *out = 1;
        return APARSE_ERROR_NONE;
    } else {
        if ((err = aparse__error_begin_arg(state, arg))) {
            return err;
        }
        if ((err = aparse__state_out_s(
               state, "invalid value for boolean flag: "))) {
            return err;
        }
        if ((err = aparse__error_quote(state, text, text_size))) {
            return err;
        }
        if ((err = aparse__state_out(state, '\n'))) {
            return err;
        }
        return APARSE_ERROR_PARSE;
    }
}

APARSE_INTERNAL void aparse__arg_store_int_destroy(aparse__arg* arg);
APARSE_INTERNAL aparse_error aparse__arg_store_int_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size);

APARSE_INTERNAL void
aparse__arg_store_int_init(aparse__arg* arg, int val, int* out) {
    APARSE_ASSERT(out != APARSE_NULL);
    arg->nargs = APARSE_NARGS_0_OR_1_EQ;
    arg->callback = aparse__arg_store_int_cb;
    arg->callback_data = (void*)out;
    arg->callback_data_2.int_val = val;
    arg->destroy = aparse__arg_store_int_destroy;
}

APARSE_INTERNAL void aparse__arg_store_int_destroy(aparse__arg* arg) {
    APARSE__UNUSED(arg);
}

APARSE_INTERNAL aparse_error aparse__arg_store_int_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size) {
    aparse_error err = APARSE_ERROR_NONE;
    int* out = (int*)arg->callback_data;
    APARSE__UNUSED(state);
    APARSE__UNUSED(sub_arg_idx);
    if (text == APARSE_NULL) {
        *out = arg->callback_data_2.int_val;
        return APARSE_ERROR_NONE;
    } else if (text_size == 1 && *text == '0') {
        /* don't store anything */
        return APARSE_ERROR_NONE;
    } else if (text_size == 1 && *text == '1') {
        *out = arg->callback_data_2.int_val;
        return APARSE_ERROR_NONE;
    } else {
        if ((err = aparse__error_begin_arg(state, arg))) {
            return err;
        }
        if ((err = aparse__state_out_s(
               state, "invalid value for boolean flag: "))) {
            return err;
        }
        if ((err = aparse__error_quote(state, text, text_size))) {
            return err;
        }
        if ((err = aparse__state_out(state, '\n'))) {
            return err;
        }
        return APARSE_ERROR_PARSE;
    }
}

APARSE_INTERNAL void aparse__arg_str_destroy(aparse__arg* arg);
APARSE_INTERNAL aparse_error aparse__arg_str_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size);

APARSE_INTERNAL void
aparse__arg_str_init(aparse__arg* arg, const char** out, aparse_size* out_size) {
    APARSE_ASSERT(out != APARSE_NULL);
    arg->nargs = (aparse_nargs)1;
    arg->callback = aparse__arg_str_cb;
    arg->callback_data = (void*)out;
    arg->callback_data_2.plain = (void*)out_size;
    arg->destroy = aparse__arg_str_destroy;
}

APARSE_INTERNAL void aparse__arg_str_destroy(aparse__arg* arg) { APARSE__UNUSED(arg); }

APARSE_INTERNAL aparse_error aparse__arg_str_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size) {
    const char** out = (const char**)arg->callback_data;
    aparse_size* out_size = (aparse_size*)arg->callback_data_2.plain;
    APARSE_ASSERT(text != APARSE_NULL);
    APARSE__UNUSED(state);
    APARSE__UNUSED(sub_arg_idx);
    *out = text;
    if (out_size) {
        *out_size = text_size;
    }
    return APARSE_ERROR_NONE;
}

APARSE_INTERNAL void aparse__arg_int_destroy(aparse__arg* arg);
APARSE_INTERNAL aparse_error aparse__arg_int_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size);

APARSE_INTERNAL void aparse__arg_int_init(aparse__arg* arg, int* out) {
    arg->nargs = (aparse_nargs)1;
    arg->callback = aparse__arg_int_cb;
    arg->callback_data = (void*)out;
    arg->destroy = aparse__arg_int_destroy;
}

#define APARSE__ARG_INT_MAX 9999999
#define APARSE__ARG_INT_SIZE_MAX 7

APARSE_INTERNAL aparse_error aparse__arg_int_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size) {
    aparse_error err = APARSE_ERROR_NONE;
    const char* end = text + text_size;
    int acc = 0;
    int n = 0;
    APARSE__UNUSED(sub_arg_idx);
    while (text < end) {
        if (n == APARSE__ARG_INT_SIZE_MAX) {
            goto error;
        }
        if (acc > APARSE__ARG_INT_MAX) {
            goto error;
        }
        if (*text >= '0' && *text <= '9') {
            acc *= 10;
            acc += *text - '0';
        } else {
            goto error;
        }
        text++;
        n++;
    }
    *((int*)arg->callback_data) = acc;
    return APARSE_ERROR_NONE;
error:
    if ((err = aparse__error_begin_arg(state, arg))) {
        return err;
    }
    if ((err =
           aparse__state_out_s(state, "invalid value for integer option: "))) {
        return err;
    }
    if ((err = aparse__error_quote(state, text, text_size))) {
        return err;
    }
    if ((err = aparse__state_out(state, '\n'))) {
        return err;
    }
    return APARSE_ERROR_PARSE;
}

APARSE_INTERNAL void aparse__arg_int_destroy(aparse__arg* arg) { APARSE__UNUSED(arg); }

APARSE_INTERNAL void aparse__arg_help_destroy(aparse__arg* arg);
APARSE_INTERNAL aparse_error aparse__arg_help_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size);

APARSE_INTERNAL void aparse__arg_help_init(aparse__arg* arg) {
    arg->nargs = (aparse_nargs)0;
    arg->callback = aparse__arg_help_cb;
    arg->destroy = aparse__arg_help_destroy;
}

APARSE_INTERNAL void aparse__arg_help_destroy(aparse__arg* arg) {
    APARSE__UNUSED(arg);
}

APARSE_INTERNAL aparse_error aparse__arg_help_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size) {
    aparse_error err = APARSE_ERROR_NONE;
    APARSE__UNUSED(arg);
    APARSE__UNUSED(sub_arg_idx);
    APARSE__UNUSED(text);
    APARSE__UNUSED(text_size);
    if ((err = aparse__error_usage(state))) {
        return err;
    }
    {
        int has_printed_header = 0;
        aparse__arg* cur = state->head;
        while (cur) {
            if (cur->type != APARSE__ARG_TYPE_POSITIONAL) {
                cur = cur->next;
                continue;
            }
            if (!has_printed_header) {
                if ((err = aparse__state_out_s(
                       state, "\npositional arguments:\n"))) {
                    return err;
                }
                has_printed_header = 1;
            }
            if ((err = aparse__state_out_s(state, "  "))) {
                return err;
            }
            if (cur->metavar == APARSE_NULL) {
                if ((err = aparse__state_out_n(
                       state, cur->contents.pos.name,
                       cur->contents.pos.name_size))) {
                    return err;
                }
            } else {
                if ((err = aparse__state_out_n(
                       state, cur->metavar, cur->metavar_size))) {
                    return err;
                }
            }
            if ((err = aparse__state_out(state, '\n'))) {
                return err;
            }
            if (cur->help != APARSE_NULL) {
                if ((err = aparse__state_out_s(state, "    "))) {
                    return err;
                }
                if ((err = aparse__state_out_n(
                       state, cur->help, cur->help_size))) {
                    return err;
                }
                if ((err = aparse__state_out(state, '\n'))) {
                    return err;
                }
            }
            cur = cur->next;
        }
    }
    {
        int has_printed_header = 0;
        aparse__arg* cur = state->head;
        while (cur) {
            if (cur->type != APARSE__ARG_TYPE_OPTIONAL) {
                cur = cur->next;
                continue;
            }
            if (!has_printed_header) {
                if ((err = aparse__state_out_s(
                       state, "\noptional arguments:\n"))) {
                    return err;
                }
                has_printed_header = 1;
            }
            if ((err = aparse__state_out_s(state, "  "))) {
                return err;
            }
            if (cur->contents.opt.short_opt != '\0') {
                if ((err = aparse__error_print_short_opt(state, cur))) {
                    return err;
                }
                if (cur->nargs != APARSE_NARGS_0_OR_1_EQ && cur->nargs != 0) {
                    if ((err = aparse__state_out(state, ' '))) {
                        return err;
                    }
                }
                if ((err = aparse__error_print_sub_args(state, cur))) {
                    return err;
                }
            }
            if (cur->contents.opt.long_opt != APARSE_NULL) {
                if (cur->contents.opt.short_opt != '\0') {
                    if ((err = aparse__state_out_s(state, ", "))) {
                        return err;
                    }
                }
                if ((err = aparse__error_print_long_opt(state, cur))) {
                    return err;
                }
                if (cur->nargs != APARSE_NARGS_0_OR_1_EQ && cur->nargs != 0) {
                    if ((err = aparse__state_out(state, ' '))) {
                        return err;
                    }
                }
                if ((err = aparse__error_print_sub_args(state, cur))) {
                    return err;
                }
            }
            if ((err = aparse__state_out(state, '\n'))) {
                return err;
            }
            if (cur->help != APARSE_NULL) {
                if ((err = aparse__state_out_s(state, "    "))) {
                    return err;
                }
                if ((err = aparse__state_out_n(
                       state, cur->help, cur->help_size))) {
                    return err;
                }
                if ((err = aparse__state_out(state, '\n'))) {
                    return err;
                }
            }
            cur = cur->next;
        }
    }
    return APARSE_SHOULD_EXIT;
}

APARSE_INTERNAL void aparse__arg_version_destroy(aparse__arg* arg);
APARSE_INTERNAL aparse_error aparse__arg_version_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size);

APARSE_INTERNAL void aparse__arg_version_init(aparse__arg* arg) {
    arg->nargs = (aparse_nargs)0;
    arg->callback = aparse__arg_version_cb;
    arg->destroy = aparse__arg_version_destroy;
}

APARSE_INTERNAL void aparse__arg_version_destroy(aparse__arg* arg) {
    APARSE__UNUSED(arg);
}

APARSE_INTERNAL aparse_error aparse__arg_version_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size) {
    aparse_error err = APARSE_ERROR_NONE;
    APARSE__UNUSED(arg);
    APARSE__UNUSED(sub_arg_idx);
    APARSE__UNUSED(text);
    APARSE__UNUSED(text_size);
    /* TODO: print version */
    if ((err = aparse__state_out_s(state, "version\n"))) {
        return err;
    }
    return APARSE_SHOULD_EXIT;
}

APARSE_INTERNAL void aparse__arg_custom_destroy(aparse__arg* arg);
APARSE_INTERNAL aparse_error aparse__arg_custom_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size);

APARSE_INTERNAL void aparse__arg_custom_init(
  aparse__arg* arg, aparse_custom_cb cb, void* user, aparse_nargs nargs) {
    arg->nargs = nargs;
    arg->callback = aparse__arg_custom_cb;
    arg->callback_data = (void*)user;
    arg->callback_data_2.custom_cb = cb;
    arg->destroy = aparse__arg_custom_destroy;
}

APARSE_INTERNAL void aparse__arg_custom_destroy(aparse__arg* arg) {
    APARSE__UNUSED(arg);
}

APARSE_INTERNAL aparse_error aparse__arg_custom_cb(
  aparse__arg* arg, aparse__state* state, aparse_size sub_arg_idx,
  const char* text, aparse_size text_size) {
    aparse_custom_cb cb = (aparse_custom_cb)arg->callback_data_2.custom_cb;
    aparse_state state_;
    state_.state = state;
    return cb(arg->callback_data, &state_, (int)sub_arg_idx, text, text_size);
}

APARSE_INTERNAL void aparse__arg_sub_destroy(aparse__arg* arg);

APARSE_INTERNAL void aparse__arg_sub_init(aparse__arg* arg) {
    arg->type = APARSE__ARG_TYPE_SUBCOMMAND;
    arg->contents.sub.head = APARSE_NULL;
    arg->contents.sub.tail = APARSE_NULL;
    arg->destroy = aparse__arg_sub_destroy;
}

APARSE_INTERNAL void aparse__arg_sub_destroy(aparse__arg* arg) {
    aparse__sub* sub = arg->contents.sub.head;
    APARSE_ASSERT(arg->type == APARSE__ARG_TYPE_SUBCOMMAND);
    while (sub) {
        aparse__sub* prev = sub;
        aparse__state_destroy(&prev->subparser);
        sub = prev->next;
        APARSE_FREE(prev);
    }
}

/* aparse */
APARSE_INTERNAL aparse_error aparse__error_begin_progname(aparse__state* state) {
    aparse_error err = APARSE_ERROR_NONE;
    if ((err = aparse__state_out_n(
           state, state->root->prog_name, state->root->prog_name_size))) {
        return err;
    }
    if ((err = aparse__state_out_s(state, ": "))) {
        return err;
    }
    return err;
}

APARSE_INTERNAL aparse_error aparse__error_begin(aparse__state* state) {
    aparse_error err = APARSE_ERROR_NONE;
    if ((err = aparse__error_begin_progname(state))) {
        return err;
    }
    if ((err = aparse__state_out_s(state, "error: "))) {
        return err;
    }
    return err;
}

APARSE_INTERNAL aparse_error
aparse__error_print_short_opt(aparse__state* state, const aparse__arg* arg) {
    aparse_error err = APARSE_ERROR_NONE;
    APARSE_ASSERT(arg->contents.opt.short_opt);
    if ((err = aparse__state_out(state, '-'))) {
        return err;
    }
    if ((err = aparse__state_out(state, arg->contents.opt.short_opt))) {
        return err;
    }
    return err;
}

APARSE_INTERNAL aparse_error
aparse__error_print_long_opt(aparse__state* state, const aparse__arg* arg) {
    aparse_error err = APARSE_ERROR_NONE;
    APARSE_ASSERT(arg->contents.opt.long_opt);
    if ((err = aparse__state_out_s(state, "--"))) {
        return err;
    }
    if ((err = aparse__state_out_n(
           state, arg->contents.opt.long_opt,
           arg->contents.opt.long_opt_size))) {
        return err;
    }
    return err;
}

APARSE_INTERNAL aparse_error
aparse__error_begin_opt(aparse__state* state, const aparse__arg* arg) {
    aparse_error err = APARSE_ERROR_NONE;
    APARSE_ASSERT(arg->type == APARSE__ARG_TYPE_OPTIONAL);
    if ((err = aparse__error_begin(state))) {
        return err;
    }
    if ((err = aparse__state_out_s(state, "option "))) {
        return err;
    }
    if (arg->contents.opt.short_opt != '\0') {
        if ((err = aparse__error_print_short_opt(state, arg))) {
            return err;
        }
    }
    if (arg->contents.opt.long_opt != APARSE_NULL) {
        if (arg->contents.opt.short_opt != '\0') {
            if ((err = aparse__state_out_s(state, ", "))) {
                return err;
            }
        }
        if ((err = aparse__error_print_long_opt(state, arg))) {
            return err;
        }
    }
    if ((err = aparse__state_out_s(state, ": "))) {
        return err;
    }
    return err;
}

APARSE_INTERNAL aparse_error
aparse__error_begin_pos(aparse__state* state, const aparse__arg* arg) {
    aparse_error err = APARSE_ERROR_NONE;
    APARSE_ASSERT(arg->type == APARSE__ARG_TYPE_POSITIONAL);
    if ((err = aparse__error_begin(state))) {
        return err;
    }
    if ((err = aparse__state_out_s(state, "argument "))) {
        return err;
    }
    if ((err = aparse__state_out_n(
           state, arg->contents.pos.name, arg->contents.pos.name_size))) {
        return err;
    }
    if ((err = aparse__state_out_s(state, ": "))) {
        return err;
    }
    return err;
}

APARSE_INTERNAL aparse_error
aparse__error_begin_arg(aparse__state* state, const aparse__arg* arg) {
    if (arg->type == APARSE__ARG_TYPE_OPTIONAL) {
        return aparse__error_begin_opt(state, arg);
    } else {
        return aparse__error_begin_pos(state, arg);
    }
}

APARSE_INTERNAL int
aparse__error_should_quote(const char* text, aparse_size text_size) {
    aparse_size i;
    for (i = 0; i < text_size; i++) {
        if (text[i] < ' ') {
            return 1;
        }
    }
    return 0;
}

APARSE_INTERNAL char aparse__hexdig(unsigned char c) {
    if (c < 10) {
        return '0' + (char)c;
    } else {
        return 'a' + ((char)c - 10);
    }
}

APARSE_INTERNAL aparse_error
aparse__error_quote_one(aparse__state* state, char c) {
    aparse_error err = APARSE_ERROR_NONE;
    if (c < ' ') {
        if ((err = aparse__state_out(state, '\\'))) {
            return err;
        }
        if ((err = aparse__state_out(state, 'x'))) {
            return err;
        }
        if ((err = aparse__state_out(state, aparse__hexdig((c >> 4) & 0xF)))) {
            return err;
        }
        if ((err = aparse__state_out(state, aparse__hexdig(c & 0xF)))) {
            return err;
        }
    } else {
        if ((err = aparse__state_out(state, c))) {
            return err;
        }
    }
    return err;
}

APARSE_INTERNAL aparse_error aparse__error_quote(
  aparse__state* state, const char* text, aparse_size text_size) {
    aparse_error err = APARSE_ERROR_NONE;
    aparse_size i;
    if ((err = aparse__state_out(state, '"'))) {
        return err;
    }
    for (i = 0; i < text_size; i++) {
        char c = text[i];
        if ((err = aparse__error_quote_one(state, c))) {
            return err;
        }
    }
    if ((err = aparse__state_out(state, '"'))) {
        return err;
    }
    return err;
}

APARSE_INTERNAL aparse_error
aparse__error_unrecognized_arg(aparse__state* state, const char* arg) {
    aparse_error err = APARSE_ERROR_NONE;
    if ((err = aparse__error_begin(state))) {
        return err;
    }
    if ((err = aparse__state_out_s(state, "unrecognized argument: "))) {
        return err;
    }
    if (aparse__error_should_quote(arg, aparse__slen(arg))) {
        if ((err = aparse__error_quote(state, arg, aparse__slen(arg)))) {
            return err;
        }
    } else {
        if ((err = aparse__state_out_s(state, arg))) {
            return err;
        }
    }
    if ((err = aparse__state_out(state, '\n'))) {
        return err;
    }
    return err;
}

APARSE_INTERNAL aparse_error
aparse__error_unrecognized_short_arg(aparse__state* state, char short_opt) {
    aparse_error err = APARSE_ERROR_NONE;
    if ((err = aparse__error_begin(state))) {
        return err;
    }
    if ((err = aparse__state_out_s(state, "unrecognized argument: "))) {
        return err;
    }
    if (aparse__error_should_quote(&short_opt, 1) || short_opt == ' ') {
        if ((err = aparse__state_out_n(state, "\"-", 2))) {
            return err;
        }
        if ((err = aparse__error_quote_one(state, short_opt))) {
            return err;
        }
        if ((err = aparse__state_out_n(state, "\"", 1))) {
            return err;
        }
    } else {
        if ((err = aparse__state_out_n(state, "-", 1))) {
            return err;
        }
        if ((err = aparse__state_out_n(state, &short_opt, 1))) {
            return err;
        }
    }
    if ((err = aparse__state_out(state, '\n'))) {
        return err;
    }
    return err;
}

int aparse__error_can_coalesce_in_usage(const aparse__arg* arg) {
    if (arg->type != APARSE__ARG_TYPE_OPTIONAL) {
        return 0;
    }
    if (arg->required) {
        return 0;
    }
    if (arg->contents.opt.short_opt == '\0') {
        return 0;
    }
    if ((arg->nargs != APARSE_NARGS_0_OR_1_EQ) && (arg->nargs != 0)) {
        return 0;
    }
    return 1;
}

APARSE_INTERNAL aparse_error
aparse__error_print_sub_args(aparse__state* state, const aparse__arg* arg) {
    aparse_error err = APARSE_ERROR_NONE;
    const char* var;
    aparse_size var_size;
    if (arg->metavar != APARSE_NULL) {
        var = arg->metavar;
        var_size = arg->metavar_size;
    } else if (arg->type == APARSE__ARG_TYPE_POSITIONAL) {
        var = arg->contents.pos.name;
        var_size = arg->contents.pos.name_size;
    } else {
        var = "ARG";
        var_size = 3;
    }
    if (arg->nargs == APARSE_NARGS_1_OR_MORE) {
        if ((err = aparse__state_out_n(state, var, var_size))) {
            return err;
        }
        if ((err = aparse__state_out_s(state, " ["))) {
            return err;
        }
        if ((err = aparse__state_out_n(state, var, var_size))) {
            return err;
        }
        if ((err = aparse__state_out_s(state, " ...]]"))) {
            return err;
        }
    } else if (arg->nargs == APARSE_NARGS_0_OR_MORE) {
        if ((err = aparse__state_out(state, '['))) {
            return err;
        }
        if ((err = aparse__state_out_n(state, var, var_size))) {
            return err;
        }
        if ((err = aparse__state_out_s(state, " ["))) {
            return err;
        }
        if ((err = aparse__state_out_n(state, var, var_size))) {
            return err;
        }
        if ((err = aparse__state_out_s(state, " ...]]"))) {
            return err;
        }
    } else if (arg->nargs == APARSE_NARGS_0_OR_1_EQ) {
        /* pass */
    } else if (arg->nargs == APARSE_NARGS_0_OR_1) {
        if ((err = aparse__state_out(state, '['))) {
            return err;
        }
        if ((err = aparse__state_out_n(state, var, var_size))) {
            return err;
        }
        if ((err = aparse__state_out(state, ']'))) {
            return err;
        }
    } else if (arg->nargs > 0) {
        int i;
        for (i = 0; i < arg->nargs; i++) {
            if (i) {
                if ((err = aparse__state_out(state, ' '))) {
                    return err;
                }
            }
            if ((err = aparse__state_out_n(state, var, var_size))) {
                return err;
            }
        }
    }
    return err;
}

APARSE_INTERNAL aparse_error aparse__error_usage(aparse__state* state) {
    aparse_error err = APARSE_ERROR_NONE;
    const aparse__arg* cur = state->head;
    int has_printed = 0;
    if ((err = aparse__state_out_s(state, "usage: "))) {
        return err;
    }
    if ((err = aparse__state_out_n(
           state, state->root->prog_name, state->root->prog_name_size))) {
        return err;
    }
    /* print coalesced args */
    while (cur) {
        if (aparse__error_can_coalesce_in_usage(cur)) {
            if (!has_printed) {
                if ((err = aparse__state_out_s(state, " [-"))) {
                    return err;
                }
                has_printed = 1;
            }
            if ((err =
                   aparse__state_out(state, cur->contents.opt.short_opt))) {
                return err;
            }
        }
        cur = cur->next;
    }
    if (has_printed) {
        if ((err = aparse__state_out(state, ']'))) {
            return err;
        }
    }
    /* print other args */
    cur = state->head;
    while (cur) {
        if (!aparse__error_can_coalesce_in_usage(cur)) {
            if ((err = aparse__state_out(state, ' '))) {
                return err;
            }
            if (!cur->required) {
                if ((err = aparse__state_out(state, '['))) {
                    return err;
                }
            }
            if (cur->type == APARSE__ARG_TYPE_OPTIONAL) {
                if (cur->contents.opt.short_opt) {
                    if ((err = aparse__error_print_short_opt(state, cur))) {
                        return err;
                    }
                } else if (cur->contents.opt.long_opt) {
                    if ((err = aparse__error_print_long_opt(state, cur))) {
                        return err;
                    }
                }
                if (cur->nargs != APARSE_NARGS_0_OR_1_EQ && cur->nargs != 0) {
                    if ((err = aparse__state_out(state, ' '))) {
                        return err;
                    }
                }
            }
            if ((err = aparse__error_print_sub_args(state, cur))) {
                return err;
            }
            if (!cur->required) {
                if ((err = aparse__state_out(state, ']'))) {
                    return err;
                }
            }
        }
        cur = cur->next;
    }
    if ((err = aparse__state_out(state, '\n'))) {
        return err;
    }
    return err;
}

#if 0
APARSE_INTERNAL void aparse__error_print_opt_name(aparse_state* state,
    const struct aparse__arg*                   opt)
{
    (void)(state);
    if (opt->short_opt && !opt->long_opt) {
        /* Only short option was specified */
        fprintf(stderr, "-%c", opt->short_opt);
    } else if (opt->short_opt && opt->long_opt) {
        /* Both short option and long option were specified */
        fprintf(stderr, "-%c/--%s", opt->short_opt, opt->long_opt);
    } else if (opt->long_opt) {
        /* Only long option was specified */
        fprintf(stderr, "--%s", opt->long_opt);
    }
}

APARSE_INTERNAL void aparse__error_print_usage_coalesce_short_args(aparse_state* state)
{
    /* Print optional short options without arguments */
    size_t i;
    int    has_printed_short_opt_no_arg = 0;
    for (i = 0; i < state->args_size; i++) {
        const struct aparse__arg* current_opt = &state->args[i];
        /* Filter out positional options */
        if (current_opt->type == APARSE__ARG_TYPE_POSITIONAL) {
            continue;
        }
        /* Filter out required options */
        if (current_opt->required) {
            continue;
        }
        /* Filter out options with no short option */
        if (!current_opt->short_opt) {
            continue;
        }
        /* Filter out options with nargs that don't coalesce */
        if (!aparse__nargs_can_coalesce(current_opt->nargs)) {
            continue;
        }
        /* Print the initial '[-' */
        if (!has_printed_short_opt_no_arg) {
            has_printed_short_opt_no_arg = 1;
            fprintf(stderr, " [-");
        }
        fprintf(stderr, "%c", current_opt->short_opt);
    }
    if (has_printed_short_opt_no_arg) {
        fprintf(stderr, "]");
    }
}

APARSE_INTERNAL void aparse__error_print_usage_arg(aparse_state* state,
    const struct aparse__arg*                    current_arg)
{
    const char* metavar = "ARG";
    (void)(state);
    if (current_arg->metavar) {
        metavar = current_arg->metavar;
    }
    /* Optional arguments are encased in [] */
    if (!current_arg->required) {
        fprintf(stderr, "[");
    }
    /* Print option name */
    if (current_arg->type == APARSE__ARG_TYPE_OPTIONAL) {
        if (current_arg->short_opt) {
            fprintf(stderr, "-%c", current_arg->short_opt);
        } else {
            fprintf(stderr, "--%s", current_arg->long_opt);
        }
        /* Space separates the option name from the arguments */
        if (!aparse__nargs_can_coalesce(current_arg->nargs)) {
            fprintf(stderr, " ");
        }
    }
    if (current_arg->nargs == APARSE_NARGS_0_OR_1) {
        fprintf(stderr, "[%s]", metavar);
    } else if (current_arg->nargs == APARSE_NARGS_0_OR_MORE) {
        fprintf(stderr, "[%s ...]", metavar);
    } else if (current_arg->nargs == APARSE_NARGS_1_OR_MORE) {
        fprintf(stderr, "%s [%s ...]", metavar, metavar);
    } else {
        int j = (int)current_arg->nargs;
        for (j = 0; j < current_arg->nargs; j++) {
            if (j) {
                fprintf(stderr, " ");
            }
            fprintf(stderr, "%s", metavar);
        }
    }
    if (!current_arg->required) {
        fprintf(stderr, "]");
    }
}

APARSE_INTERNAL void aparse__error_print_usage(aparse_state* state)
{
    size_t                    i;
    const struct aparse__arg* current_arg;
    if (state->argc == 0) {
        fprintf(stderr, "usage:");
    } else {
        fprintf(stderr, "usage: %s", state->argv[0]);
    }
    /* Print optional short options with no arguments first */
    aparse__error_print_usage_coalesce_short_args(state);
    /* Print other optional options */
    for (i = 0; i < state->args_size; i++) {
        current_arg = &state->args[i];
        /* Filter out positionals */
        if (current_arg->type == APARSE__ARG_TYPE_POSITIONAL) {
            continue;
        }
        /* Filter out required options */
        if (current_arg->required) {
            continue;
        }
        /* Filter out options we printed in coalesce */
        if (aparse__nargs_can_coalesce(current_arg->nargs)) {
            continue;
        }
        fprintf(stderr, " ");
        aparse__error_print_usage_arg(state, current_arg);
    }
    /* Print mandatory options */
    for (i = 0; i < state->args_size; i++) {
        current_arg = &state->args[i];
        /* Filter out positionals */
        if (current_arg->type == APARSE__ARG_TYPE_POSITIONAL) {
            continue;
        }
        /* Filter out optional options */
        if (!current_arg->required) {
            continue;
        }
        fprintf(stderr, " ");
        aparse__error_print_usage_arg(state, current_arg);
    }
    /* Print mandatory positionals */
    for (i = 0; i < state->args_size; i++) {
        current_arg = &state->args[i];
        /* Filter out optionals */
        if (current_arg->type == APARSE__ARG_TYPE_OPTIONAL) {
            continue;
        }
        /* Filter out optional positionals */
        if (!current_arg->required) {
            continue;
        }
        fprintf(stderr, " ");
        aparse__error_print_usage_arg(state, current_arg);
    }
    /* Print optional positionals */
    for (i = 0; i < state->args_size; i++) {
        current_arg = &state->args[i];
        /* Filter out optionals */
        if (current_arg->type == APARSE__ARG_TYPE_OPTIONAL) {
            continue;
        }
        /* Filter out mandatory positionals */
        if (current_arg->required) {
            continue;
        }
        fprintf(stderr, " ");
        aparse__error_print_usage_arg(state, current_arg);
    }
    fprintf(stderr, "\n");
}

APARSE_INTERNAL void aparse__error_begin(aparse_state* state)
{
    aparse__error_print_usage(state);
    if (state->argc == 0) {
        fprintf(stderr, "error: ");
    } else {
        fprintf(stderr, "%s: error: ", state->argv[0]);
    }
}

APARSE_INTERNAL void aparse__error_end(aparse_state* state)
{
    APARSE__UNUSED(state);
    fprintf(stderr, "\n");
}

APARSE_INTERNAL void aparse__error_arg_begin(aparse_state* state)
{
    APARSE__UNUSED(state);
}

APARSE_INTERNAL void aparse__error_arg_end(aparse_state* state)
{
    APARSE__UNUSED(state);
}

#endif

/* aparse */
#include <stdio.h>

APARSE_INTERNAL aparse_error aparse__state_default_out_cb(void* user, const char* buf, aparse_size buf_size) {
    APARSE__UNUSED(user);
    if (fwrite(buf, buf_size, 1, stdout) != 1) {
        return APARSE_ERROR_OUT;
    }
    return APARSE_ERROR_NONE;
}

APARSE_INTERNAL void aparse__state_init(aparse__state* state) {
    state->head = APARSE_NULL;
    state->tail = APARSE_NULL;
    state->help = APARSE_NULL;
    state->help_size = 0;
    state->out_cb = aparse__state_default_out_cb;
    state->user = APARSE_NULL;
    state->root = APARSE_NULL;
    state->is_root = 0;
}

#if 0

APARSE_INTERNAL void aparse__state_init_from(aparse__state* state, aparse__state* other) {
    aparse__state_init(state);
    state->out_cb = other->out_cb;
    state->user = other->user;
    state->root = other->root;
}

#endif

APARSE_INTERNAL void aparse__state_destroy(aparse__state* state) {
    aparse__arg* arg = state->head;
    while (arg) {
        aparse__arg* prev = arg;
        arg = arg->next;
        aparse__arg_destroy(prev);
        APARSE_FREE(prev);
    }
    if (state->is_root) {
        if (state->root != APARSE_NULL) {
            APARSE_FREE(state->root);
        }
    }
}

APARSE_INTERNAL void aparse__state_set_out_cb(aparse__state* state, aparse_out_cb out_cb, void* user) {
    state->out_cb = out_cb;
    state->user = user;
}

APARSE_INTERNAL void aparse__state_reset(aparse__state* state) {
    aparse__arg* cur = state->head;
    while (cur) {
        cur->was_specified = 0;
        if (cur->type == APARSE__ARG_TYPE_SUBCOMMAND) {
            aparse__sub* sub = cur->contents.sub.head;
            while (sub) {
                aparse__state_reset(&sub->subparser);
                sub = sub->next;
            }
        }
        cur = cur->next;
    }
}

APARSE_INTERNAL aparse_error aparse__state_arg(aparse__state* state) {
    aparse__arg* arg = (aparse__arg*)APARSE_MALLOC(sizeof(aparse__arg));
    if (arg == APARSE_NULL) {
        return APARSE_ERROR_NOMEM;
    }
    aparse__arg_init(arg);
    if (state->head == APARSE_NULL) {
        state->head = arg;
        state->tail = arg;
    } else {
        state->tail->next = arg;
        state->tail = arg;
    }
    return APARSE_ERROR_NONE;
}

APARSE_INTERNAL void aparse__state_check_before_add(aparse__state* state) {
    /* for release builds */
    APARSE__UNUSED(state);

    /* If this fails, you forgot to specifiy a type for the previous argument. */
    APARSE_ASSERT(APARSE__IMPLIES(state->tail != APARSE_NULL, state->tail->callback != APARSE_NULL));
}

APARSE_INTERNAL void aparse__state_check_before_modify(aparse__state* state) {
    /* for release builds */
    APARSE__UNUSED(state);

    /* If this fails, you forgot to call add_opt() or add_pos(). */
    APARSE_ASSERT(state->tail != APARSE_NULL);
}

APARSE_INTERNAL void aparse__state_check_before_set_type(aparse__state* state) {
    /* for release builds */
    APARSE__UNUSED(state);

    /* If this fails, you forgot to call add_opt() or add_pos(). */
    APARSE_ASSERT(state->tail != APARSE_NULL);

    /* If this fails, you are trying to set the argument type of a subcommand. */
    APARSE_ASSERT(state->tail->type != APARSE__ARG_TYPE_SUBCOMMAND);

    /* If this fails, you called arg_xxx() twice. */
    APARSE_ASSERT(state->tail->callback == APARSE_NULL);
}

APARSE_INTERNAL aparse_error aparse__state_add_opt(aparse__state* state, char short_opt, const char* long_opt) {
    aparse_error err = APARSE_ERROR_NONE;
    /* If either of these fail, you specified both short_opt and long_opt as
     * NULL. For a positional argument, use aparse__add_pos. */
    APARSE_ASSERT(APARSE__IMPLIES(short_opt == '\0', long_opt != APARSE_NULL));
    APARSE_ASSERT(APARSE__IMPLIES(long_opt == APARSE_NULL, short_opt != '\0'));
    if ((err = aparse__state_arg(state))) {
        return err;
    }
    state->tail->type = APARSE__ARG_TYPE_OPTIONAL;
    state->tail->contents.opt.short_opt = short_opt;
    state->tail->contents.opt.long_opt = long_opt;
    if (long_opt != APARSE_NULL) {
        state->tail->contents.opt.long_opt_size = aparse__slen(long_opt);
    } else {
        state->tail->contents.opt.long_opt_size = 0;
    }
    return err;
}

APARSE_INTERNAL aparse_error aparse__state_add_pos(aparse__state* state, const char* name) {
    aparse_error err = APARSE_ERROR_NONE;
    if ((err = aparse__state_arg(state))) {
        return err;
    }
    state->tail->type = APARSE__ARG_TYPE_POSITIONAL;
    state->tail->contents.pos.name = name;
    state->tail->contents.pos.name_size = aparse__slen(name);
    return err;
}

APARSE_INTERNAL aparse_error aparse__state_add_sub(aparse__state* state) {
    aparse_error err = APARSE_ERROR_NONE;
    if ((err = aparse__state_arg(state))) {
        return err;
    }
    aparse__arg_sub_init(state->tail);
    return err;
}

#if 0
APARSE_INTERNAL aparse_error aparse__state_sub_add_cmd(aparse__state* state, const char* name, aparse__state** subcmd) {
    aparse__sub* sub = (aparse__sub*)APARSE_MALLOC(sizeof(aparse__sub));
    APARSE_ASSERT(state->tail->type == APARSE__ARG_TYPE_SUBCOMMAND);
    APARSE_ASSERT(name != APARSE_NULL);
    if (sub == APARSE_NULL) {
        return APARSE_ERROR_NOMEM;
    }
    sub->name = name;
    sub->name_size = aparse__slen(name);
    aparse__state_init_from(&sub->subparser, state);
    sub->next = APARSE_NULL;
    if (state->tail->contents.sub.head == APARSE_NULL) {
        state->tail->contents.sub.head = sub;
        state->tail->contents.sub.tail = sub;
    } else {
        state->tail->contents.sub.tail->next = sub;
        state->tail->contents.sub.tail = sub;
    }
    *subcmd = &sub->subparser;
    return 0;
}

#endif


APARSE_INTERNAL aparse_error aparse__state_flush(aparse__state* state) {
    aparse_error err = APARSE_ERROR_NONE;
    if (state->root->out_buf_ptr) {
        if ((err = state->out_cb(state->user, state->root->out_buf, state->root->out_buf_ptr))) {
            return err;
        }
        state->root->out_buf_ptr = 0;
    }
    return err;
}

APARSE_INTERNAL aparse_error aparse__state_out(aparse__state* state, char out) {
    aparse_error err = APARSE_ERROR_NONE;
    if (state->root->out_buf_ptr == APARSE__STATE_OUT_BUF_SIZE) {
        if ((err = aparse__state_flush(state))) {
            return err;
        }
    }
    state->root->out_buf[state->root->out_buf_ptr++] = out;
    return err;
}

APARSE_INTERNAL aparse_error aparse__state_out_s(aparse__state* state, const char* s) {
    aparse_error err = APARSE_ERROR_NONE;
    while (*s) {
        if ((err = aparse__state_out(state, *s))) {
            return err;
        }
        s++;
    }
    return err;
}

APARSE_INTERNAL aparse_error aparse__state_out_n(aparse__state* state, const char* s, aparse_size n) {
    aparse_error err = APARSE_ERROR_NONE;
    aparse_size i;
    for (i = 0; i < n; i++) {
        if ((err = aparse__state_out(state, s[i]))) {
            return err;
        }
    }
    return err;
}

/* aparse */
/* accepts an lvalue */
#define APARSE__NEXT_POSITIONAL(n)                                            \
    while ((n) != APARSE_NULL && (n)->type != APARSE__ARG_TYPE_POSITIONAL) {      \
        (n) = (n)->next;                                                      \
    }

int aparse__is_positional(const char* arg_text) {
    return (arg_text[0] == '\0')                          /* empty string */
           || (arg_text[0] == '-' && arg_text[1] == '\0') /* just a dash */
           || (arg_text[0] == '-' && arg_text[1] == '-' &&
               arg_text[2] == '\0') /* two dashes */
           || (arg_text[0] != '-'); /* all positionals */
}

/* Returns NULL if the option does not match. */
const char* aparse__arg_match_long_opt(
  const struct aparse__arg* opt, const char* arg_without_dashes) {
    aparse_size a_pos = 0;
    const char* a_str = opt->contents.opt.long_opt;
    const char* b = arg_without_dashes;
    while (1) {
        if (a_pos == opt->contents.opt.long_opt_size) {
            if (*b != '\0' && *b != '=') {
                return NULL;
            } else {
                /* *b equals '\0' or '=' */
                return b;
            }
        }
        if (*b == '\0' || a_str[a_pos] != *b) {
            /* b ended first or a and b do not match */
            return NULL;
        }
        a_pos++;
        b++;
    }
    return NULL;
}

APARSE_API aparse_error
aparse__parse_argv(aparse__state* state, int argc, const char* const* argv) {
    aparse_error err = APARSE_ERROR_NONE;
    int argc_idx = 0;
    aparse__arg* next_positional = state->head;
    aparse_size arg_text_size;
    while (next_positional != APARSE_NULL &&
           next_positional->type != APARSE__ARG_TYPE_POSITIONAL) {
        next_positional = next_positional->next;
    }
    aparse__state_reset(state);
    while (argc_idx < argc) {
        const char* arg_text = argv[argc_idx++];
        if (aparse__is_positional(arg_text)) {
            if (next_positional == APARSE_NULL) {
                if ((err = aparse__error_unrecognized_arg(state, arg_text))) {
                    return err;
                }
                return APARSE_ERROR_PARSE;
            }
            arg_text_size = aparse__slen((const aparse_char*)arg_text);
            if ((err = next_positional->callback(
                   next_positional, state, 0, arg_text, arg_text_size))) {
                return err;
            }
            next_positional = next_positional->next;
            while (next_positional != APARSE_NULL &&
                   next_positional->type != APARSE__ARG_TYPE_POSITIONAL) {
                next_positional = next_positional->next;
            }
        } else {
            int is_long = 0;
            const char* arg_end;
            if (arg_text[0] == '-' && arg_text[1] != '-') {
                arg_end = arg_text + 1;
            } else {
                arg_end = arg_text + 2;
                is_long = 1;
            }
            do {
                aparse__arg* arg = state->head;
                int has_text_left = 0;
                if (!is_long) {
                    char short_opt = *(arg_end++);
                    while (1) {
                        if (arg == APARSE_NULL) {
                            break;
                        }
                        if (arg->type == APARSE__ARG_TYPE_OPTIONAL) {
                            if (arg->contents.opt.short_opt == short_opt) {
                                break;
                            }
                        }
                        arg = arg->next;
                    }
                    if (arg == APARSE_NULL) {
                        if ((err = aparse__error_unrecognized_short_arg(
                               state, short_opt))) {
                            return err;
                        }
                        return APARSE_ERROR_PARSE;
                    }
                    has_text_left = *arg_end != '\0';
                } else {
                    while (1) {
                        if (arg == APARSE_NULL) {
                            break;
                        }
                        if (arg->type == APARSE__ARG_TYPE_OPTIONAL) {
                            if (arg->contents.opt.long_opt != APARSE_NULL) {
                                aparse_size opt_pos = 0;
                                const char* opt_ptr =
                                  arg->contents.opt.long_opt;
                                const char* arg_ptr = arg_end;
                                int found = 0;
                                while (1) {
                                    if (
                                      opt_pos ==
                                      arg->contents.opt.long_opt_size) {
                                        if (
                                          *arg_ptr != '\0' &&
                                          *arg_ptr != '=') {
                                            break;
                                        } else {
                                            /* *b equals '\0' or '=' */
                                            arg_end = arg_ptr;
                                            found = 1;
                                            break;
                                        }
                                    }
                                    if (
                                      *arg_ptr == '\0' ||
                                      opt_ptr[opt_pos] != *arg_ptr) {
                                        /* b ended first or a and b do not
                                         * match */
                                        break;
                                    }
                                    opt_pos++;
                                    arg_ptr++;
                                }
                                if (found) {
                                    break;
                                }
                            }
                        }
                        arg = arg->next;
                    }
                    if (arg == APARSE_NULL) {
                        if ((err = aparse__error_unrecognized_arg(
                               state, arg_text))) {
                            return err;
                        }
                        return APARSE_ERROR_PARSE;
                    }
                }
                if (*arg_end == '=') {
                    /* use equals as argument */
                    if (arg->nargs == 0 || arg->nargs > 1) {
                        if ((err = aparse__error_begin_arg(state, arg))) {
                            return err;
                        }
                        if ((err = aparse__state_out_s(
                               state, "cannot parse '='\n"))) {
                            return err;
                        }
                        return APARSE_ERROR_PARSE;
                    } else {
                        arg_end++;
                        if ((err = arg->callback(
                               arg, state, 0, arg_end, aparse__slen(arg_end)))) {
                            return err;
                        }
                    }
                    break;
                } else if (has_text_left) {
                    /* use rest of arg as argument */
                    if (arg->nargs > 1) {
                        if ((err = aparse__error_begin_arg(state, arg))) {
                            return err;
                        }
                        if ((err =
                               aparse__state_out_s(state, "cannot parse '"))) {
                            return err;
                        }
                        if ((err = aparse__state_out_s(state, arg_end))) {
                            return err;
                        }
                        if ((err = aparse__state_out(state, '\n'))) {
                            return err;
                        }
                        return APARSE_ERROR_PARSE;
                    } else if (
                      arg->nargs != APARSE_NARGS_0_OR_1_EQ &&
                      arg->nargs != 0) {
                        if ((err = arg->callback(
                               arg, state, 0, arg_end, aparse__slen(arg_end)))) {
                            return err;
                        }
                        break;
                    } else {
                        if ((err = arg->callback(arg, state, 0, APARSE_NULL, 0))) {
                            return err;
                        }
                        /* fallthrough, continue parsing short options */
                    }
                } else if (
                  argc_idx == argc || !aparse__is_positional(argv[argc_idx])) {
                    if (
                      arg->nargs == APARSE_NARGS_1_OR_MORE ||
                      arg->nargs == 1 || arg->nargs > 1) {
                        if ((err = aparse__error_begin_arg(state, arg))) {
                            return err;
                        }
                        if ((err = aparse__state_out_s(
                               state, "expected an argument\n"))) {
                            return err;
                        }
                        return APARSE_ERROR_PARSE;
                    } else if (
                      arg->nargs == APARSE_NARGS_0_OR_1_EQ ||
                      arg->nargs == 0) {
                        if ((err = arg->callback(arg, state, 0, APARSE_NULL, 0))) {
                            return err;
                        }
                        /* fallthrough */
                    } else {
                        if ((err = arg->callback(arg, state, 0, APARSE_NULL, 0))) {
                            return err;
                        }
                    }
                    break;
                } else {
                    if (arg->nargs == APARSE_NARGS_0_OR_1 || arg->nargs == 1) {
                        arg_text = argv[argc_idx++];
                        arg_text_size = aparse__slen(arg_text);
                        if ((err = arg->callback(
                               arg, state, 0, arg_text, arg_text_size))) {
                            return err;
                        }
                    } else if (
                      arg->nargs == APARSE_NARGS_0_OR_1_EQ ||
                      arg->nargs == 0) {
                        if ((err = arg->callback(arg, state, 0, APARSE_NULL, 0))) {
                            return err;
                        }
                    } else {
                        aparse_size sub_arg_idx = 0;
                        while (argc_idx < argc) {
                            arg_text = argv[argc_idx++];
                            arg_text_size = aparse__slen(arg_text);
                            if ((err = arg->callback(
                                   arg, state, sub_arg_idx++, arg_text,
                                   arg_text_size))) {
                                return err;
                            }
                            if ((int)sub_arg_idx == arg->nargs) {
                                break;
                            }
                        }
                        if ((int)sub_arg_idx != arg->nargs) {
                            if ((err = aparse__error_begin_arg(state, arg))) {
                                return err;
                            }
                            if ((err = aparse__state_out_s(
                                   state, "expected an argument\n"))) {
                                return err;
                            }
                            return APARSE_ERROR_PARSE;
                        }
                    }
                    break;
                }
            } while (!is_long);
        }
    }
    if (next_positional) {
        aparse__error_begin_arg(state, next_positional);
        aparse__state_out_s(state, "missing positional argument\n");
        err = APARSE_ERROR_PARSE;
    }
    return err;
}

#undef APARSE__NEXT_POSITIONAL

#endif /* APARSE_IMPLEMENTATION */
#endif /* APARSE_H */
#ifdef __cplusplus
}
#endif
