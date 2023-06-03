#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define APARSE_IMPLEMENTATION
#include "aparse.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SZ 16384

typedef unsigned char u8;
typedef unsigned int u32;

u32 make_color(u32 r, u32 g, u32 b, u32 a)
{
  return (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) |
         ((a & 0xFF) << 24);
}

u32 pal_rgba(u32 i) { return i; }

u32 pal_xt256(u32 i)
{
  i = i & 0xFF;
  if (i < 16) {
    int r = !!(i & 1), g = !!(i & 2), b = !!(i & 4), l = 1 - !!(i & 8);
    return make_color((r * 0xFF >> l), (g * 0xFF) >> l, (b * 0xFF) >> l, 0xFF);
  } else if (i < 232 && (i -= 16)) {
    int r = i / 36, g = ((i % 36) / 6), b = i % 6;
    r *= 0xFF * 2, g *= 0xFF * 2, b *= 0xFF * 2;
    return make_color(r / 12, g / 12, b / 12, 0xFF);
  } else {
    int l = ((i - 232) * 10) + 8;
    return make_color(l, l, l, 0xFF);
  }
}

u32 pal_bin(u32 i) { return 0xFFFFFFFF * !!i; }

int main(int argc, const char* const* argv)
{
  u32* buf = NULL;
  u8* in_buf = malloc(CHUNK_SZ);
  size_t nlines = 0;
  int w = 16;
  size_t x = 0, y = 0;
  FILE* f = stdin;
  int bw = 8;
  u32 acc = 0;
  int acc_bits = 0;
  int leftover = 8;
  const char* palette = "xt256";
  const char* out_name = "bviz.png";
  u32 (*pal_fn)(u32);
  aparse_state ap;
  assert(in_buf);
  aparse_init(&ap);
  aparse_add_opt(&ap, 'w', NULL);
  aparse_arg_type_int(&ap, &w);
  aparse_arg_help(&ap, "width of output file");

  aparse_add_opt(&ap, 'b', NULL);
  aparse_arg_type_int(&ap, &bw);
  aparse_arg_help(&ap, "input bits per output pixel [1, 32]");

  aparse_add_opt(&ap, 'p', "palette");
  aparse_arg_type_str(&ap, &palette, NULL);
  aparse_arg_help(&ap, "palette function, valid options are: xt256, rgba, bin");

  aparse_add_opt(&ap, 'o', NULL);
  aparse_arg_type_str(&ap, &out_name, NULL);
  aparse_arg_help(&ap, "output filename");

  aparse_add_opt(&ap, 'h', "help");
  aparse_arg_type_help(&ap);
  aparse_arg_help(&ap, "show this help text and exit");

  if (aparse_parse(&ap, argc, argv)) {
    return EXIT_FAILURE;
  }

  assert(bw >= 1 && bw <= 32);
  assert(w > 0);
  if (!strcmp(palette, "xt256")) {
    pal_fn = &pal_xt256;
  } else if (!strcmp(palette, "rgba")) {
    pal_fn = &pal_rgba;
  } else if (!strcmp(palette, "bin")) {
    pal_fn = &pal_bin;
  } else {
    printf("invalid palette: %s\n", palette);
    return EXIT_FAILURE;
  }

  while (!feof(f)) {
    size_t in = fread((void*)in_buf, 1, CHUNK_SZ, f);
    size_t i;
    u32 mask;
    for (i = 0; i < in;) {
      int out_shift;
      if (y >= nlines) {
        size_t new_lines = nlines ? nlines * 2 : 4;
        buf = (u32*)realloc(buf, new_lines * w * sizeof(u32));
        memset(buf + nlines * w * 4, 0, (new_lines - nlines) * w * sizeof(u32));
        nlines = new_lines;
      }
      assert(leftover);
      out_shift = (bw - acc_bits) <= leftover ? (bw - acc_bits) : leftover;
      mask = (1 << out_shift) - 1;
      acc |= (in_buf[i] & mask) << acc_bits;
      acc_bits += out_shift;
      in_buf[i] >>= out_shift;
      leftover -= out_shift;
      if (!leftover)
        i++, leftover = 8;
      if (acc_bits == bw) {
        buf[y * w + x++] = pal_fn(acc);
        acc = 0;
        acc_bits = 0;
        if (x == (size_t)w) {
          x = 0;
          y++;
        }
      }
    }
  }
  if (y)
    stbi_write_png(out_name, w, y + 1, 4, buf, 0);
  aparse_destroy(&ap);
  return 0;
}
