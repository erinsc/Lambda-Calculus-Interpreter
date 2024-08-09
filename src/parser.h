#ifndef _PARSER_H
#define _PARSER_H

#include "term.h"

size_t _lambda_print(const term_t* term, size_t index);
void lambda_print(const term_t* term);

void _parse_whitespace(const char text[], size_t* read_pos);
char _parse_exp(const char text[], size_t* read_pos, term_t* term, size_t* write_pos);
char _parse_var(const char text[], size_t* read_pos, term_t* term, size_t* write_pos);

char _parse_abs(const char text[], size_t* read_pos, term_t* term, size_t* write_pos);
char _parse_app(const char*, size_t*, term_t*, size_t*);

void lambda_parse(const char text[], term_t* term);
num_t exp_parse(const char text[]);
void lambda_log(const term_t* term);

#endif //_PARSER_H
