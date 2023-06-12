#ifndef utils_h
#define utils_h

#include <stdio.h>

// stdin에서 개행문자 전까지 문자 읽어오기
char* read_line(void);

// FILE포인터에서 개행문자 전까지 문자 읽어오기
char* read_line_f(FILE *stream);

// 문자열 맨 앞, 맨 뒤에서 횡공백류 제거
void trim(char *string);

// 문자열 내부에 모든 횡공백류 제거
void remove_all_spaces(char *string);

// 문자열 내부에 모든 공백 제거
void remove_all_space(char* string);

// 입력된 명령어를 숫자로 반환
// 존제하지 않을경우 -1 반환
int is_correct_command(char *string);

// 문자열 내부에 공백이 있는지 체크
int is_contain_spaces(char* string);

// 문자열 내부에 숫자가 아닌 문자가 있는지 체크
int is_contain_non_number(char* string);

// 문자가 알파벳인지 체크
// isalpha가 있지만 윈도우 환경에서 한글이 들어가면 뻗어버리는 문제가 있음.
int is_alpha(int c);

// 문자열내 모든 알파벳 소문자로
void to_lower(char *string);

// 날짜 유효성 체크
int checkDate(char date_input[]);

// 프로그램 위치 불러오기
void get_current_dir(char *from, char *at);

void* safe_realloc_trim(void *p, size_t n);

#endif
