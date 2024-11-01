#include <math.h>
#include <stdio.h>

int digits(int n) {
  if (n < 10) {
    return 1;
  } else if (n < 100) {
    return 2;
  }
  return 3;
}

void swatch(int color, int width) {
  int space = width - digits(color), right = space / 2, left = space - right;
  printf("%*s%d%*s", left, "", color, right, "");
}

void csi(void) { printf("\x1b["); }

void sgr(int n) {
  csi();
  printf("%im", n);
}

void bg256(int bg) {
  csi();
  printf("48;5;%im", bg);
}

void sep(void) {
  sgr(0);
  printf(" ");
}

void feed(void) {
  sgr(0);
  printf("\n");
}

void ramp8_fg(int start) {
  int i;
  for (i = start; i < start + 8; i++) {
    sgr(i == start && start == 30 ? 100 : 0);
    sgr(i);
    swatch(i, 4);
  }
}

void ramp8_bg(int start) {
  int i;
  for (i = start; i < start + 8; i++) {
    sgr(i);
    sgr(i == start && start == 40 ? 37 : 30);
    swatch(i, 4);
  }
}

void ramp8_256(int start) {
  int i;
  for (i = start; i < start + 8; i++) {
    sgr(i == start && start == 0 ? 37 : 30);
    bg256(i);
    swatch(i, 4);
  }
}

void indent(int spc) { printf("%*s", spc, ""); }

void label(const char *text) {
  sgr(1);
  sgr(3);
  sgr(7);
  printf("%s", text);
  sgr(0);
}

int main(void) {
  int i, j, k;
  label("ANSI ");
  ramp8_fg(30);
  ramp8_bg(40);
  feed();
  label(" Ext ");
  ramp8_fg(90);
  ramp8_bg(100);
  feed();
  label(" 256 ");
  ramp8_256(0);
  ramp8_256(8);
  feed();
  for (k = 0; k < 12; k++) {
    for (j = 0; j < 3; j++) {
      for (i = 0; i < 6; i++) {
        int idx = 16 + 72 * j + (6 * (k < 6 ? k : 17 - k)) + i;
        int g = k < 6 ? k : 12 - k, r = j * 2 + (k > 6);
        int lum = r * 3 + g * 10 + i; /* quick 'n' dirty std luminance */
        sgr(lum > 30 ? 30 : 37);
        bg256(idx);
        swatch(idx, 4);
      }
      (j == 2) ? feed() : sep();
    }
  }
  for (i = 0; i < 2; i++) {
    indent(1);
    for (j = 0; j < 12; j++) {
      int idx = 232 + 12 * i + j;
      sgr(i ? 30 : 37);
      bg256(idx);
      swatch(idx, 6);
    }
    feed();
  }
  {
    int on[] = {1, 4, 3, 9, 7, 2, 5, 21, 6};
    int off[] = {22, 24, 23, 29, 27, 22, 25, 28, 25};
    label("Attr ");
    indent(5);
    for (i = 0; i < 9; i++) {
      sgr(on[i]);
      swatch(on[i], 2);
      sgr(0);
      printf("/");
      sgr(off[i]);
      swatch(off[i], 2);
      if (i < 8)
        sep();
    }
    feed();
  }
  return 0;
}
