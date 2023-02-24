#define PATHMAX 1000      // Maximum length of filepath names
#define SCAN_ERROR -3
#define PANIC -4
#define BRACKET_ERROR -5
#define EOF_ERROR -6

void check_alloc(void *obj, char *msg);
int count_dedents(int *top, int dentstack[], int indent_level, int rows);
double extract_float(char *s);
int deescape(char *dest, char *s);
int extract_int(char *s);
int extract_string(char *dest, char *s);
int get_quote_count(char *text, int len);
int is_enclosed(int p_nesting, int sq_nesting, int cb_nesting);
int panic(char *errmsg);
char *strip_underscores(char *s);
char *substring(char *s, int start, int end);
void truncate_str(char *dest, char *src, int level);
void err_t_lookahead(int yychar);
void err_lookahead(int yychar, int count, const char *msg, ...);
