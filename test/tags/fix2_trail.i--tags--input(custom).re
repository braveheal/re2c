#include <stddef.h>
#include <stdio.h>

static void lex(const char *s)
{
#define YYPEEK()           *s
#define YYSKIP()           ++s
#define YYBACKUP()         marker = s
#define YYRESTORE()        s = marker
#define YYSTAGP(t)         t = s
#define YYSTAGN(t)         t = NULL
#define YYRESTORETAG(t)    s = t
#define YYSHIFT(o)         s += o
#define YYSHIFTSTAG(t, o)  t += o
    const char *marker, *p0, *p1, *p2, *p3;
    /*!stags:re2c format = "const char *@@;"; */
    /*!re2c
        re2c:define:YYCTYPE = char;
        re2c:yyfill:enable = 0;

        * { printf("error\n"); return; }
        @p0 "0"
        @p1 "11"
        @p2 "222"
        @p3 "3333"
        / "44444" {
            printf("'%.*s', '%.*s', '%.*s', '%.*s', '%s'\n",
                p1 - p0, p0,
                p2 - p1, p1,
                p3 - p2, p2,
                s - p3, p3,
                s);
                return;
        }
    */
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        lex(argv[i]);
    }
    return 0;
}
