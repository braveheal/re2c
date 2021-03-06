// Code generated by re2c, DO NOT EDIT.
//line "go/eof/02_bounds_checking.--lang(go).re":1
//go:generate re2go "$GOFILE" -o "$OUTPUT"
package main

import (
	"strings"
	"testing"
)

var YYMAXFILL int = 1


// Expects YYMAXFILL-padded string.
func lex(str string) int {
	var cursor int
	limit := len(str)
	count := 0
loop:
	
//line "go/eof/02_bounds_checking.--lang(go).go":22
{
	var yych byte
	if (limit - cursor < 1) {
		return -1
	}
	yych = str[cursor]
	switch (yych) {
	case 0x00:
		goto yy2
	case ' ':
		goto yy6
	case '\'':
		goto yy9
	default:
		goto yy4
	}
yy2:
	cursor += 1
//line "go/eof/02_bounds_checking.--lang(go).re":25
	{ return count }
//line "go/eof/02_bounds_checking.--lang(go).go":43
yy4:
	cursor += 1
//line "go/eof/02_bounds_checking.--lang(go).re":24
	{ return -1 }
//line "go/eof/02_bounds_checking.--lang(go).go":48
yy6:
	cursor += 1
	if (limit - cursor < 1) {
		return -1
	}
	yych = str[cursor]
	switch (yych) {
	case ' ':
		goto yy6
	default:
		goto yy8
	}
yy8:
//line "go/eof/02_bounds_checking.--lang(go).re":27
	{ goto loop }
//line "go/eof/02_bounds_checking.--lang(go).go":64
yy9:
	cursor += 1
	if (limit - cursor < 1) {
		return -1
	}
	yych = str[cursor]
	switch (yych) {
	case '\'':
		goto yy11
	case '\\':
		goto yy13
	default:
		goto yy9
	}
yy11:
	cursor += 1
//line "go/eof/02_bounds_checking.--lang(go).re":26
	{ count += 1; goto loop }
//line "go/eof/02_bounds_checking.--lang(go).go":83
yy13:
	cursor += 1
	if (limit - cursor < 1) {
		return -1
	}
	yych = str[cursor]
	goto yy9
}
//line "go/eof/02_bounds_checking.--lang(go).re":28

}

// Pad string with YYMAXFILL zeroes at the end.
func pad(str string) string {
	return str + strings.Repeat("\000", YYMAXFILL)
}

func TestLex(t *testing.T) {
	var tests = []struct {
		res int
		str string
	}{
		{0, ""},
		{3, "'qu\000tes' 'are' 'fine: \\'' "},
		{-1, "'unterminated\\'"},
	}

	for _, x := range tests {
		t.Run(x.str, func(t *testing.T) {
			res := lex(pad(x.str))
			if res != x.res {
				t.Errorf("got %d, want %d", res, x.res)
			}
		})
	}
}
