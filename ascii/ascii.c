#include <stdio.h>

const char *ctrls[] = {"\\0", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "\\a",
                       "\\b", "\\t", "\\n", "\\v", "\\f", "\\r", "SO",  "SI",
                       "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
                       "CAN", "EM",  "SUB", "\\e", "FS",  "GS",  "RS",  "US"};

void swatch(int color) { printf("\x1b[%im", color); }

void lo(void) {
  int i;
  printf("    ");
  for (i = 0; i < 16; i++) {
    printf("\x1b[7m  0%X\x1b[0m", i);
  }
  printf("\n");
}

void hi(int j) { printf("\x1b[0m\x1b[7m %X0 \x1b[0m", j); }

int main(void) {
  int i, j;
  lo();
  for (j = 0; j < 16; j++) {
    hi(j);
    for (i = 0; i < 16; i++) {
      int idx = 16 * j + i;
      if (idx < 32) {
        swatch(36);
        printf("%4s", ctrls[idx]);
      } else if (idx < 0x7F) {
        swatch(0);
        printf("%4c", idx);
      } else if (idx == 0x7F) {
        swatch(31);
        printf(" DEL");
      } else if (idx < 0xC0) {
        swatch(35);
        printf(" +%02X", idx - 0x80);
      } else if (idx < 0xE0) {
        swatch(idx < 0xC2 ? 30 : 32);
        printf("  U2");
      } else if (idx < 0xF0) {
        swatch((idx == 0xE0 || idx == 0xED) ? 33 : 32);
        printf("  U3");
      } else if (idx < 0xF8) {
        swatch((idx < 0xF5) ? (idx == 0xF0 || idx == 0xF4) ? 33 : 32 : 30);
        printf("  U4");
      } else {
        swatch(30);
        if (idx < 0xFE) {
          printf("  U%i", idx < 0xF8 ? 4 : idx < 0xFC ? 5 : 6);
        } else {
          printf(" BOM");
        }
      }
    }
    printf("\x1b[0m\n");
  }
  return 0;
}
