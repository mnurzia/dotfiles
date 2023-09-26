#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#define APARSE_IMPLEMENTATION
#include <aparse.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SZ 16384

typedef unsigned char u8;
typedef unsigned int u32;

u32 make_color(u32 r, u32 g, u32 b, u32 a) {
  return (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) |
         ((a & 0xFF) << 24);
}

u32 lsb_ext(u32 in, u32 iw, u32 ow) {
  return (in << (ow - iw)) | (((1 << (ow - iw)) - 1) * (in & 1));
}

u32 pal_rgba8888(u32 i) /* db: 32 */ { return i; }

u32 pal_rgb565(u32 i) /* db: 16 */
{
  int r = i & 0x1F, g = (i >> 5) & 0x3F, b = (i >> 11) & 0x1F;
  return make_color(lsb_ext(r, 5, 8), lsb_ext(g, 6, 8), lsb_ext(b, 5, 8), 0xFF);
}

u32 pal_gray8(u32 i) /* db: 8 */
{
  return make_color(i & 0xFF, i & 0xFF, i & 0xFF, 0xFF);
}

u32 pal_xt8(u32 i) /* db: 8 */
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

u32 pal_byteclass8(u32 i) /* db: 8 */
{
  if (!i)
    return make_color(0x00, 0x00, 0x00, 0xFF);
  else if (i < 0x20 && i != 0x09 && i != 0x0A && i != 0x0D)
    return make_color(0x00, 0xFF, 0x00, 0xFF);
  else if (i < 0x7F)
    return make_color(0x00, 0x00, 0xFF, 0xFF);
  else if (i < 0xFF)
    return make_color(0xFF, 0x00, 0x00, 0xFF);
  else
    return make_color(0xFF, 0xFF, 0xFF, 0xFF);
}

u32 pal_bin(u32 i) /* db: 1-32 */
{
  return make_color(0xFF, 0xFF, 0xFF, 0xFF) * !!i;
}

void write_func(void *context, void *data, int size) {
  FILE *f = (FILE *)context;
  fwrite(data, 1, size, f);
}

int main(int argc, const char *const *argv) {
  u32 *buf = NULL;
  u8 *in_buf = malloc(CHUNK_SZ);
  size_t nlines = 0;
  int w = 16;
  size_t x = 0, y = 0;
  FILE *f = stdin;
  FILE *fo = stdout;
  int bw = 8;
  u32 acc = 0;
  int acc_bits = 0;
  int leftover = 8;
  const char *palette = "xt8";
  const char *in_name = NULL;
  const char *out_name = "bviz.png";
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
  aparse_arg_help(&ap,
                  "palette function, valid options are: xt8, rgba8888, rgb565, "
                  "gray8, bin, "
                  "byteclass8");

  aparse_add_opt(&ap, 'i', NULL);
  aparse_arg_type_str(&ap, &in_name, NULL);

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
  if (!strcmp(palette, "xt8")) {
    pal_fn = &pal_xt8;
  } else if (!strcmp(palette, "rgba8888")) {
    pal_fn = &pal_rgba8888;
  } else if (!strcmp(palette, "rgb565")) {
    pal_fn = &pal_rgb565;
  } else if (!strcmp(palette, "gray8")) {
    pal_fn = &pal_gray8;
  } else if (!strcmp(palette, "bin")) {
    pal_fn = &pal_bin;
  } else if (!strcmp(palette, "byteclass8")) {
    pal_fn = &pal_byteclass8;
  } else {
    printf("invalid palette: %s\n", palette);
    return EXIT_FAILURE;
  }

  if (in_name)
    f = fopen(in_name, "r");

  while (!feof(f)) {
    size_t in = fread((void *)in_buf, 1, CHUNK_SZ, f);
    size_t i;
    u32 mask;
    for (i = 0; i < in;) {
      int out_shift;
      if (y >= nlines) {
        size_t new_lines = nlines ? nlines * 2 : 4;
        buf = (u32 *)realloc(buf, new_lines * w * sizeof(u32));
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
  if (out_name)
    fo = fopen(out_name, "w");
  if (y)
    stbi_write_png_to_func(&write_func, (void *)fo, w, y, 4, buf, 0);
  aparse_destroy(&ap);
  return 0;
}
