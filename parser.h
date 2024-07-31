#ifndef _PARSER_H
#define _PARSER_H

#include <stdint.h>
#include <stdio.h>

typedef uint_fast32_t num_t;

size_t _lambda_print(const num_t term[], size_t index) {
    if (term[index] == 0) {
        if (term[index+1] == 1) printf("(");
        size_t second_index = _lambda_print(term, index+1);
        if (term[index+1] == 1) printf(")");
        printf(" ");
        if (term[second_index] <= 1) printf("(");
        size_t third_index = _lambda_print(term, second_index);
        if (term[second_index] <= 1) printf(")");
        return third_index;
    }
    if (term[index] == 1) {
        printf("λ");
        _lambda_print(term, index+1);
        while (term[index+2] == 1) {
            printf(" ");
            _lambda_print(term, index+3);
            index += 2;
        }
        printf(".");
        return _lambda_print(term, index+2);
    }
    printf("%zu", term[index]);
    return index+1;
}
void lambda_print(const num_t term[]) {
    _lambda_print(term, 0);
    printf("\n");
}

void _parse_whitespace(const char text[], size_t* read_pos) {
    char c = text[*read_pos];
    while (c == ' ' || c == '\t') {
        c = text[++(*read_pos)];
    }
}
void _parse_var(const char text[], size_t* read_pos, num_t term[], size_t* write_pos) {
    char c = text[*read_pos];
    num_t acc = 0;
    while ('0' <= c && c <= '9') {
        acc = acc * 10 + c - '0';
        c = text[++(*read_pos)];
    }
    term[(*write_pos)++] = acc;
}
void _parse_app(const char*, size_t*, num_t*, size_t*);
void _parse_abs(const char text[], size_t* read_pos, num_t term[], size_t* write_pos) {
    while (true) {
        term[(*write_pos)++] = 1;
        _parse_whitespace(text, read_pos);
        _parse_var(text, read_pos, term, write_pos);
        
        _parse_whitespace(text, read_pos);
        char c = text[(*read_pos)];
        if (c == '.') {
            ++(*read_pos);
            _parse_app(text, read_pos, term, write_pos);
            break;
        }
    }
}
void _parse_app(const char text[], size_t* read_pos, num_t term[], size_t* write_pos) {
    size_t app_start = *write_pos;
    bool first = true;
    while (true) {
        _parse_whitespace(text, read_pos);
        char c = text[*read_pos];
        if (c == ')' || c == '\0') {
            break;
        }
        if (!first) {
            for (size_t pos = *write_pos; pos > app_start; --pos) {
                term[pos] = term[pos-1];
            }
            term[app_start] = 0;
            ++(*write_pos);
        }
        first = false;
        if ('0' <= c && c <= '9') {
            _parse_var(text, read_pos, term, write_pos);
            _parse_whitespace(text, read_pos);
        }
        else if (c == '(') {
            ++(*read_pos);
            _parse_app(text, read_pos, term, write_pos);
            ++(*read_pos);
        }
        else if (c == '\\') {
            ++(*read_pos);
            _parse_abs(text, read_pos, term, write_pos);
        }
        else {
            ++(*read_pos);
        }
    }
}
size_t lambda_parse(const char text[], num_t term[]) {
    size_t read = 0;
    size_t write = 0;
    _parse_app(text, &read, term, &write);
    return write;
}

#endif //_PARSER_H
