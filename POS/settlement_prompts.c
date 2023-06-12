//
//  settlement_prompt.c
//  POS
//
//  Created by 정준규 on 2023/04/25.
//

#include "common_prompts.h"
#include "settlement_prompts.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void settlement_prompt() {
    while (1) {
        printf("<날짜별 정산>\n");
        printf("\t1. 특정 날짜 입력\n");
        printf("\t0. 돌아가기\n");
        printf("POS / 메인 메뉴 - 번호 선택 > ");
        
        int ret = command_prompt(1);
        if (ret == 0) {
            return;
        } else if (ret == 1) {
            printf("날짜별 정산 내역 - 날짜입력 > ");
            char* date_input = read_line();
            trim(date_input);
            to_lower(date_input);
            print_settlement(date_input);
        }
    }
}
