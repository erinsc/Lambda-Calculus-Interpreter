#ifndef _PARSER_H
#define _PARSER_H

#include <stdint.h>
#include <stdio.h>
#include "types.h"

size_t _lambda_print(const array_t term, size_t index) {
    num_t* values = term.values;
    if (values[index] == 0) {
        if (values[index+1] == 1) printf("(");
        size_t second_index = _lambda_print(term, index+1);
        if (values[index+1] == 1) printf(")");
        printf(" ");
        if (values[second_index] <= 1) printf("(");
        size_t third_index = _lambda_print(term, second_index);
        if (values[second_index] <= 1) printf(")");
        return third_index;
    }
    if (values[index] == 1) {
        printf("λ");
        _lambda_print(term, index+1);
        while (values[index+2] == 1) {
            printf(" ");
            _lambda_print(term, index+3);
            index += 2;
        }
        printf(".");
        return _lambda_print(term, index+2);
    }
    if (values[index] == 2) {
        num_t acc = values[index+1] - 3;
        while (acc > 0) {
            printf("%c", (char)((acc -1) % 26) + 'A');
            acc = (acc - 1) / 26;
        }        
        return index+2;
    }
    num_t n = values[index]-3;
    printf("%c", (char)(n % 26 + 'a'));
    if (n / 26 > 0)
        printf("%zu", n / 26);
    return index+1;
}
void lambda_print(const array_t term) {
    _lambda_print(term, 0);
    printf("\n");
}
void _parse_whitespace(const char text[], size_t* read_pos) {
    char c = text[*read_pos];
    while (c == ' ' || c == '\t') {
        c = text[++(*read_pos)];
    }
}
char _parse_exp(const char text[], size_t* read_pos, array_t term, size_t* write_pos) {
    num_t* term_v = term.values;
    term_v[(*write_pos)++] = 2;
    size_t counter = 0;
    
    char c = text[*read_pos];
    while ('A' <= c && c <= 'Z' && counter < 6) {
        c = text[++(*read_pos)];
        ++counter;
    }
    num_t acc = 0;
    while (counter > 0) {
        c = text[--(*read_pos)];
        acc = acc * 26 + c - 'A'+1;
        --counter;
    }
    while ('A' <= c && c <= 'Z') {
        c = text[++(*read_pos)];
    }
    term_v[(*write_pos)++] = acc + 3;
    _parse_whitespace(text, read_pos);
    return 'S';
}
char _parse_var(const char text[], size_t* read_pos, array_t term, size_t* write_pos) {
    num_t* term_v = term.values;
    char c = text[(*read_pos)++];
    num_t acc = 0;
    char n = text[(*read_pos)];
    while ('0' <= n && n <= '9') {
        acc = acc * 10 + c - '0';
        n = text[++(*read_pos)];
    }
    acc = acc * 26 + c - 'a';
    term_v[(*write_pos)++] = acc + 3;
    _parse_whitespace(text, read_pos);
    return 'S';
}
char _parse_app(const char*, size_t*, array_t, size_t*);
char _parse_abs(const char text[], size_t* read_pos, array_t term, size_t* write_pos) {
    while (true) {
        term.values[(*write_pos)++] = 1;
        _parse_whitespace(text, read_pos);
        char c = text[(*read_pos)];
        if (!('a' <= c && c <= 'z')) return 'F';
        _parse_var(text, read_pos, term, write_pos);
        c = text[(*read_pos)];
        if (c == '.') {
            ++(*read_pos);
            if (_parse_app(text, read_pos, term, write_pos) == 'F') return 'F';
            return 'S';
        }
        if (!('a' <= c && c <= 'z')) return 'F';
    }
}
char _parse_app(const char text[], size_t* read_pos, array_t term, size_t* write_pos) {
    size_t app_start = *write_pos;
    bool first = true;
    bool read = false;
    while (true) {
        if (!first && read) {
            for (size_t pos = *write_pos; pos > app_start; --pos) {
                term.values[pos] = term.values[pos-1];
            }
            term.values[app_start] = 0;
            ++(*write_pos);
        }
        if (read) {
            first = false;
        }
        _parse_whitespace(text, read_pos);
        char c = text[*read_pos];
        if (c == ')' || c == '\0') {
            return 'S';
        }
        else if ('a' <= c && c <= 'z') {
            read = true;
            _parse_var(text, read_pos, term, write_pos);
        }
        else if ('A' <= c && c <= 'Z') {
            read = true;
            _parse_exp(text, read_pos, term, write_pos);
        }
        else if (c == '(') {
            read = true;
            ++(*read_pos);
            if (_parse_app(text, read_pos, term, write_pos) == 'F') return 'F';
            ++(*read_pos);
        }
        else if (c == '\\') {
            read = true;
            ++(*read_pos);
            if (_parse_abs(text, read_pos, term, write_pos) == 'F') return 'F';
        }
        else {
            read = false;
            ++(*read_pos);
            printf("UNKNOWN: %c\n", c);
        }
    }
}
size_t lambda_parse(const char text[], array_t term) {
    size_t read = 0;
    size_t write = 0;
    if (_parse_app(text, &read, term, &write) == 'F') return 0;
    return write;
}

#endif //_PARSER_H
