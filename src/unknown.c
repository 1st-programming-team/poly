#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

void figure(char *buffer)
{
	tokens token_list = lex(buffer);
	if (!token_list.list) {
		puts("Expression error!");
		return;
	}
	poly_data p = get_expr(&token_list);
	if (p.data) {
		if (token_list.index < token_list.size) {
			puts("Syntax Error!\nunexpected token");
		} else {
			put_poly(p.data);
		}
		dispose_poly(p.data);
	}
	free(token_list.list);
}

int main(int argc, char **argv)
{
	while (1) {
		int ch, capacity = 1, idx = 0;
		char *buffer = (char*)malloc(capacity * sizeof(char));
		while (ch = getchar(), ch != EOF && ch != '\n') {
			buffer[idx++] = ch;
			if (idx == capacity) {
				char *ptr = (char*)malloc(2 * capacity * sizeof(char));
				memcpy(ptr, buffer, capacity * sizeof(char));
				free(buffer); buffer = ptr;
				capacity <<= 1;
			}
		}
		buffer[idx] = 0;
		figure(buffer);
		free(buffer);
	}
	return 0;
}
