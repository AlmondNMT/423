#define SCAN_ERROR -3
#define PANIC -4
#define BRACKET_ERROR -5

char *substring(char *s, int start, int end);
int extract_string(char *dest, char *s);
char *strip_underscores(char *s);
int extract_int(char *s);
double extract_float(char *s);
void truncate_str(char *dest, char *src, int level);
int count_dedents(int *top, int dentstack[], int indent_level, int rows);
int get_quote_count(char *text, int len);
int panic(char *errmsg);
int deescape(char *dest, char *s);
int is_enclosed(int p_nesting, int sq_nesting, int cb_nesting, int rows);
