/* Generated by re2c */

{
	YYCTYPE yych;
	if (YYLESSTHAN (3)) YYFILL(3);
	yych = YYPEEK ();
	switch (yych) {
	case 'a':	goto yy4;
	default:	goto yy2;
	}
yy2:
	YYSKIP ();
	YYBACKUPCTX ();
	yych = YYPEEK ();
	goto yy6;
	YYRESTORECTX ();
	{}
yy4:
	YYSKIP ();
	YYBACKUPCTX ();
	switch ((yych = YYPEEK ())) {
	case 'a':	goto yy10;
	default:	goto yy8;
	}
	YYRESTORECTX ();
	{}
yy6:
	YYSKIP ();
	if (YYLESSTHAN (1)) YYFILL(1);
	yych = YYPEEK ();
	goto yy6;
yy8:
	YYSKIP ();
	if (YYLESSTHAN (1)) YYFILL(1);
	yych = YYPEEK ();
	goto yy8;
yy10:
	YYSKIP ();
	YYBACKUPCTX ();
	yych = YYPEEK ();
	goto yy8;
}

re2c: warning: line 2: unreachable rule  [-Wunreachable-rules]
re2c: warning: line 3: unreachable rule  [-Wunreachable-rules]
re2c: warning: line 4: unreachable rule (shadowed by rule at line 2) [-Wunreachable-rules]
