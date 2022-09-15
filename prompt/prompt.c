#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SEG_SEP "\xee\x82\xb0"

int last_seg_bg = -1;

void put_fg(int fg) {
    printf("\\[\x1b[38;5;%im\\]", fg);
}

void put_bg(int bg) {
    printf("\\[\x1b[48;5;%im\\]", bg);
}

void put_sep(void) {
    printf(SEG_SEP);
}

void clear(void) {
    printf("\\[\x1b[0m\\]");
}

void seg_begin() {
    clear();
}

/* Output segment with foreground and background. */
void seg(int fg, int bg) {
    if (last_seg_bg != -1) {
        put_fg(last_seg_bg);
        put_bg(bg);
        put_sep();
    }
    put_fg(fg);
    put_bg(bg);
    last_seg_bg = bg;
}

void seg_end(void) {
    clear();
    put_fg(last_seg_bg);
    put_sep();
    clear();
    printf(" ");
    fflush(stdout);
}

void print_login_hostname() {
    char* buf;
    char* buf2;
    buf = malloc(80);
    buf2 = malloc(160);
    *buf = '\0';
    gethostname(buf, 80);
    getlogin_r(buf2, 160);
    {
        char* c = buf;
        while (*c) {
            if (*c == '.') {
                *c = '\0';
            }
            c++;
        }
    }
    printf(" %s@%s ", buf2, buf);
    free(buf);
    free(buf2);
}

void print_wd() {
    char* buf = malloc(PATH_MAX);
    char* home = getenv("HOME");
    char* wd = getcwd(buf, PATH_MAX);
    printf(" ");
    if (strncmp(home, wd, strlen(home)) == 0) {
        printf("~");
        wd += strlen(home);
    }
    printf("%s ", wd);
}

void print_exit(int exit_code) {
    printf(" %i ", exit_code);
}


void seg_user_color() {
    char* color = getenv("PROMPT_USER_COLORS");
    int fg, bg;
    if (getuid() == 0) {
        fg = 15;
        bg = 229;
    } else if (color) {
        int ret_color;
        sscanf(color, "%i, %i", &fg, &bg);
    } else {
        fg = 15;
        bg = 99;
    }
    seg(fg, bg);
}

void print_hash() {
    if (getuid() == 0) {
        printf(" # ");
    } else {
        printf(" $ ");
    }
}

int main(int argc, char** argv) {
    int exit_code;
    sscanf(argv[1], "%i", &exit_code);
    seg_begin();
    seg(15, 236);
    print_login_hostname();
    seg(15, 234);
    print_wd();
    if (exit_code != 0) {
        seg(15, 9);
        print_exit(exit_code);
    }
    seg_user_color();
    print_hash();
    seg_end();
    return 0;
}
