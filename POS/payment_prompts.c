#include "common_prompts.h"
#include "types.h"
#include "utils.h"
#include "payment_prompts.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern Table *tables; // 테이블
extern Product_Array all_products; // 모든 상품들의 목록을 포함한 구조체
extern int table_amount;
extern int date;

void process_payment(int table) {
    while(1) {
        printf("\t1. 한번에 결제\n");
        printf("\t2. 비율 결제\n");
        printf("\t3. 일부만 결제\n");
        printf("\t0. 돌아가기\n");

        printf("POS / 결제 방식 선택 - 번호 선택 > ");
        int ret = command_prompt(3);
        if (ret == 0) {
            return;
        } else if (ret == 1) {
            pay_all_at_once(table);
            return;
        } else if (ret == 2) {
            pay_with_ratio(table);
            return;
        } else if (ret == 3) {
            pay_partially(table);
            return;
        }
    }
}

// 현재 테이블의 총 주문금액을 반환
// 주문금액 0원 이하일 시 -1 반환
int get_total_price(int table_num) {
    Table *table = &tables[table_num - 1];
    int i;
    int order_price = 0; // 주문액 총합 저장
    for (i = 0; i < table->length; i++) { // 현재 테이블에 주문상품 종류 수만큼 반복
        order_price += table->products[i].price * table->products[i].amount;
    }
    if (order_price <= 0) { // 주문 총액이 0 이하일 경우 -1 반환
        return -1;
    }
    return order_price;
}

void pay_all_at_once(int table_num) {
    int order_price = get_total_price(table_num);
    
    while(1) {
        printf("주문 금액 확인 : %d\n", order_price);
        printf("결제하시겠습니까?\n");
        printf("\t1. 결제하기\n");
        printf("\t0. 돌아가기\n");
        printf("POS / 한 번에 결제 - 번호 선택 > ");
        
        int ret = command_prompt(1);
        if (ret == 0) {
            return;
        } else if (ret == 1) {
            break;
        }
    }
    end_purchase(table_num); // 결제 종료
    printf("결제가 완료되었습니다.\n");
    printf("결제 금액 : %d\n", order_price);
    printf("결제 일시 : %8d\n", date);
}

void pay_with_ratio(int table_num) {
    int order_price = get_total_price(table_num);
    int people_num;
    char* input;
    char* confirm_str;
    
    while(1) {
        printf("\t1. 비율 입력받기\n");
        printf("\t0. 돌아가기\n");
        printf("POS / 비율 결제 - 번호 선택 > ");
        
        int ret = command_prompt(1);
        if (ret == 0) {
            return;
        } else if (ret == 1) {
            break;
        }
    }

    printf("주문 금액 확인 : %d\n", order_price);

    while (1) {
        printf("결제할 사람 수 : ");
        input = read_line(); // 입력받기
        people_num = atoi(input);
        int flag = 0;
        if (strlen(input) < 2) {
            for (int i = 0 ; i < strlen(input); i++) {
                if (!isdigit(input[i])) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                if (people_num >= 2 && people_num <= 10) {
                    free(input);
                    break;
                }
            }
        }
        printf("오류 : 인원수는 2에서 10 사이의 숫자만 가능합니다. 유효한 숫자를 입력해 주세요.\n");
        free(input);
    }

    int *arr = malloc(sizeof(int) * people_num);
    if (arr == NULL) {
        printf("오류 : 메모리 문제로 결제에 실패하였습니다. 이전 메뉴로 돌아갑니다.\n");
        return;
    }
    
    for (int i = 0; i < people_num; i++) {
        while (1) {
            int ratio;
            printf("(%d/%d)인: ",i + 1, people_num);
            input = read_line(); // 입력받기
            ratio = atoi(input);
            if (strlen(input) == 1 && ratio > 0 && ratio < 10) {
                arr[i] = ratio;
                free(input); // 기존 문자열 free
                break;
            } else {
                printf("오류 : 비율은 1에서 9 사이의 숫자입니다. 유효한 숫자를 입력해 주세요.\n");
            }
            free(input); // 기존 문자열 free
        }
    }

    while(1) {
        printf("이대로 결제하시겠습니까?\n");
        printf("\t1. 결제하기\n");
        printf("\t0. 돌아가기\n");
        printf("POS / 비율 결제 - 번호 선택 > ");
        
        int ret = command_prompt(1);
        if (ret == 0) {
            return;
        } else if (ret == 1) {
            break;
        }
    }

    int *result_arr = malloc(sizeof(int) * people_num);
    if (result_arr == NULL) {
        printf("오류 : 메모리 문제로 결제에 실패하였습니다. 이전 메뉴로 돌아갑니다.\n");
        return;
    }
    
    calculate_ratio(table_num, people_num, arr, result_arr);
    end_purchase(table_num);
    printf("결제가 완료되었습니다.\n");
    printf("총 결제 금액 : %d\n", order_price);
    for (int i = 0; i < people_num; i++) {
        printf("%d/%d인 결제 금액 : %d\n", i + 1, people_num, result_arr[i]);
    }
    printf("결제 일시 : %8d\n", date);
}

void pay_partially(int table_num) {
    Table *table = &tables[table_num - 1];
    int order_price = get_total_price(table_num);

    while(1) {
        printf("\t1. 일부 결제할 상품 입력\n");
        printf("\t0. 돌아가기\n");
        printf("POS / 일부만 결제 - 번호 선택 > ");
        int ret = command_prompt(1);
        if (ret == 0) {
            return;
        } else if (ret == 1) {
            break;
        }
    }
    
    printf("주문 금액 확인 : %d\n", order_price);
    printf("주문내역 :\n");
    // 주문 내역 출력
    for (int i = 0; i < table->length; i++) {
        printf("%s\t%lld\t%lld\n", table->products[i].name, table->products[i].price, table->products[i].amount);
    }
    
    while(1) {
        printf("부분 결제할 상품: ");
        char *input = read_line();
        int ret = partial_pay(table_num, input);
        if (ret == -1) {
            printf("오류 : 입력이 없거나, 상품명과 수량의 짝이 맞지 않습니다. 유효한 상품명과 수량을 입력해 주십시오.\n");
        } else if (ret == -2) {
            printf("오류 : 존재하지 않는 상품명입니다. 유효한 상품명을 입력해 주십시오.\n");
        } else if (ret == -3) {
            printf("오류 : 상품 수량은 1 이상의 정수입니다. 유효한 수량을 입력해 주십시오.\n");
        } else if (ret == -4) {
            printf("오류 : 주문된 수량보다 결제하려는 수량이 더 많습니다. 유효한 수량을 입력해 주십시오.\n");
        } else if (ret == -5) {
            printf("오류 : 중복된 상품명이 입력되었습니다. 유효한 상품명을 입력해 주십시오.\n");
        } else if (ret == -6) {
            printf("오류 : 메모리 문제로 결제에 실패하였습니다. 이전 메뉴로 돌아갑니다.\n");
            free(input);
            return;
        } else { // 돌아가기
            free(input);
            return;
        }
        free(input);
    }
}

// 비율에 따른 개개인 결제금액 계산
// 파라미터는 (테이블 번호, 결제 인원수, 인원별 비율, 계산값 저장할 인자)
// 계산값 저장할 인자는 호출할 함수에서 미리 선언
void calculate_ratio(int tablenum, int number_of_people, int ratio[], int pay_individual_param[]) {
    int pay_sum = get_total_price(tablenum); // 결제할 총액
    int temp = pay_sum;
    
    int i;
    int ratio_sum = 0; // 비율의 총합을 저장할 변수

    for (i = 0; i < number_of_people; i++) {
        ratio_sum = ratio_sum + ratio[i]; // 입력받은 비율의 총합
    }

    // 나머지는 처음 비율 입력한 사람이 계산
    for (i = 1; i < number_of_people; i++) { // 첫 번째 사람 제외 결제금액 할당
        pay_individual_param[i] = (((pay_sum * ratio[i]) / ratio_sum) / 100) * 100;
        temp -= pay_individual_param[i];
    }
    pay_individual_param[0] = temp;
}

// 테이블에 있는 "전체" 상품 결제 완료 후
// 테이블 주문내역 초기화
// 전체 리스트에 결제된 수량 더해주기
// + 합쳐진 테이블 있는지 돌고 병합 해제
void end_purchase(int tablenum) {
    Table *table = &tables[tablenum - 1];
    // 전체 리스트에 결제된 수량 더하기
    //int i, j;
    //for (i = 0; i < table->length; i++) { // tables[tablenum].list[] 인덱스
    //    for (j = 0; j < all_products.length; j++) { // allproduct.list[] 인덱스
    //        // 주문시에 all_product와 같은 주문명 받아오므로 공백 검사 불필요
    //        // 특정 상품이 주문된 상태에서 상품 정보 변경될 시 고려 필요
    //        if (!strcmp(table->products[i].name, all_products.products[j].name)) {

    //            break; // 다음 tables[tablenum].list 탐색
    //        }
    //    }
    //}
    
    // 테이블 병합 해제
    uncombine_Table(tablenum);

    // 테이블 주문내역 초기화
    if (table->products != NULL) {
        free(table->products);
    }
    table->length = 0;
}

// 문자열을 입력받아 상품명들과 수량을 분석하여 조건에 따라
// 결제 진행후 결제된 금액 반환 또는 오류 반환
// 오류 리스트
// -1: 올바르지 않은 입력
// -2: 존재하지 않는 상품명
// -3: 올바르지 않은 수량
// -4: 주문 수량 보다 결제 수량이 더 많음
// -5: 중복된 상품명 입력
// -6: 메모리 할당 오류
int partial_pay(int table_num, char* input) {
    int parse_count = 0;
    int arr_size = 0;
    char* tmp = malloc(sizeof(char) * (strlen(input) + 1)); // 원본 문자열과 동일한 크기의 임시 배열
    if (tmp == NULL) {
        return -6;
    }
    
    char** products = NULL; // 상품명 어레이
    int* numbers = NULL; // 상품 갯수 어레이

    while (1) {
        int ret;
        size_t tmp_len;
        int number = 0; // 상품갯수
        char name[16]; // 상품명

        trim(input); // 선행 횡공백류 제거
        if (strlen(input) == 0) {
            if (!parse_count) {
                return -1; // 아무것도 입력되지 않음. 상위 프롬프트에서 체크되면 이곳에서 체크할 필요가 없음.
            } else {
                break; // 입력 분석 끝
            }
        }
        
        if (!is_alpha(input[0])) return -1; // 상품 갯수 다음이 알파벳 아니면 오류 반환
        if ((ret = sscanf(input, "%[a-zA-Z ]", tmp)) != 1) break; // 상품명 읽기
        trim(tmp); // 후행 공백 제거
        if ((tmp_len = strlen(tmp)) > 15) return -1; // 실 상품명 길이가 15자가 넘으면 올마르지 않은 상품명이기에 오류 반환
        remove_all_space(tmp); // 띄어쓰기 제거
        to_lower(tmp); // 모두 소문자로 변환
        strcpy(name, tmp); // 상품명 임시 저장
        memcpy(input, input + tmp_len, strlen(input) - tmp_len + 1); // 숫자를 읽기 위해 상품명 부분 제거후 당기기
        parse_count++;
        
        trim(input); // 선행 횡공백류 제거
        if (!isdigit(input[0])) return -1; // 상품명 다음이 숫자가 아니면 오류 반환
        if ((ret = sscanf(input, "%[0-9.]", tmp)) != 1) break; // 상품 갯수 읽기
        if (strstr(tmp, ".") != NULL) return -3; // 상품 갯수에 소수점이 포함됨 포함시 에러 반환 TODO: 1.0, 1.00등은 허용할지? <- 기획서 수정필요?
        
        number = atoi(tmp); // 상품 갯수 임시 저장
        if (number < 1) return -3; // 1보다 작은 숫자임
        tmp_len = strlen(tmp); // 상품갯수 길이
        memcpy(input, input + tmp_len, strlen(input) - tmp_len + 1); // 숫자를 읽기 위해 상품명 부분 제거후 당기기
        parse_count++;
        
        // 임시 배열에 추출한 상품명과 갯수 기록
        void* realloced_products = realloc(products, sizeof(char*) * ++arr_size);
        void* realloced_numbers = realloc(numbers, sizeof(int) * arr_size);
        void* malloced_name = malloc(sizeof(char) * 16);
        // malloc, realloc에 실패하면
        if (!realloced_products || !realloced_numbers || !malloced_name) {
            // 이전에 할당된 메모리 free
            if (!realloced_products) free(products);
            if (!realloced_numbers) free(numbers);
            return -6; // 오류 반환
        }
        
        // realloc된 메모리 주소 할당
        products = realloced_products;
        numbers = realloced_numbers;
        strcpy(malloced_name, name); // 문자열 복사
        
        // 어레이에 저장
        int index = arr_size - 1;
        products[index] = malloced_name;
        numbers[index] = number;
    }

    if (parse_count % 2 != 0) return -1; // 상품명과 상품 갯수가 짝이 맞지 않음 -> 입력이 잘못됨
    
    // 상품명 중복 체크
    for (int i = 0; i < arr_size; i++) {
        for (int j = i + 1; j < arr_size; j++) {
            char a[16], b[16];
            strcpy(a, products[i]);
            remove_all_space(a);
            to_lower(a);
            strcpy(b, products[j]);
            remove_all_space(b);
            to_lower(b);
            if (!strcmp(a, b)) return -5;
        }
    }

    // 주문 내역에 결제할 상품이 있는지 확인후 인덱스 저장
    int* index_table = malloc(sizeof(int) * arr_size);
    if (index_table == NULL) {
        return -6;
    }
    
    for (int i = 0; i < arr_size; i++) {
        int index = -1;
        for (int j = 0; j < tables[table_num - 1].length; j++) {
            char tmp[16];
            strcpy(tmp, tables[table_num - 1].products[j].name);
            remove_all_space(tmp);
            to_lower(tmp);
            if (strcmp(tmp, products[i]) == 0) {
                index = j;
                break;
            }
        }
        if (index == -1) return -2; // 존재하지 않는 상품명
        if (tables[table_num - 1].products[index].amount < numbers[i]) return -4; // 주문한 상품수보다 계산하려는 상품수가 더 많음
        index_table[i] = index; // 인덱스 저장
    }

    // 정말로 결제?
    while(1) {
        printf("이대로 결제하시겠습니까?\n");
        printf("\t1. 결제하기\n");
        printf("\t0. 돌아가기\n");
        printf("POS / 일부만 결제 - 번호 선택 > ");
        
        int ret = command_prompt(1);
        if (ret == 0) {
            return -10;
        } else if (ret == 1) {
            break;
        }
    }

    int total_price = 0; // 결제하려는 토탈 금액

    Table* table = &tables[table_num - 1];

    for (int i = 0; i < arr_size; i++) {
        int index = index_table[i];
        tables[table_num - 1].products[index].amount -= numbers[i]; // 테이블에 존제하는 갯수 감소

//        int ap_index = -1;
//        for (int j = 0; j < all_products.length; j++) {
//            if (strcmp(all_products.products[j].name, tables[table_num - 1].products[index].name) == 0) {
//                ap_index = j;
//                break;
//            }
//        }
//        all_products.products[ap_index].amount += numbers[i]; // 정산을 위해 판매 내역에 더하기
        total_price += numbers[i] * tables[table_num - 1].products[index].price;
        
        // 개수가 0이 되면 배열 당기기
        if (table->products[index].amount == 0) { //상품이 0개가 되면 realloc 후 땡김
            int64_t updateLength = table->length - 1; //새로운 realloc 크기

            for (int64_t i = index; i < updateLength; i++) { //앞으로 한칸씩 땡김
                table->products[i].amount = table->products[i + 1].amount;
                table->products[i].price = table->products[i + 1].price;
                strcpy(table->products[i].name, table->products[i + 1].name);
            }

            table->products = safe_realloc_trim(table->products, updateLength * sizeof(Product));
//            void* realloced = realloc(table->products, updateLength * sizeof(Product));
//            if (realloced == NULL) {
//                printf("오류 : 메모리 문제로 취소에 실패하였습니다. 이전 메뉴로 돌아갑니다.\n");
//                return;
//            }
//            else {
//                table->products = realloced;
//            }
            table->length--;
        }
    }
    
    int purchase_end = 1;
    for (int i = 0; i < table->length && purchase_end == 1; i++) {
        if (table->products[i].amount != 0) {
            purchase_end = 0;
        }
    }
    
    if (purchase_end == 1) {
        end_purchase(table_num);
    }
    
    // 작업이 끝났으니 malloc 한 항목들 free
    free(tmp);
    free(numbers);
    free(index_table);
    for (int i = 0; i < arr_size; i++) {
        free(products[i]);
    }
    free(products);
    
    printf("결제가 완료되었습니다.\n");
    printf("총 결제 금액 : %d\n", total_price);
    
    return total_price;
}

void uncombine_Table(int table_num) {
    for (int i = 0; i < table_amount; i++) { //전체 테이블 for loop 돌면서
        if (tables[i].delegate == table_num && tables[i].status == kCombined) { // 대표테이블이 현재 테이블과 같다면
            tables[i].delegate = -1;
            tables[i].status = kOrdinary;
        }
    }
}
