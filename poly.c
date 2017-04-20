#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLEN 2048

#define is_number(ch) \
	(((ch)<='9')&&((ch)>='0'))
#define is_word(ch) \
	(((ch)<='Z')&&((ch)>='A')||\
	 ((ch)<='z')&&((ch)>='a')||\
	 ((ch)=='_'))

#define LEX_ERROR        (-1)
#define TOK_WORD         (-2)
#define TOK_M_BRAK_O     (-3)
#define TOK_M_BRAK_C     (-4)
#define TOK_S_BRAK_O     (-5)
#define TOK_S_BRAK_C     (-6)
#define TOK_ADD          (-7)
#define TOK_SUB          (-8)
#define TOK_MUL          (-9)
#define TOK_POW          (-10)

#define MIN_INT          (-2147483648)
#define assert_parser_error(s) \
do {\
	puts("Syntax Error!");\
	if (*s) puts(s);\
	token_list->index = token_list->size;\
	return 0;\
} while(0)
#define assert_number_error(s) \
do {\
	puts("Syntax Error!");\
	if (*s) puts(s);\
	token_list->index = token_list->size;\
	return MIN_INT;\
} while (0)
#define empty() \
	(token_list->index >= token_list->size)
#define current() \
	(token_list->list[token_list->index])
#define next() \
	(token_list->index++)
#define force_get(ty, p, f) \
	ty p=(f)(token_list);\
	if (!p) return 0;
#define force_int(p, f) \
	int p=(f)(token_list);\
	if (p == MIN_INT) return MIN_INT;
#define force_number(p, f) \
	p=(f)(token_list);\
	if (p == MIN_INT) return 0;


void move_pointer(char **ptr)
{
	while (**ptr &&
		(**ptr == ' '||**ptr == '\t'||**ptr == '\n'||**ptr == '\r')) 
	(*ptr)++;
}

int get_token(char **pbuffer)
{
	switch (**pbuffer) {
		case '[': {
			(*pbuffer)++; move_pointer(pbuffer); return TOK_M_BRAK_O;
		} break;
		case ']': {
			(*pbuffer)++; move_pointer(pbuffer); return TOK_M_BRAK_C;
		} break;
		case '(': {
			(*pbuffer)++; move_pointer(pbuffer); return TOK_S_BRAK_O;
		} break;
		case ')': {
			(*pbuffer)++; move_pointer(pbuffer); return TOK_S_BRAK_C;
		} break;
		case '+': {
			(*pbuffer)++; move_pointer(pbuffer); return TOK_ADD;
		} break;
		case '-': {
			(*pbuffer)++; move_pointer(pbuffer); return TOK_SUB;
		} break;
		case '*': {
			(*pbuffer)++; move_pointer(pbuffer); return TOK_MUL;
		} break;
		case '^': {
			(*pbuffer)++; move_pointer(pbuffer); return TOK_POW;
		} break;
		default: {
			if (is_number(**pbuffer)) {
				char *num_ptr = *pbuffer;
				(*pbuffer)++;
				while (is_number(**pbuffer)) {
					(*pbuffer)++;
				}
				char ch = **pbuffer;
				**pbuffer = 0;
				int num = 0;
				sscanf(num_ptr, "%d", &num);
				**pbuffer = ch;
				move_pointer(pbuffer);
				return num;
			} else if (is_word(**pbuffer)) {
				(*pbuffer)++;
				while (is_word(**pbuffer)||is_number(**pbuffer)) {
					(*pbuffer)++;
				}
				move_pointer(pbuffer);
				return TOK_WORD;
			} else {
				return LEX_ERROR;
			}
		}
	}
}

typedef struct
{
	int *list;
	int index;
	int size;
} tokens;

struct poly_struct;
typedef struct poly_struct
{
	int a, p;
	struct poly_struct *next;
} poly;

poly *make_poly(int a, int p)
{
	poly *pt = (poly*)malloc(sizeof(poly));
	pt->a = a; pt->p = p; pt->next = 0;
	return pt;
}

void add_poly(poly *lhs, poly *rhs)
{
	poly *term = rhs;
	for (; term != 0; term = term->next) {
		if (term->a != 0) {
			poly *p = lhs, *plast = 0;
			for (; p != 0; plast = p, p = p->next) {
				if (p->p >= term->p) break;
			}
			if (p != 0 && p->p == term->p) {
				if ((p->a += term->a) == 0) {
					if (plast != 0) {
						plast->next = p->next;
						free(p);
					}
				}
			} else if (plast == 0) {
				if (lhs->a != 0) {
					poly *pp = (poly*)malloc(sizeof(poly));
					memcpy(pp, lhs, sizeof(poly));
					lhs->next = pp;
				}
				lhs->a = term->a; lhs->p = term->p;
			} else {
				poly *pp = (poly*)malloc(sizeof(poly));
				pp->a = term->a; pp->p = term->p; pp->next = p;
				plast->next = pp;
			}
		}
	}
}

void sub_poly(poly *lhs, poly *rhs)
{
	poly *term = rhs;
	for (; term != 0; term = term->next) {
		if (term->a != 0) {
			poly *p = lhs, *plast = 0;
			for (; p != 0; plast = p, p = p->next) {
				if (p->p >= term->p) break;
			}
			if (p != 0 && p->p == term->p) {
				if ((p->a -= term->a) == 0) {
					if (plast != 0) {
						plast->next = p->next;
						free(p);
					}
				}
			} else if (plast == 0) {
				if (lhs->a != 0) {
					poly *pp = (poly*)malloc(sizeof(poly));
					memcpy(pp, lhs, sizeof(poly));
					lhs->next = pp;
				}
				lhs->a = -term->a; lhs->p = term->p;
			} else {
				poly *pp = (poly*)malloc(sizeof(poly));
				pp->a = -term->a; pp->p = term->p; pp->next = p;
				plast->next = pp;
			}
		}
	}
}

void mul_poly(poly *lhs, poly *rhs)
{

}

void dispose_poly(poly *p)
{
	poly *pp;
	for (; p != 0; p = pp) {
		pp = p->next; free(p);
	}
}

void reverse_poly(poly *p)
{
	for (; p != 0; p = p->next) {
		p->a = -p->a;
	}
}

void put_poly(poly *p)
{
	int first = 0;
	for (; p != 0; p = p->next) {
		if (p->a != 0) {
			if (!first) {
				first = 1;
			} else {
				if (p->a > 0) printf("+");
			}
			switch (p->p) {
				case 0: {
					printf("%d", p->a);
				} break;
				case 1: {
					printf(p->a == 1 ? "x" : p->a == -1 ? "-x" : "%dx", p->a);
				} break;
				default: {
					printf(p->a == 1 ? "x^%*d" : p->a == -1 ? "-x^%*d" : "%dx^%d", p->a, p->p);
				} break;
			}
		}
	}
	puts(first ? "" : "0");
}


int get_number(tokens *);
int get_number1(tokens *);

int get_number2(tokens *token_list)
{
	if (!empty()) {
		switch (current()) {
			case TOK_S_BRAK_O: {	// (expr)
				next();
				force_int(p, get_number);
				if (current() != TOK_S_BRAK_C) {
					assert_number_error("expected ')'");
				}
				next();
				return p;
			} break;
			case TOK_SUB: {
				next();
				force_int(p, get_number1);
				return -p;
			} break;
			case TOK_WORD: {
				return 1;
			} break;
			default: {
				int p = current();
				if (p < 0) assert_number_error("expected number");
				next();
				return p;
			}
		}
	} else {
		assert_number_error("unexpected eof");
	}
}

int get_number1_r(int lhs, tokens *token_list)
{
	if (empty()) {// e
		return lhs;
	}
	switch (current()) {
		case TOK_MUL: {		// * expr1 expr_r
			next();
			force_int(p, get_number2);
			return get_number1_r(lhs * p, token_list);
		} break;
		/*case TOK_DIV: {		// / expr1 expr_r
			next(token_list);
			force_get(poly*, p, get_expr2, token_list);
			div_poly(lhs, p);
			dispose_poly(p);
			return get_expr1_r(lhs, token_list);
		} break;*/
		default: {			// e
			return lhs;
		}
	}
}

int get_number1(tokens *token_list)
{
	force_int(p, get_number2);
	return get_number1_r(p, token_list);
}

int get_number_r(int lhs, tokens *token_list)
{
	if (empty()) {// e
		return lhs;
	}
	switch (current()) {
		case TOK_ADD: {		// + expr1 expr_r
			next();
			force_int(p, get_number1);
			return get_number_r(lhs + p, token_list);
		} break;
		case TOK_SUB: {		// - expr1 expr_r
			next();
			force_int(p, get_number1);
			return get_number_r(lhs - p, token_list);
		} break;
		default: {			// e
			return lhs;
		}
	}
}

int get_number(tokens *token_list)
{
	force_int(p, get_number1);
	return get_number_r(p, token_list);
}

poly *get_term(tokens *token_list)
{
	int a = 1, p = 1;
	if (!empty()) {
		if (current() == TOK_WORD) {
			next();
			if (!empty()) {
				if (current() == TOK_POW) {
					next();
					force_number(p, get_number2);
				}
			}
			return make_poly(a, p);
		} else {
			force_number(a, get_number1);
			if (!empty()) {
				if (current() == TOK_WORD) {
					next();
					if (!empty()) {
						if (current() == TOK_POW) {
							next();
							force_number(p, get_number2);
						}
					}
					return make_poly(a, p);
				}
			}
			return make_poly(a, 0);
		}
	} else {
		assert_parser_error("unexpected eof");
	}
}

poly *get_terms(tokens *token_list)
{
	force_get(poly*, term, get_term);
	if (!empty()) {
		switch (current()) {
			case TOK_ADD: {
				next();
				force_get(poly*, pr, get_terms);
				add_poly(term, pr);
				dispose_poly(pr);
				return term;
			} break;
			case TOK_SUB: {
				next();
				force_get(poly*, pr, get_terms);
				sub_poly(term, pr);
				dispose_poly(pr);
				return term;
			} break;
			default: {
				return term;
			}
		}
	} else {
		return term;
	}
}

poly *get_poly(tokens *token_list)
{
	if (!empty()) {
		if (current() != TOK_M_BRAK_O) {
			assert_parser_error("expected '['");
		}
		next();
		force_get(poly*, p, get_terms);
		if (current() != TOK_M_BRAK_C) {
			assert_parser_error("expected ']'");
		}
		next();
		return p;
	} else {
		assert_parser_error("expected poly");
	}
}

poly *get_expr(tokens *);
poly *get_expr1(tokens *);

poly *get_expr2(tokens *token_list)
{
	if (!empty()) {
		switch (current()) {
			case TOK_S_BRAK_O: {	// (expr)
				next();
				force_get(poly*, p, get_expr);
				if (current() != TOK_S_BRAK_C) {
					assert_parser_error("expected ')'");
				}
				next();
				return p;
			} break;
			case TOK_SUB: {
				next();
				force_get(poly*, p, get_expr1);
				reverse_poly(p);
				return p;
			} break;
			default: {
				force_get(poly*, p, get_poly);
				return p;
			}
		}
	} else {
		assert_parser_error("expected poly");
	}
}

poly *get_expr1_r(poly *lhs, tokens *token_list)
{
	if (empty()) {// e
		return lhs;
	}
	switch (current()) {
		case TOK_MUL: {		// * expr1 expr_r
			next();
			force_get(poly*, p, get_expr2);
			mul_poly(lhs, p);
			dispose_poly(p);
			return get_expr1_r(lhs, token_list);
		} break;
		/*case TOK_DIV: {		// / expr1 expr_r
			next(token_list);
			force_get(poly*, p, get_expr2, token_list);
			div_poly(lhs, p);
			dispose_poly(p);
			return get_expr1_r(lhs, token_list);
		} break;*/
		default: {			// e
			return lhs;
		}
	}
}

poly *get_expr1(tokens *token_list)
{
	force_get(poly*, p, get_expr2);
	poly *pp = get_expr1_r(p, token_list);
	if (!pp) return 0;
	if (pp != p) {
		dispose_poly(p);
	}
	return pp;
}

poly *get_expr_r(poly *lhs, tokens *token_list)
{
	if (empty()) {// e
		return lhs;
	}
	switch (current()) {
		case TOK_ADD: {		// + expr1 expr_r
			next();
			force_get(poly*, p, get_expr1);
			add_poly(lhs, p);
			dispose_poly(p);
			return get_expr_r(lhs, token_list);
		} break;
		case TOK_SUB: {		// - expr1 expr_r
			next();
			force_get(poly*, p, get_expr1);
			sub_poly(lhs, p);
			dispose_poly(p);
			return get_expr_r(lhs, token_list);
		} break;
		default: {			// e
			return lhs;
		}
	}
}

poly *get_expr(tokens *token_list)
{
	if (empty()) {
		assert_parser_error("unexpected eof");
	}
	switch (current()) {
		case TOK_M_BRAK_O: {
			force_get(poly*, p, get_expr1);
			poly* pp = get_expr_r(p, token_list);
			if (!pp) return 0;
			if (pp != p) {
				dispose_poly(p);
			}
			return pp;
		} break;
		default: {
			force_get(poly*, p, get_terms);
			return p;
		}
	}
}

tokens lex(char *buffer)
{
	move_pointer(&buffer);
	int capacity = 1;
	tokens result = {
		(int*)malloc(capacity * sizeof(int)),
		0,
		0,
	};
	while (*buffer) {
		int tok = get_token(&buffer);
		if (!~tok) {
			free(result.list);
			result.list = 0;
			return result;
		}
		//printf("%d\n", tok);
		result.list[result.size++] = tok;
		if (result.size == capacity) {
			int *p = (int*)malloc(capacity * 2 * sizeof(int));
			memcpy(p, result.list, capacity * sizeof(int));
			free(result.list);
			result.list = p;
			capacity *= 2;
		}
	}
	return result;
}

void figure(char *buffer)
{
	tokens token_list = lex(buffer);
	if (!token_list.list) {
		puts("Expression error!");
		return;
	}
	poly *p = get_expr(&token_list);
	if (p) {
		if (token_list.index < token_list.size) {
			puts("Syntax Error!\nunexpected token");
		} else {
			put_poly(p);
		}
		dispose_poly(p);
	}
	free(token_list.list);
}

int main(int argc, char **argv)
{
	while (1) {
		int ch;
		char buffer[MAXLEN], *ptr = buffer; 
		while (ch = getchar(), ch != EOF && ch != '\n') {
			*ptr++ = ch;
		}
		*ptr = 0;
		figure(buffer);
	}
}
