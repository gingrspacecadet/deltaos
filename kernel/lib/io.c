#include <drivers/console.h>
#include <drivers/serial.h>
#include <stdarg.h>

enum output_mode {
    SERIAL,
    CONSOLE,
};

static enum output_mode mode = SERIAL;

void puts(const char *s) {
    switch (mode) {
        case SERIAL:
            serial_write(s);
            return;
        case CONSOLE:
            con_print(s);
            return;
        default: return;
    }
}

void putc(const char c) {
    switch (mode) {
        case SERIAL:
            serial_write_char(c);
            return;
        case CONSOLE:
            con_putc(c);
            return;
        default: return;
    }
}

void printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[1024];
    char *out = buffer;
    const char *p = format;

    while (*p) {
        if (*p == '%') {
            p++;
            if (*p == 's') {
                const char *str = va_arg(args, const char *);
                while (*str) {
                    if (*str == '\\' && *(str + 1)) {
                        str++;
                        switch (*str) {
                            case 'n':  *out++ = '\n'; break;
                            case 't':  *out++ = '\t'; break;
                            case '\\': *out++ = '\\'; break;
                            default:
                                *out++ = '\\';
                                *out++ = *str;
                                break;
                        }
                    } else {
                        *out++ = *str;
                    }
                    str++;
                }
            }
            else if (*p == 'c') {
                *out++ = (char)va_arg(args, int);
            }
            else if (*p == 'd') {
                int num = va_arg(args, int);
                char tmp[12];
                int len = 0;

                if (num < 0) {
                    *out++ = '-';
                    /* handle INT_MIN safely */
                    unsigned int u = (unsigned int)(-(num + 1)) + 1;
                    num = (int)u;
                }

                if (num == 0) {
                    tmp[len++] = '0';
                } else {
                    unsigned int u = (unsigned int)num;
                    while (u) {
                        tmp[len++] = (char)('0' + (u % 10));
                        u /= 10;
                    }
                }
                /* reverse into buffer */
                for (int i = len - 1; i >= 0; i--) {
                    *out++ = tmp[i];
                }
            } else if (*p == 'x') {
                unsigned int num = va_arg(args, unsigned int);
                char tmp[9];
                int len = 0;

                if (num == 0) {
                    tmp[len++] = '0';
                } else {
                    while (num) {
                        int digit = num & 0xF;
                        if (digit < 10) {
                            tmp[len++] = (char)('0' + digit);
                        } else {
                            tmp[len++] = (char)('a' + (digit - 10));
                        }
                        num >>= 4;
                    }
                }
                /* reverse into buffer */
                for (int i = len - 1; i >= 0; i--) {
                    *out++ = tmp[i];
                }
            }
            else if (*p == '%') {
                *out++ = '%';
            }
            else {
                /* unknown specifier → literal %}
                */
                *out++ = '%';
                *out++ = *p;
            }
        }
        else {
            *out++ = *p;
        }
        p++;
    }

    /* terminate and write once */
    *out = '\0';
    va_end(args);

    puts(buffer);
}

void set_outmode(enum output_mode m) {
    mode = m;
}