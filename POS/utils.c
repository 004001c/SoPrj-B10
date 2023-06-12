#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 개행문자 전까지 입력을 읽어들임
// 메모리 누수 방지를 위해선 사용 후 free 해줘야함.
char* read_line() {
    return read_line_f(stdin);
}
 
char* read_line_f(FILE* stream) {
    int buffer_size = 16;
    int size = buffer_size;
    char* string = malloc(sizeof(char) * size); // 처음은 16자리 char 배역
    if (!string) return NULL; // 메모리 할당 실패시 NULL 반환
    int pos = 0;
    int read;

    while ((read = getc(stream)) != EOF && read != '\n') { // EOF 또는 개행 문자 전까지
        if (pos == size) { // 할당한 배열이 꽉차면
            size += buffer_size; // buffer_size만큼 크기 증가
            char* realloced = realloc(string, sizeof(char) * size); // 배열 크기 확장
            if (!realloced) {
                free(string);
                return NULL; // 확장 실패시 NULL 반환
            } else {
                string = realloced;
            }
        }
        string[pos++] = (char)read; // 읽은 문자 배열에 저장
    }

    if (pos == size) { //배열이 꽉찼다면
        char* realloced = realloc(string, sizeof(char) * (size + 1)); // 한칸 확장
        if (!realloced) {
            free(string);
            return NULL; // 확장 실패시 NULL 반환
        }
        else {
            string = realloced;
        }
    }
    string[pos] = '\0'; // 문자열 끝에 널문자 추가
    return string; // 읽은 문자열 반환
}

// 문자열 앞 뒤에서 횡공백류 제거
// <횡공백류열0><상품명><횡공백류열1><수량><횡공백류열0> 인경우 맨 앞과 맨 뒤의 <횡공백류열0>만 제거함
void trim(char* string) {
    int len = strlen(string);
    int pre = 0;
    int suf = len - 1;
    
    int flag = 0; // 0: 횡공백류열만 있음, 1: 횡공백류열이 아닌 문자가 포함됨
    for (int i = 0; i < len && flag == 0; i++) {
        if (string[i] != ' ' && string[i] != '\t') {
            flag = 1;
            break;
        }
    }
    // 횡공백류열만 있으므로
    if (flag == 0){
        string[0] = '\0'; // 빈 문자열로 만들고
        return; // Early return
    }

    // 횡공백류가 아닌 첫번째 위치 찾기
    while (string[pre] == ' ' || string[pre] == '\t') {
         pre++;
    }

    // 횡공백류가 아닌 마지막 위치 찾기
    while (string[suf] == ' ' || string[suf] == '\t') {
        suf--;
    }

    int true_len = suf - pre + 1; // 실제 문자열 구간 길이
    memmove(string, string + pre, true_len); // 맨 앞으로 문자열 당기기
    string[true_len] = '\0';
}

// 문자열에서 모든 횡공백류를 제거
void remove_all_spaces(char* string) {
    size_t len = strlen(string);
    int swap = 1;
    for (int i = len - 1 ; i > 0 && swap; i--) {
        swap = 0;
        for (int j = 0; j < i; j++) {
            if(string[j] == ' ' || string[j] == '\t') {
                string[j] = string[j + 1];
                string[j + 1] = ' ';
                swap = 1;
            }
        }
    }
    trim(string);
}

// 문자열에서 모든 공백 제거
void remove_all_space(char* string) {
    size_t len = strlen(string);
    int swap = 1;
    for (int i = len - 1 ; i > 0 && swap; i--) {
        swap = 0;
        for (int j = 0; j < i; j++) {
            if(string[j] == ' ') {
                string[j] = string[j + 1];
                string[j + 1] = ' ';
                swap = 1;
            }
        }
    }
    trim(string);
}


int is_correct_command(char *string) {
    char command_list[12][6] = {
        "0", "back",
        "1", "one",
        "2", "two",
        "3", "three",
        "4", "four",
        "5", "five",
    };
    int flag = -1;
    for (int i = 0; i < 12 && flag == -1; i++) {
        if(strcmp(command_list[i], string) == 0) {
            if (i % 2 != 0) {
                return (i - 1) / 2;
            } else {
                return i / 2;
            }
        }
    }
    return flag;
}

int is_contain_spaces(char* string) {
    int flag = 0;
    for (int i = 0; i < strlen(string) && flag == 0; i++) {
        if (string[i] == ' ' || string[i] == '\t') flag = 1;
    }
    return flag;
}

int is_contain_non_number(char* string) {
    int flag = 0;
    for (int i = 0; i < strlen(string) && flag == 0; i++) {
        if (string[i] < '0' || string[i] > '9') flag = 1;
    }
    return flag;
}

int is_alpha(int c){
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ? 1 : 0);
}

void to_lower(char* string) {
    size_t len = strlen(string);
    for (int i = 0; i < len; i++) {
        if (isupper(string[i])) {
            string[i] = tolower(string[i]);
        }
    }
}

int checkDate(char date_input[]) { // 날짜 규칙확인
    int non_leap_year_dates[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    int leap_year_dates[12] = { 31,29,31,30,31,30,31,31,30,31,30,31 };
    int day, year, month, date;
    if (strlen(date_input) == 6) {
        for (int i = 0; i < 6; i++) {
            if (!isdigit(date_input[i]))
                return -1; // 날짜가 숫자가 아님 // YYYYMMDD YYMMDD 형식으로 출력
        }
        day = atoi(date_input);
        day += 20000000;
        year = day / 10000;
        month = (day / 100) - year * 100;
        date = day % 100;
        if (month > 12 && month <1)
            return -2; // 그레고리 력에 존재하는 날이 아님
        if ((year % 4 == 0) && !((year % 100) == 0 && year % 400 != 0)) {
            if (date <= leap_year_dates[month - 1]&&date>0)
                return day; // 올바른 입력
            else
                return -2; // 그레고리 력에 존재하는 날이 아님
        }
        else {
            if (date <= non_leap_year_dates[month - 1]&&date>0)
                return day;// 올바른 입력
            else
                return -2;// 그레고리 력에 존재하는 날이 아님
        }

    }
    else if (strlen(date_input) == 8) {
        for (int i = 0; i < 8; i++) {
            if (!isdigit(date_input[i]))
                return -1;
        }
        int day = atoi(date_input);
        year = day / 10000;
        month = (day / 100) - year * 100;
        date = day % 100;
        if (month > 12 && month<1)
            return -2; // 그레고리 력에 존재하는 날이 아님
        if ((year % 4 == 0) && !((year % 100) == 0 && year % 400 != 0)) {
            if (date <= leap_year_dates[month - 1] && date >0)
                return day;// 올바른 입력
            else
                return -2;// 그레고리 력에 존재하는 날이 아님
        }
        else {
            if (date <= non_leap_year_dates[month - 1] && date>0)
                return day;// 올바른 입력
            else
                return -2;// 그레고리 력에 존재하는 날이 아님
        }
    }
    else
        return -1; // 날짜가 6글자나 8글자가 아님 // YYYYMMDD YYMMDD 형식으로 출력
}

void get_current_dir(char *from, char *at) {
    int last = 0;
#ifdef _WIN32
    char divider = '\\';
#else
    char divider = '/';
#endif

    for (int i = strlen(from) - 1; i > 0; i--) {
        if (from[i] == divider) {
            last = i;
            break;
        }
    }
    strncpy(at, from, last + 1);
    sprintf(at, "%s%s%c", at, "posdata", divider);
}

void* safe_realloc_trim(void *p, size_t n) {
    void *p2 = realloc(p, n);
    return p2 ? p2 : p;
}
