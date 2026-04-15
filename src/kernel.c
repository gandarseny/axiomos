// --- Multiboot header ---
__attribute__((section(".multiboot")))
unsigned int multiboot_header[] = {
    0x1BADB002,
    0x0,
    -(0x1BADB002 + 0x0)
};

#define VGA ((volatile char*)0xB8000)
int cursor = 0;

// --- strcmp (MUST BE FIRST) ---
int strcmp(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++; b++;
    }
    return *a == *b;
}

// --- COLOR SYSTEM ---
unsigned char color = 0x0F;

unsigned char parse_color(const char* name) {
    if (strcmp(name, "black")) return 0x0;
    if (strcmp(name, "blue")) return 0x1;
    if (strcmp(name, "green")) return 0x2;
    if (strcmp(name, "red")) return 0x4;
    if (strcmp(name, "yellow")) return 0xE;
    if (strcmp(name, "white")) return 0xF;
    return 0xF;
}

void apply_colors();

// --- RAM filesystem ---
#define MAX_FILES 16
#define MAX_NAME 16
#define MAX_CONTENT 128

struct file {
    char name[MAX_NAME];
    char content[MAX_CONTENT];
};

struct file fs[MAX_FILES];
int fs_count = 0;

// --- VGA ---
void putc(char c) {
    if (c == '\n') {
        cursor = (cursor / 160 + 1) * 160;
        return;
    }
    VGA[cursor++] = c;
    VGA[cursor++] = color;
}

void puts(const char* s) {
    while (*s) putc(*s++);
}

void clearx() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        VGA[i] = ' ';
        VGA[i + 1] = color;
    }
    cursor = 0;
}

// --- keyboard ---
unsigned char inb(unsigned short port) {
    unsigned char r;
    asm volatile ("inb %1, %0" : "=a"(r) : "Nd"(port));
    return r;
}

char getkey() {
    unsigned char sc;
    while (1) {
        if (!(inb(0x64) & 1)) continue;
        sc = inb(0x60);
        if (sc & 0x80) continue;

        const char keymap[] =
        "\0\0""1234567890-=\0\0"
        "qwertyuiop[]\n\0"
        "asdfghjkl;'`\0"
        "\\zxcvbnm,./\0";

        if (sc < sizeof(keymap)) {
            char c = keymap[sc];
            if (c) return c;
        }
    }
}

// --- atoi ---
int atoi(const char* s) {
    int r = 0;
    while (*s) {
        r = r * 10 + (*s - '0');
        s++;
    }
    return r;
}

// --- APPLY COLORS ---
void apply_colors() {
    for (int i = 0; i < fs_count; i++) {
        if (strcmp(fs[i].name, "reg.colors")) {

            char *c = fs[i].content;
            char *bg = c;

            while (*bg && *bg != ',') bg++;

            if (*bg) {
                *bg = 0;
                bg++;
            }

            unsigned char fg = parse_color(c);
            unsigned char b = parse_color(bg);

            color = (b << 4) | fg;
            return;
        }
    }
}

// =======================
//      CALCULATOR
// =======================
void calculate() {
    clearx();
    puts("CALCULATOR\n");
    puts("p=+  m=*  -=-  /=/\n");
    puts("type 'exit' to leave\n\n");
    puts("calc>\n");

    char buf[32];
    int i = 0;

    while (1) {
        char c = getkey();

        if (c == '\n') {
            buf[i] = 0;
            puts("\n");

            if (strcmp(buf, "exit")) {
                puts("bye\n");
                return;
            }

            if (strcmp(buf, "clearx")) {
                clearx();
                puts("calc>\n");
                i = 0;
                continue;
            }

            int a = 0, b = 0;
            char op = 0;
            int pos = 0;

            while (buf[pos]) {
                if (buf[pos]=='p'||buf[pos]=='m'||buf[pos]=='-'||buf[pos]=='/') {
                    op = buf[pos];
                    break;
                }
                pos++;
            }

            if (!op) {
                puts("bad\n");
                i = 0;
                continue;
            }

            buf[pos] = 0;
            a = atoi(buf);
            b = atoi(buf + pos + 1);

            int res = 0;

            if (op == 'p') res = a + b;
            else if (op == 'm') res = a * b;
            else if (op == '-') res = a - b;
            else if (op == '/') {
                if (b == 0) {
                    puts("div0\n");
                    i = 0;
                    continue;
                }
                res = a / b;
            }

            char out[16];
            int k = 0;

            if (res == 0) out[k++]='0';
            else {
                char tmp[16];
                int t=0;
                while(res>0){
                    tmp[t++] = (res%10)+'0';
                    res/=10;
                }
                for(int j=t-1;j>=0;j--) out[k++]=tmp[j];
            }

            out[k]=0;
            puts(out);
            puts("\n");

            i = 0;
            puts("calc>\n");
        }
        else {
            buf[i++] = c;
            putc(c);
        }
    }
}

// =======================
//         SHELL
// =======================
void shell() {
    char buf[128];
    int i = 0;

    puts("\n>SPACE> ");

    while (1) {
        char c = getkey();

        if (c == '\n') {
            buf[i] = 0;
            puts("\n");

            char *args[8];
            int argc = 0;
            char *p = buf;

            while (*p && argc < 8) {
                args[argc++] = p;
                while (*p && *p != '=') p++;
                if (*p) *p++ = 0;
            }

            if (argc == 0) {
                puts(">SPACE> ");
                i = 0;
                continue;
            }

            if (strcmp(args[0], "pulse")) {
                puts("system beating\n");
            }
            else if (strcmp(args[0], "about")) {
                puts("AxiomOS Adrift\n");
            }
            else if (strcmp(args[0], "glyphs")) {
                puts("pulse clearx haltx about flst write see calculate\n");
            }
            else if (strcmp(args[0], "clearx")) {
                clearx();
            }
            else if (strcmp(args[0], "haltx")) {
                puts("core stopped\n");
                asm volatile ("cli; hlt");
            }
            else if (strcmp(args[0], "flst")) {
                for (int j = 0; j < fs_count; j++) {
                    puts(fs[j].name);
                    puts("\n");
                }
            }
            else if (strcmp(args[0], "write") && argc >= 3) {
                int j = fs_count++;

                int k = 0;
                char *n = args[1];
                while (*n && k < MAX_NAME) fs[j].name[k++] = *n++;
                fs[j].name[k] = 0;

                k = 0;
                char *s = args[2];
                while (*s && k < MAX_CONTENT) {
                    fs[j].content[k++] = (*s == '-' ? '\n' : *s);
                    s++;
                }
                fs[j].content[k] = 0;

                puts("written\n");

                apply_colors(); // apply color config
            }
            else if (strcmp(args[0], "see") && argc >= 2) {
                for (int j = 0; j < fs_count; j++) {
                    if (strcmp(fs[j].name, args[1])) {
                        puts(fs[j].content);
                        puts("\n");
                        break;
                    }
                }
            }
            else if (strcmp(args[0], "calculate")) {
                calculate();
            }
            else {
                puts("unknown glyph\n");
            }

            i = 0;
            puts(">SPACE> ");
        }
        else {
            buf[i++] = c;
            putc(c);
        }
    }
}

// =======================
//        KERNEL
// =======================
void kernel_main() {
    clearx();

    // create calculate.axp
    char *name = "calculate.axp";
    char *content = "built-in calculator";

    int k = 0;
    while (name[k]) {
        fs[fs_count].name[k] = name[k];
        k++;
    }
    fs[fs_count].name[k] = 0;

    k = 0;
    while (content[k]) {
        fs[fs_count].content[k] = content[k];
        k++;
    }
    fs[fs_count].content[k] = 0;

    fs_count++;

    puts("AxiomOS booted [SPACE]>\n");
    shell();
}