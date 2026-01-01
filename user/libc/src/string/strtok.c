#include <string.h>

char *strtok(char *str, const char *delim) {
    static char *next;
    if (str) next = str;
    if (!next) return NULL;

    char *start = next;
    while (*next && !strchr(delim, *next)) next++;
    if (*next) {
        *next = '\0';
        next++;
    } else {
        next = NULL;
    }
    return start;
}