#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct prec {
  int lvl;
  const char *op, *nick;
} prec;

#define PRECDEF(l, o, n)                                                       \
  { l, o, n } /* gives us better formatting w/ clang format */

const prec PREC[] = {
    PRECDEF(1, "++x --x", "prefix incr/decr"),
    PRECDEF(1, "x()", "function call"),
    PRECDEF(1, "x[y]", "array subscript"),
    PRECDEF(1, "x.y", "member access"),
    PRECDEF(1, "x->y", "member dereference"),
    PRECDEF(1, "(T){x}", "compound literal"),
    PRECDEF(2, "x++ x--", "suffix incr/decr"),
    PRECDEF(2, "+x  -x", "unary plus/minus"),
    PRECDEF(2, "!x  ~x", "logical/bitwise not"),
    PRECDEF(2, "(T)x", "cast"),
    PRECDEF(2, "*x", "pointer dereference"),
    PRECDEF(2, "&x", "address-of"),
    PRECDEF(2, "sizeof x", "sizeof operator"),
    PRECDEF(3, "x*y x/y x%y", "mul, div, mod"),
    PRECDEF(4, "x+y x-y", "add, sub"),
    PRECDEF(5, "x<<y  x>>y", "bitwise shift L/R"),
    PRECDEF(6, "x<y   x<=y", "less, less or equal"),
    PRECDEF(6, "x>y   x>=y", "greater, greater or equal"),
    PRECDEF(7, "x==y  x!=y", "equals, not equals"),
    PRECDEF(8, "x&y", "bitwise AND"),
    PRECDEF(9, "x^y", "bitwise XOR"),
    PRECDEF(10, "x|y", "bitwise OR"),
    PRECDEF(11, "x&&y", "logical AND"),
    PRECDEF(12, "x||y", "logical OR"),
    PRECDEF(13, "x?y:z", "ternary conditional"),
    PRECDEF(14, "x=y   x+=y  x-=y", "assign, &add/sub"),
    PRECDEF(14, "x*=y  x/=y  x%=y", "...&mul/div/mod"),
    PRECDEF(14, "x<<=y x>>=y", "...&bitwise shift L/R"),
    PRECDEF(14, "x&=y  x^=y  x|=y", "...&bitwise and/xor/or"),
    PRECDEF(15, "x,y", "comma"),
};

#define CSI "\x1b["

const char *format_op(const prec *pp, int width) {
  const char *in = pp->op;
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
      p += sprintf(p, CSI "%im", (color = next_color));
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
      printf(CSI "4m");
    else
      printf(CSI "24m");
    if (p == PREC || p->lvl != (p - 1)->lvl)
      printf(CSI "7;97m%2i " CSI "27m", p->lvl);
    else
      printf(CSI "7;97m   " CSI "27m");
    printf(" %s" CSI "97m%-20s ", format_op(p, 12), p->nick);
    if (p2 + 1 != PREC + nprec && (p2 + 1)->lvl != p2->lvl)
      printf(CSI "4m");
    else
      printf(CSI "24m");
    if (p2 == PREC || p2->lvl != (p2 - 1)->lvl)
      printf(CSI "7m %2i " CSI "27m", p2->lvl);
    else
      printf(CSI "7m    " CSI "27m");
    printf(" %s " CSI "97m%s" CSI "0m\n", format_op(p2, 16), p2->nick);
  }
  return 0;
}
