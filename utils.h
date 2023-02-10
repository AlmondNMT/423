#define SCAN_ERROR -3
#define PANIC -4

char *substring(char *s, int start, int end);
int extract_string(char *dest, char *s);
char *strip_underscores(char *s);
int extract_int(char *s);
double extract_float(char *s);
void truncate_str(char *dest, char *src, int level);
int get_quote_count(char *text, int len);
int panic(char *errmsg);
int deescape(char *dest, char *s);
