/* Generated by re2c */
#include <assert.h>
typedef enum { OK, FAIL } Result;




Result lex(const char *YYCURSOR)
{
    
{
	char yych;
	yych = *YYCURSOR;
	switch (yych) {
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy4;
	default:	goto yy2;
	}
yy2:
	++YYCURSOR;
	{ return FAIL; }
yy4:
	yych = *++YYCURSOR;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy4;
	default:	goto yy6;
	}
yy6:
	{ return OK; }
}

}

int main()
{
    assert(lex("123") == OK);
    return 0;
}
