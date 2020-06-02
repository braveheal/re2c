/* Generated by re2c */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static const size_t SIZE = 4096;

struct input_t {
    uint16_t buf[SIZE + 1];
    uint16_t *lim;
    uint16_t *cur;
    uint16_t *mar;
    uint16_t *tok;
    bool eof;

    input_t()
        : buf()
        , lim(buf + SIZE)
        , cur(lim)
        , mar(lim)
        , tok(lim)
        , eof(false)
    {
        fill();
    }
    int fill()
    {
        if (eof) {
            return 1;
        }
        const size_t free = tok - buf;
        if (free < 1) {
            return 2;
        }
        memmove(buf, tok, lim - tok);
        lim -= free;
        cur -= free;
        mar -= free;
        tok -= free;
        lim += fread(lim, sizeof(uint16_t), free, stdin);
        lim[0] = 65535;
        if (lim < buf + SIZE) {
            eof = true;
        }
        return 0;
    }
};

static bool lex(input_t & in, unsigned int &count)
{
    for (count = 0;;) {
        in.tok = in.cur;
        
{
	uint16_t yych;
yyFillLabel0:
	yych = *in.cur;
	if (yych == '\n') goto yy5;
	if (yych >= 0xFFFF) {
		if (in.lim <= in.cur) {
			if (in.fill() == 0) goto yyFillLabel0;
			goto yyeofrule1;
		}
		goto yy2;
	}
yy2:
	++in.cur;
yyFillLabel1:
	yych = *in.cur;
	if (yych == '\n') goto yy4;
	if (yych <= 0xFFFE) goto yy2;
	if (in.lim <= in.cur) {
		if (in.fill() == 0) goto yyFillLabel1;
		goto yy4;
	}
	goto yy2;
yy4:
	{ ++count; continue; }
yy5:
	++in.cur;
yyFillLabel2:
	yych = *in.cur;
	if (yych == '\n') goto yy5;
	if (yych >= 0xFFFF) {
		if (in.lim <= in.cur) {
			if (in.fill() == 0) goto yyFillLabel2;
		}
		goto yy7;
	}
yy7:
	{ continue; }
yyeofrule1:
	{ return true; }
}

    }
}

int main()
{
    input_t in;
    unsigned int count;
    if (lex(in, count)) {
        printf("glorious %u numbers!\n", count);
    } else {
        printf("error\n");
    }

    return 0;
}
