#include "src/util/c99_stdint.h"
#include <stddef.h>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "src/code/bitmap.h"
#include "src/code/emit.h"
#include "src/code/go.h"
#include "src/code/input_api.h"
#include "src/code/label.h"
#include "src/code/output.h"
#include "src/adfa/action.h"
#include "src/adfa/adfa.h"
#include "src/skeleton/skeleton.h"
#include "src/util/counter.h"
#include "src/util/string_utils.h"

namespace re2c
{

static void emit_state(OutputFile & o, uint32_t ind, const State * s, bool used_label);

void emit_state (OutputFile & o, uint32_t ind, const State * s, bool used_label)
{
	const opt_t *opts = o.block().opts;
	if (used_label)
	{
		o.wstring(opts->labelPrefix).wlabel(s->label).ws(":\n");
	}
	if (opts->dFlag && (s->action.type != Action::INITIAL))
	{
		o.wind(ind).wstring(opts->yydebug).ws("(").wlabel(s->label).ws(", ").wstring(output_expr_peek(opts)).ws(");\n");
	}
}

void DFA::count_used_labels (std::set<label_t> & used, label_t start,
	label_t initial, bool force_start, bool fFlag) const
{
	// In '-f' mode, default state is always state 0
	if (fFlag)
	{
		used.insert (label_t::first ());
	}
	if (force_start)
	{
		used.insert (start);
	}
	for (State * s = head; s; s = s->next)
	{
		s->go.used_labels (used);
	}
	for (uint32_t i = 0; i < accepts.size (); ++i)
	{
		used.insert (accepts[i].first->label);
	}
	// must go last: it needs the set of used labels
	if (used.count (head->label))
	{
		used.insert (initial);
	}
}

void DFA::emit_body(OutputFile &o, uint32_t& ind,
	const std::set<label_t> &used_labels, label_t initial) const
{
	code_lines_t code;
	gen_settags(code, *this, tags0, o.block().opts);
	for (size_t i = 0; i < code.size(); ++i) {
		o.wind(ind).wstring(code[i]);
	}

	// If DFA has transitions to initial state, then initial state
	// has a piece of code that advances input position. Wee must
	// skip it when entering DFA.
	if (used_labels.count(head->label)) {
		o.wind(ind).ws("goto ").wstring(o.block().opts->labelPrefix)
			.wlabel(initial).ws(";\n");
	}

	for (State * s = head; s; s = s->next) {
		emit_state(o, ind, s, used_labels.count(s->label));
		emit_action(o, ind, *this, s, used_labels);
		s->go.emit(o, ind, *this);
	}
}

void DFA::emit_dot(OutputFile &o, bool last_cond) const
{
	const opt_t *opts = o.block().opts;
	if (!opts->cFlag || !o.cond_goto) {
		o.ws("digraph re2c {\n");
	}
	o.wdelay_cond_goto(0);
	if (opts->cFlag) {
		o.wstring(cond).ws(" -> ").wlabel(head->label).ws("\n");
	}
	for (State *s = head; s; s = s->next) {
		if (s->action.type == Action::ACCEPT) {
			const accept_t &accepts = *s->action.info.accepts;
			for (uint32_t i = 0; i < accepts.size(); ++i) {
				o.wlabel(s->label).ws(" -> ")
					.wlabel(accepts[i].first->label)
					.ws(" [label=\"yyaccept=")
					.wu32(i).ws("\"]").ws("\n");
			}
		} else if (s->action.type == Action::RULE) {
			const Code *code = rules[s->action.info.rule].code;
			if (!code->autogen) {
				o.wlabel(s->label).ws(" [label=\"")
					.wstring(code->fname)
					.ws(":").wu32(code->fline)
					.ws("\"]").ws("\n");
			}
		}
		s->go.emit(o, 0, *this);
	}
	if (!opts->cFlag || last_cond) {
		o.ws("}\n");
	}
}

static void find_list_tags_cmd(const tcmd_t *p, bool *list)
{
	for (; p; p = p->next) {
		const tagver_t l = p->lhs, h = p->pred;
		if (h != TAGVER_ZERO) {
			list[l] = list[h] = true;
		}
	}
}

static void prop_list_tags_cmd(const tcmd_t *p, bool *list)
{
	for (; p; p = p->next) {
		const tagver_t l = p->lhs, r = p->rhs;
		if (tcmd_t::iscopy(r)) {
			if (list[l]) list[r] = true;
			if (list[r]) list[l] = true;
		}
	}
}

static void find_list_tags(const DFA &dfa, bool *list)
{
	for (State *s = dfa.head; s; s = s->next) {
		const Go &go = s->go;
		find_list_tags_cmd(dfa.tcpool[go.tags], list);
		for (uint32_t i = 0; i < go.nSpans; ++i) {
			find_list_tags_cmd(dfa.tcpool[go.span[i].tags], list);
		}
	}
	for (State *s = dfa.head; s; s = s->next) {
		const Go &go = s->go;
		prop_list_tags_cmd(dfa.tcpool[go.tags], list);
		for (uint32_t i = 0; i < go.nSpans; ++i) {
			prop_list_tags_cmd(dfa.tcpool[go.span[i].tags], list);
		}
	}
}

void DFA::emit(Output & output, uint32_t& ind, bool isLastCond, bool& bPrologBrace)
{
	OutputFile &o = output.source;
	OutputBlock &ob = o.block();
	const opt_t *opts = ob.opts;

	std::set<std::string> tagnames, tagvars, taglistnames, taglistvars;
	if (!oldstyle_ctxmarker) {
		for (size_t i = 0; i < tags.size(); ++i) {
			const Tag &tag = tags[i];
			if (history(tag)) {
				taglistvars.insert(*tag.name);
			} else if (tag.name) {
				tagvars.insert(*tag.name);
			}
		}
		bool *list = new bool[maxtagver + 1]();
		find_list_tags(*this, list);
		for (tagver_t v = 1; v <= maxtagver; ++v) {
			const std::string name = vartag_name(v, opts->tags_prefix);
			if (list[v]) {
				taglistnames.insert(name);
			} else {
				tagnames.insert(name);
			}
		}
		delete[] list;
		ob.tags.insert(tagnames.begin(), tagnames.end());
	}
	if (!cond.empty()) o.block().types.push_back(cond);

	bool bProlog = (!opts->cFlag || !o.cond_goto);

	// start_label points to the beginning of current re2c block
	// (prior to condition dispatch in '-c' mode)
	// it can forced by configuration 're2c:startlabel = <integer>;'
	label_t start_label = o.label_counter.next ();
	// initial_label points to the beginning of DFA
	// in '-c' mode this is NOT equal to start_label
	label_t initial_label = bProlog && opts->cFlag
		? o.label_counter.next ()
		: start_label;
	for (State * s = head; s; s = s->next)
	{
		s->label = o.label_counter.next ();
	}
	std::set<label_t> used_labels;
	count_used_labels (used_labels, start_label, initial_label,
		opts->startlabel_force && opts->startlabel.empty(), opts->fFlag);

	head->action.set_initial(initial_label);

	if (opts->target == TARGET_SKELETON) {
		if (output.skeletons.insert (name).second)
		{
			emit_start(o, max_fill, name, key_size, def_rule, need_backup,
				need_accept, oldstyle_ctxmarker, tagnames, tagvars,
				taglistnames, taglistvars, bitmaps);
			uint32_t i = 2;
			emit_body (o, i, used_labels, initial_label);
			emit_end(o, name, need_backup, oldstyle_ctxmarker, taglistnames);
		}
	} else if (opts->target == TARGET_DOT) {
		emit_dot(o, isLastCond);
	} else {
		// Generate prolog
		if (bProlog)
		{
			o.ws("\n").wdelay_line_info ();
			if ((!opts->fFlag && ob.used_yyaccept)
			||  (!opts->fFlag && opts->bEmitYYCh)
			||  (opts->bFlag && !opts->cFlag && !bitmaps.empty())
			||  (opts->cFlag && !o.cond_goto && opts->gFlag)
			||  (opts->fFlag && !o.state_goto && opts->gFlag)
			)
			{
				bPrologBrace = true;
				o.wind(ind++).ws("{\n");
			}
			else if (ind == 0)
			{
				ind = 1;
			}
			if (!opts->fFlag)
			{
				if (opts->bEmitYYCh)
				{
					o.wind(ind).wstring(opts->yyctype).ws(" ").wstring(opts->yych).ws(";\n");
				}
				o.wdelay_yyaccept_init (ind);
			}
			else
			{
				o.ws("\n");
			}
		}
		if (opts->bFlag && !opts->cFlag)
		{
			bitmaps.gen(o, ind);
		}
		if (bProlog)
		{
			o.wdelay_cond_table(ind);
			o.wdelay_state_goto (ind);
			if (opts->cFlag)
			{
				if (used_labels.count(start_label))
				{
					o.wstring(opts->labelPrefix).wlabel(start_label).ws(":\n");
				}
			}
			o.wuser_start_label ();
			o.wdelay_cond_goto(ind);
		}
		if (opts->cFlag && !cond.empty())
		{
			if (opts->condDivider.length())
			{
				std::string divider = opts->condDivider;
				strrreplace(divider, opts->condDividerParam, cond);
				o.wstring(divider).ws("\n");
			}
			o.wstring(opts->condPrefix).wstring(cond).ws(":\n");
		}
		if (opts->cFlag && opts->bFlag && !bitmaps.empty())
		{
			o.wind(ind++).ws("{\n");
			bitmaps.gen(o, ind);
		}
		// Generate code
		emit_body (o, ind, used_labels, initial_label);
		if (opts->cFlag && opts->bFlag && !bitmaps.empty())
		{
			o.wind(--ind).ws("}\n");
		}
		// Generate epilog
		if ((!opts->cFlag || isLastCond) && bPrologBrace)
		{
			o.wind(--ind).ws("}\n");
		}
	}
}

std::string vartag_name(tagver_t ver, const std::string &prefix)
{
	std::ostringstream s;
	s << prefix << ver;
	return s.str();
}

std::string vartag_expr(tagver_t ver, const std::string &prefix, const std::string &expression)
{
	const std::string s = vartag_name(ver, prefix);
	std::string e = expression;
	strrreplace(e, "@@", s);
	return e;
}

} // end namespace re2c