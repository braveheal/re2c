/* Generated by re2c */
#line 1 "conditions/condtype.c--type-header(condtype.h).re"
int main ()
{
	YYCONDTYPE cond;
	char * YYCURSOR;
#define YYGETCONDITION() cond

#line 10 "conditions/condtype.c--type-header(condtype.h).c"
{
	unsigned char yych;
	switch (YYGETCONDITION()) {
	case yyca:
		goto yyc_a;
	case yycb:
		goto yyc_b;
	}
/* *********************************** */
yyc_a:
	yych = *YYCURSOR;
	switch (yych) {
	case 'a':	goto yy3;
	default:	goto yy2;
	}
yy2:
yy3:
	++YYCURSOR;
#line 9 "conditions/condtype.c--type-header(condtype.h).re"
	{}
#line 31 "conditions/condtype.c--type-header(condtype.h).c"
/* *********************************** */
yyc_b:
	yych = *YYCURSOR;
	switch (yych) {
	case 'b':	goto yy8;
	default:	goto yy7;
	}
yy7:
yy8:
	++YYCURSOR;
#line 10 "conditions/condtype.c--type-header(condtype.h).re"
	{}
#line 44 "conditions/condtype.c--type-header(condtype.h).c"
}
#line 11 "conditions/condtype.c--type-header(condtype.h).re"

	return 0;
}
/* Generated by re2c */

enum YYCONDTYPE {
	yyca,
	yycb,
};
conditions/condtype.c--type-header(condtype.h).re:11:2: warning: control flow in condition 'a' is undefined for strings that match '[\x0-\x60\x62-\xFF]', use default rule '*' [-Wundefined-control-flow]
conditions/condtype.c--type-header(condtype.h).re:11:2: warning: control flow in condition 'b' is undefined for strings that match '[\x0-\x61\x63-\xFF]', use default rule '*' [-Wundefined-control-flow]
