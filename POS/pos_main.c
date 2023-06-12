#include "utils.h"
#include "types.h"
#include "login.h"
#include "main_prompts.h"
#include "table_amount_prompt.h"
#include "product.h"

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

Table *tables; // 테이블 4개
int table_amount;
Product_Array all_products; // 모든 상품들의 목록을 포함한 구조체
char base_dir[FILENAME_MAX];
int date; // 로그인된 날짜

extern int latest_login_date; // 로그인 실패시 사용, 가장 최근 로그인 날짜

void pos_main(char* path) {
    setlocale(LC_ALL, "en-US.UTF-8");
    
    get_current_dir(path, base_dir);
    // 파일 무결성 검사
    if(read_product_file() == -1) exit(EXIT_FAILURE);
    
    // table_amount가 -1이라면 테이블 개수 입력 테이블로
    if (table_amount == -1) table_amount_prompt();
    if ((tables = malloc(sizeof(Table) * table_amount)) == NULL) {
        printf("메모리 할당에 실패하였습니다. 프로그램을 종료합니다.\n");
        exit(EXIT_FAILURE);
    }
    
    // init tables
    for (int i = 0; i < table_amount; i++) {
        tables[i].products = NULL;
        tables[i].length = 0;
        tables[i].status = kOrdinary;
        tables[i].delegate = -1;
    }
    
    // 로그인
    while (1) {
        printf("POS / 로그인 메뉴 - 날짜 입력> ");
        char *input = read_line();
        trim(input);
        int result = process_login(input);
        if (result == -1) {
            printf("오류 : YYYYMMDD 혹은 YYMMDD형식으로 입력해주십시오.\n");
        } else if (result == -2) {
            printf("오류 : 그레고리력에 존재하지 않는 날짜입니다.\n");
        }
        else if (result == -3) {
            int year = latest_login_date / 10000;
            int month = (latest_login_date / 100) - year * 100;
            int date = latest_login_date % 100;
            printf("오류: 마지막 로그인 날짜가 %d년 %d월 %d일입니다. 마지막 정산일 이후의 날짜로만 로그인 할 수 있습니다.\n", year, month, date);
        } else {
            date = result; // 로그인 날짜 저장
            break;
        }
    }
    
    // 메인 프롬프트로
    main_prompt();
}
