//
//  common_prompts.c
//  core
//
//  Created by 이승윤 on 2023/05/05.
//

#include "common_prompts.h"
#include "utils.h"

#include <string.h>
#include <stdlib.h>

int command_prompt(int max) {
    return command_prompt_b(max, 1);
}

int command_prompt_b(int max, int back) {
    char* input = read_line(); // 선택지 입력받기
    trim(input);
    to_lower(input);
    int ret = is_correct_command(input);
    if (strlen(input) == 0) {
        printf("오류 : 명령어를 입력해주세요.\n");
    } else if (is_contain_spaces(input)) {
        printf("오류 : 명령어가 너무 많습니다. 최대 1개의 명령어만 인자로 입력해주세요.\n");
    } else if (ret > max || (back == 0 && ret == 0)) {
        printf("오류 : 현재 메뉴에 '%d'번 선택지는 존재하지 않습니다.\n", ret);
    } else if (ret < 0) {
        printf("오류 : '%s'이라는 명령어는 없습니다\n", input);
    } else {
        return ret;
    }
    
    printf("-----------------+-------------------------------------+----------------------------------\n");
    printf("명령어군\t|\t설명\n");
    if (1 <= max) printf("1 one\t|\t선택지의 1번 메뉴로 갑니다.\n");
    if (2 <= max) printf("2 two\t|\t선택지의 2번 메뉴로 갑니다.\n");
    if (3 <= max) printf("3 three\t|\t선택지의 3번 메뉴로 갑니다.\n");
    if (4 <= max) printf("4 four\t|\t선택지의 4번 메뉴로 갑니다.\n");
    if (5 <= max) printf("5 fice\t|\t선택지의 5번 메뉴로 갑니다.\n");
    if (back == 1) printf("0 back\t|\t이전 메뉴로 돌아갑니다.\n");
    printf("-----------------+-------------------------------------+----------------------------------\n");

    free(input);
    return -1;
}

int read_amount() {
    char* input = read_line(); // 개수 입력받기
    trim(input); // 앞뒤 횡공백류 제거
    int result = 0;
    if (strlen(input) == 0) {
        result = -1;
    } else if (is_contain_spaces(input)) {
        result = -2;
    } else if (is_contain_non_number(input)) {
        result = -3;
    } else if (input[0] == '0') {
        if (strlen(input) == 1) {
            result = -5;
        } else {
            result = -4;
        }
    } else {
        result = atoi(input);
        if (result > 20) result = -5;
    }
    free(input);
    return result;
}
