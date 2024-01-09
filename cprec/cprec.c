#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct prec {
  int lvl;
  const char *op, *nick;
} prec;

const prec PREC[] = {
    {
        1,
        "++x --x",
        "prefix incr/decr",
    },
    {
        1,
        "x()",
        "function call",
    },
    {
        1,
        "x[y]",
        "array subscript",
    },
    {
        1,
        "x.y",
        "member access",
    },
    {
        1,
        "x->y",
        "member dereference",
    },
    {
        1,
        "(T){x}",
        "compound literal",
    },
    {
        2,
        "x++ x--",
        "suffix incr/decr",
    },
    {
        2,
        "+x  -x",
        "unary plus/minus",
    },
    {
        2,
        "!x  ~x",
        "logical/bitwise not",
    },
    {
        2,
        "(T)x",
        "cast",
    },
    {
        2,
        "*x",
        "pointer dereference",
    },
    {
        2,
        "&x",
        "address-of",
    },
    {
        2,
        "sizeof x",
        "sizeof operator",
    },
    {
        3,
        "x*y x/y x%y",
        "mul, div, mod",
    },
    {
        4,
        "x+y x-y",
        "add, sub",
    },
    {
        5,
        "x<<y  x>>y",
        "bitwise shift L/R",
    },
    {
        6,
        "x<y   x<=y",
        "less, less or equal",
    },
    {
        6,
        "x>y   x>=y",
        "greater, greater or equal",
    },
    {
        7,
        "x==y  x!=y",
        "equals, not equals",
    },
    {
        8,
        "x&y",
        "bitwise AND",
    },
    {
        9,
        "x^y",
        "bitwise XOR",
    },
    {
        10,
        "x|y",
        "bitwise OR",
    },
    {
        11,
        "x&&y",
        "logical AND",
    },
    {
        12,
        "x||y",
        "logical OR",
    },
    {
        13,
        "x?y:z",
        "ternary conditional",
    },
    {
        14,
        "x=y   x+=y  x-=y",
        "assign, &add/sub",
    },
    {
        14,
        "x*=y  x/=y  x%=y",
        "...&mul/div/mod",
    },
    {
        14,
        "x<<=y x>>=y",
        "...&bitwise shift L/R",
    },
    {
        14,
        "x&=y  x^=y  x|=y",
        "...&bitwise and/xor/or",
    },
    {
        15,
        "x,y",
        "comma",
    },
};

const char *format_op(const prec *prec, int width) {
  const char *in = prec->op;
  char *out = malloc(width * 8);
  char *p = out;
  int color = -1, i = 0;
  for (; i < width; i++) {
    int next_color = color;
    if (*in == 'x' || *in == 'y' || (*in == 'z' && *(in - 1) != 'i') ||
        *in == 'T')
      next_color = 90;
    else
      next_color = 96;
    if (next_color != color)
      p += sprintf(p, "\x1b[%im", (color = next_color));
    *(p++) = *in ? *(in++) : ' ';
  }
  *(p++) = '\0';
  return out;
}

int main(void) {
  const prec *p, *p2;
  int nprec = (sizeof(PREC) / sizeof(prec));
  for (p = PREC, p2 = PREC + nprec / 2; p < PREC + nprec / 2; p++, p2++) {
    if (p + 1 != PREC + nprec / 2 && (p + 1)->lvl != p->lvl)
      printf("\x1b[4m");
    else
      printf("\x1b[24m");
    if (p == PREC || p->lvl != (p - 1)->lvl)
      printf("\x1b[7;97m%2i \x1b[27m", p->lvl);
    else
      printf("\x1b[7;97m   \x1b[27m");
    printf(" %s\x1b[97m%-20s ", format_op(p, 12), p->nick);
    if (p2 + 1 != PREC + nprec && (p2 + 1)->lvl != p2->lvl)
      printf("\x1b[4m");
    else
      printf("\x1b[24m");
    if (p2 == PREC || p2->lvl != (p2 - 1)->lvl)
      printf("\x1b[7m %2i \x1b[27m", p2->lvl);
    else
      printf("\x1b[7m    \x1b[27m");
    printf(" %s \x1b[97m%s\x1b[0m\n", format_op(p2, 16), p2->nick);
  }
  return 0;
}
