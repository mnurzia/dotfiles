#include <stdio.h>
#include <string.h>
typedef enum finger {
  FINGER_T,
  FINGER_LP,
  FINGER_LR,
  FINGER_LM,
  FINGER_LI,
  FINGER_RI,
  FINGER_RM,
  FINGER_RR,
  FINGER_RP
} finger;

int colors[] = {1, 6, 2, 3, 4, 5, 3, 2, 6};

typedef struct key {
  finger finger;
  const char* bot;
  const char* top;
  int width;
} key;

#define KEYDEF(f, t, b, w)                                                     \
  {                                                                            \
    FINGER_##f, t, b, w                                                        \
  }

const key keyboard[] = {
    KEYDEF(LP, "`", "~", 1),
    KEYDEF(LP, "1", "!", 1),
    KEYDEF(LR, "2", "@", 1),
    KEYDEF(LM, "3", "#", 1),
    KEYDEF(LM, "4", "$", 1),
    KEYDEF(LI, "5", "%", 1),
    KEYDEF(LI, "6", "^", 1),
    KEYDEF(RI, "7", "&", 1),
    KEYDEF(RM, "8", "*", 1),
    KEYDEF(RR, "9", "(", 1),
    KEYDEF(RP, "0", ")", 1),
    KEYDEF(RP, "-", "_", 1),
    KEYDEF(RP, "=", "+", 1),
    KEYDEF(RP, "bksp", "", 5),
    {0},
    KEYDEF(LP, "tab", "", 3),
    KEYDEF(LP, "Q", "", 1),
    KEYDEF(LR, "W", "", 1),
    KEYDEF(LM, "E", "", 1),
    KEYDEF(LI, "R", "", 1),
    KEYDEF(LI, "T", "", 1),
    KEYDEF(RI, "Y", "", 1),
    KEYDEF(RI, "U", "", 1),
    KEYDEF(RM, "I", "", 1),
    KEYDEF(RR, "O", "", 1),
    KEYDEF(RP, "P", "", 1),
    KEYDEF(RP, "[", "{", 1),
    KEYDEF(RP, "]", "}", 1),
    KEYDEF(RP, "\\", "|", 3),
    {0},
    KEYDEF(LP, "caps", "", 4),
    KEYDEF(LP, "A", "", 1),
    KEYDEF(LR, "S", "", 1),
    KEYDEF(LM, "D", "", 1),
    KEYDEF(LI, "F", "", 1),
    KEYDEF(LI, "G", "", 1),
    KEYDEF(RI, "H", "", 1),
    KEYDEF(RI, "J", "", 1),
    KEYDEF(RM, "K", "", 1),
    KEYDEF(RR, "L", "", 1),
    KEYDEF(RP, ";", ":", 1),
    KEYDEF(RP, "'", "\"", 1),
    KEYDEF(RP, "enter", "", 5),
    {0},
    KEYDEF(LP, "shift", "", 5),
    KEYDEF(LR, "Z", "", 1),
    KEYDEF(LM, "X", "", 1),
    KEYDEF(LI, "C", "", 1),
    KEYDEF(LI, "V", "", 1),
    KEYDEF(LI, "B", "", 1),
    KEYDEF(RI, "N", "", 1),
    KEYDEF(RI, "M", "", 1),
    KEYDEF(RM, ",", "<", 1),
    KEYDEF(RR, ".", ">", 1),
    KEYDEF(RP, "/", "?", 1),
    KEYDEF(RP, "shift", "", 7),
    {0},
    KEYDEF(LP, "ctrl", "", 4),
    KEYDEF(LP, "alt", "", 3),
    KEYDEF(T, " ", "", 22),
    KEYDEF(RP, "alt", "", 3),
    KEYDEF(RP, "ctrl", "", 4),
    {0},
    {0},
};

int main(int argc, const char** argv)
{
  /* ~  !  @  #  $  %  ^  &  *  (  )  _  +
   * `  1  2  3  4  5  6  7  8  9  0  -  =  bksp
   *                                    {  }  |
   * tab  q  w  e  r  t  y  u  i  o  p  [  ]  \
   *                                  :  '
   * caps  a  s  d  f  g  h  j  k  l  ;  '  enter
   *                             <  >  ?
   * shift  z  x  c  v  b  n  m  ,  .  /  shift
   *
   * ctrl  alt                          alt  ctrl */
  const key *ptr = keyboard, *save;
  int subrow;
  (void)(argc), (void)(argv);
  while (ptr->top) {
    save = ptr;
    for (subrow = 0; subrow < 2; subrow++) {
      ptr = save;
      while (ptr->top) {
        const char* label = subrow ? ptr->bot : ptr->top;
        unsigned int width = strlen(label), i, j;
        printf("\x1b[7m\x1b[38;5;%im\xe2\x96\x8f", colors[ptr->finger]);
        for (i = 0; i < (ptr->width - width) / 2; i++)
          printf(" ");
        printf("%s", label);
        for (j = 0; j < (ptr->width - width) - i; j++)
          printf(" ");
        printf("\xe2\x96\x95\x1b[0m");
        ptr++;
      }
      printf("\n");
    }
    ptr++;
  }
  return 0;
}
