#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

#define PLAT_UNIX

static const uint8_t utf8d[] = {
    // The first part of the table maps bytes to character classes that
    // to reduce the size of the transition table and create bitmasks.
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9,
    9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 10,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 3, 3, 11, 6, 6, 6, 5, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8,

    // The second part is a transition table that maps a combination
    // of a state of the automaton and a character class to a state.
    0, 12, 24, 36, 60, 96, 84, 12, 12, 12, 48, 72, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 0, 12, 12, 12, 12, 12, 0, 12, 0, 12, 12, 12, 24, 12,
    12, 12, 12, 12, 24, 12, 24, 12, 12, 12, 12, 12, 12, 12, 12, 12, 24, 12, 12,
    12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 36, 12, 36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 36, 12, 36, 12, 12,
    12, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12};

uint32_t utf_decode(uint32_t *state, uint32_t *codep, uint32_t byte) {
  uint32_t type = utf8d[byte];

  *codep = (*state != UTF8_ACCEPT) ? (byte & 0x3fu) | (*codep << 6)
                                   : (0xff >> type) & (byte);

  *state = utf8d[256 + *state + type];
  return *state;
}

char *names[128] = {
    "NULL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS",  "HT",
    "LF",   "VT",  "FF",  "CR",  "SO",  "SI",  "DLE", "DC1", "DC2", "DC3",
    "DC4",  "NAK", "SYN", "ETB", "CAN", "EM",  "SUB", "ESC", "FS",  "GS",
    "RS",   "US",  " ",   "!",   "\"",  "#",   "$",   "%",   "&",   "'",
    "(",    ")",   "*",   "+",   ",",   "-",   ".",   "/",   "0",   "1",
    "2",    "3",   "4",   "5",   "6",   "7",   "8",   "9",   ":",   ";",
    "<",    "=",   ">",   "?",   "@",   "A",   "B",   "C",   "D",   "E",
    "F",    "G",   "H",   "I",   "J",   "K",   "L",   "M",   "N",   "O",
    "P",    "Q",   "R",   "S",   "T",   "U",   "V",   "W",   "X",   "Y",
    "Z",    "[",   "\\",  "]",   "^",   "_",   "`",   "a",   "b",   "c",
    "d",    "e",   "f",   "g",   "h",   "i",   "j",   "k",   "l",   "m",
    "n",    "o",   "p",   "q",   "r",   "s",   "t",   "u",   "v",   "w",
    "x",    "y",   "z",   "{",   "|",   "}",   "~",   "DEL"};

#ifdef PLAT_UNIX
struct termios orig_termios;
#include <termios.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

void cleanup(void) {
  printf("\x1b[?1000l\x1b[?1002l\x1b[?1015l\x1b[?1006l\n");
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void sig(int s) {
  if (s == SIGINT || s == SIGQUIT) {
    cleanup();
    exit(0);
  }
}

int main(void) {
#ifdef PLAT_UNIX
  struct termios raw;
  tcgetattr(STDIN_FILENO, &orig_termios);
  raw = orig_termios;
  /* Don't map carriage return to newline on input. */
  raw.c_iflag &= ~(ICRNL);
  /* Disable input parity check (probably useless) */
  raw.c_iflag &= ~(INPCK);
  /* Don't strip characters to 7 bits. */
  raw.c_iflag &= ~(ISTRIP);
  /* Disable start/stop output control (read START/STOP characters) */
  raw.c_iflag &= ~(IXON);
  /* Don't post process output so that it looks good. */
  raw.c_oflag &= ~(OPOST);
  /* 8-bit characters. */
  raw.c_cflag |= (CS8);
  /* Disable echoing. */
  raw.c_lflag &= ~(ECHO);
  /* Disable cooked (canonical) mode. */
  raw.c_lflag &= ~(ICANON);
  /* Disable extended input character processing. */
  raw.c_lflag &= ~(IEXTEN);
  /* Disable signals INTR, QUIT, SUSP. */
  /*raw.c_lflag &= ~(ISIG);*/
  /* Ensure that read() always has 1 character (might block) */
  raw.c_cc[VMIN] = 1;
  /* No time constraint */
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
#else
  HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode;
  GetConsoleMode(hOutput, &dwMode);
  dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOutput, dwMode);
  GetConsoleMode(hInput, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
  SetConsoleMode(hInput, dwMode);
#endif
  printf("\x1b[?1000h\x1b[?1002h\x1b[?1015h\x1b[?1006h\x1b[?1003h\n");
  signal(SIGINT, sig);
  signal(SIGQUIT, sig);
  atexit(&cleanup);
  int c = 0;
  uint32_t codepoint;
  uint32_t state = 0;
  while (fread(&c, 1, 1, stdin) == 1 && c != 0x03) {
    if (c == 0x02) {
      printf("\x05");
    } else if (c < 0x80) {
      printf("%02X - ('%s')", c, names[c]);
    } else if (c >= 0xC0 && c < 0xF8) {
      printf("%02X - (utf start)", c);
    } else if (c < 0xF8) {
      printf("%02X - (utf continuation)", c);
    } else {
      printf("%X ", c);
    }
    if (!utf_decode(&state, &codepoint, c & 0xFF)) {
      printf(" - U+%X\r\n", codepoint);
    } else {
      printf("\r\n");
    }
  }
}
