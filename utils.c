#include <stdlib.h>
#include <string.h>
#include "utils.h"

char *substring(char *s, int start, int end)
{
    int i = 0;
    int length = end - start;
    char *ret = malloc(sizeof(char) * (length + 1));
    while (i < length) {
        ret[i] = s[i + start];
        i++;
    }
    ret[i] = '\0';
    return ret;
}

char *extract_string(char *s)
{
    int start = 0;
    int end = strlen(s);
    if (s[1] == '"') {
        start = 3;
        end = end - 3;
    } else if (s[1] == '\'') {
        start = 1;
        end = end - 1;
    } else {
        start = 1;
        end = end - 1;
    }
    return substring(s, start, end);
}

char *strip_underscores(char *s)
{
    int length = strlen(s);
    char *temp = malloc(sizeof(char) * (length + 1));
    int temp_index = 0;
    for (int i = 0; i < length; i++) {
        if (s[i] == '_') {
            continue;
        }
        temp[temp_index++] = s[i];
    }
    temp[temp_index] = '\0';
    return temp;
}

int extract_int(char *s)
{
    char *stripped = strip_underscores(s);
    int ret = strtoul(stripped, NULL, 0);
    free(stripped);
    return ret;
}

double extract_float(char *s)
{
    char *stripped = strip_underscores(s);
    double ret = strtod(stripped, NULL);
    free(stripped);
    return ret;
}
