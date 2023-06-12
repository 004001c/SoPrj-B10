//
//  sell.c
//  POS
//
//  Created by 이승윤 on 2023/04/24.
//

#include "common_prompts.h"
#include "sell_prompts.h"
#include "payment_prompts.h"
#include "types.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

extern Table *tables; // 테이블
extern Product_Array all_products;
extern int table_amount;

void sell_prompt() {
    int table_num;

    while (1) {
        table_num =-1;
        printf("<가능한 테이블 목록>\n");
        for (int i = 1; i <= table_amount; i++) {
            if (tables[i - 1].status == kCombined) continue;
            printf("\t%d. %d번 테이블\n", i, i);
        }
        printf("\t0. 돌아가기\n");

        printf("POS / 판매관리(테이블 관리) - 테이블 번호 선택 > ");
        
        int selected_table;
        char* input = read_line(); // 개수 입력받기
        trim(input); // 앞뒤 횡공백류 제거
        if (strlen(input) == 0) {
            printf("오류 : 테이블 번호를 입력해주세요.\n");
        } else if (is_contain_spaces(input)) {
            printf("오류 : 테이블 번호는 다음과 같은 형식으로 입력할 수 있습니다. <횡공백류열0><개수><횡공백류열0>\n");
        } else if (is_contain_non_number(input)) {
            printf("오류 : 테이블 번호의 숫자가 아닌 것이 포함되어 있습니다. 개수는 숫자로만 입력할 수 있습니다.\n");
        } else if (input[0] == '0') {
            if (strlen(input) == 1) {
                return;
            } else {
                printf("오류 : 테이블 번호의 첫글자가 0입니다. 개수는 0으로 시작할 수 없습니다.\n");
            }
        } else if ((selected_table = atoi(input)) > 20) {
            printf("오류 : 테이블 번호는 1이상 20이하의 숫자여야 합니다.\n");
        } else if (selected_table > table_amount || tables[selected_table - 1].status == kCombined) {
            printf("오류 : 가능한 테이블 목록에 있는 숫자를 입력해주세요.\n");
        } else {
            table_management_prompt(selected_table);
        }
        free(input);
    }
}

void table_management_prompt(int table_num) {
    while(1) {
        printf("<%d번 테이블>\n", table_num);
        printf("\t1. 주문내역 출력\n");
        printf("\t2. 상품 주문\n");
        printf("\t3. 상품 취소\n");
        printf("\t4. 결제\n");
        printf("\t5. 테이블 합치기\n");
        printf("\t0. 돌아가기\n");

        printf("POS / %d번 테이블 - 테이블 번호 선택 > ", table_num);
        
        int ret = command_prompt(5);
        if (ret == 0) {
            return;
        } else if (ret == 1) {
            if (is_empty_table(table_num)) {
                printf("오류 : 주문된 상품이 없습니다.\n");
            } else {
                print_receipt(table_num);
            }
        } else if (ret == 2) {
            order_product(table_num);
        } else if (ret == 3) {
            if (is_empty_table(table_num)) {
                printf("오류 : 주문된 상품이 없습니다.\n");
            } else {
                cancel_order(table_num);
            }
        } else if (ret == 4) {
            if (is_empty_table(table_num)) {
                printf("오류 : 주문된 상품이 없습니다.\n");
            } else {
                process_payment(table_num);
            }
        } else if (ret == 5) {
            int possible_combine = 0;
            for (int i = 0; i < table_amount && possible_combine == 0; i++) {
                if (i == table_num - 1) continue;
                if (tables[i].status == kOrdinary) possible_combine = 1;
            }
            if (possible_combine == 1) {
                combine_Table(table_num);
            } else {
                printf("오류 : 합칠 수 있는 테이블이 없습니다.\n");
            }
        }
    }
}

// 주문내역출력
void print_receipt(int table_num) {
    Table *table = &tables[table_num - 1];
    int total_price = 0;
    
    printf("<주문 내역>\n");
    printf("\t테이블 번호 : %d번\n", table_num);
    printf("\t주문 내역 : \n");
    
    // 주문 내역 출력
    for (int i = 0; i < table->length; i++) {
        if (table->products[i].amount == 0) continue;
        printf("\t%d.%s\t%lld\t%lld\n", i+1, table->products[i].name, table->products[i].price, table->products[i].amount);
        total_price += table->products[i].price * table->products[i].amount;
    }
    // 합계 금액 출력
    printf("\t합계 : %d원\n", total_price);
}

void order_product(int table_num) {
    int i;
    Product *product_to_order = NULL;
    
    while(1) {
        printf("<상품 주문>\n");
        printf("\t1. 상품명 입력\n");
        printf("\t0. 돌아가기\n");
        printf("POS / (상품 주문) - 번호 선택 > ");
        
        int ret = command_prompt(1);
        if (ret == 0) {
            return;
        } else if (ret == 1) {
            break;
        }
    }
    
    printf("<상품 주문>");
    printf("\t테이블 번호 %d번\n", table_num);
    printf("\t상품 목록:\n");
    for (i = 0; i < all_products.length; i++)
    {
        printf("%d. %s\t%lld\n", i + 1, all_products.products[i].name, all_products.products[i].price);
    }

    // 상품명 입력받기
    while (1) {
        printf("POS /(상품 주문) - 상품명 입력 > ");
        char *input = read_line();
        trim(input);
        remove_all_space(input);
        to_lower(input);
        for (i = 0; i < all_products.length; i++) {
            char comparing[16];
            strcpy(comparing, all_products.products[i].name);
            remove_all_space(comparing);
            to_lower(comparing);
            if (strcmp(input, comparing) == 0) {
                product_to_order = &all_products.products[i];
            }
        }

        if (product_to_order != NULL) {
            break;
        }

        printf("오류 : 상품목록에 없는 상품명입니다. 상품목록에 있는 상품명을 입력해주세요\n");
    }

    // 상품수량 입력받기
    int temp_amount;
    printf("상품을 몇개 주문 하시겠습니까?\n");
    while (1) {
        printf("POS /(상품 주문) - 상품 개수 입력 > ");
        temp_amount = read_amount();
        if (temp_amount == -1) {
            printf("오류 : 개수를 입력해주세요.\n");
        }
        else if (temp_amount == -2) {
            printf("오류 : 개수는 다음과 같은 형식으로 입력할 수 있습니다. <횡공백류열0><개수><횡공백류열0>\n");
        }
        else if (temp_amount == -3) {
            printf("오류 : 개수에 숫자가 아닌 것이 포함되어 있습니다. 개수는 숫자로만 입력할 수 있습니다.\n");
        }
        else if (temp_amount == -4) {
            printf("오류 : 개수의 첫글자가 0입니다. 개수는 0으로 시작할 수 없습니다.\n");
        }
        else if (temp_amount == -5) {
            printf("오류 : 개수는 1이상 20이하의 숫자여야 합니다.\n");
        }
        else 
            break;
    }
    
    while(1) {
        printf("정말로 주문하시겠습니까?\n");
        printf("\t1. 주문\n");
        printf("\t0. 돌아가기\n");
        printf("POS / 상품 주문 - 번호 선택 > ");
        int ret = command_prompt(1);
        if (ret == 0) {
            return;
        } else if (ret == 1) {

            break;
        }
    }

    // 주문목록에 추가
    add_order(table_num, product_to_order, temp_amount);
}

void add_order(int table_num, Product *order_product, int order_quantity) {
    Table *table = &tables[table_num - 1];
    int is_already_existing_order = 0; //테이블 내에 상품이 있는지 확인 .. 있으면 1, 없으면 0
    
    for (int j = 0; j < table->length; j++) {
        char tname[16];
        strcpy(tname, table->products[j].name);
        if (!strcmp(order_product->name, tname)) // 테이블 내에 상품이 이미 있음
        {    //한개 시켰다가 취소해서 0인경우 ?  ... 상관없음
            is_already_existing_order = 1;
            all_products.products[j].amount += order_quantity;
            table->products[j].amount += order_quantity; //주문한 상품의 개수 더해주기
            break;
        }
    }
    if (!is_already_existing_order) {
        if (table->length == 0) {  //주문내역이 아예 비어있음
            if ((table->products = malloc(sizeof(Product))) == NULL) { //메모리 부족으로 malloc 호출 실패하면 종료됨
                printf("오류 : 메모리 문제로 주문에 실패하였습니다. 이전 메뉴로 돌아갑니다.\n");
                return;
            }
            strcpy(table->products[0].name, order_product->name); //상품명 저장
            table->products[0].price = order_product->price; //가격 저장
            table->products[0].amount = order_quantity; //개수 저장
            table->length++; //다음 인덱스를 가리킴
        } else {
            void* realloced = realloc(table->products, (table->length + 1) * sizeof(Product));
            if (realloced == NULL) {
                printf("오류 : 메모리 문제로 주문에 실패하였습니다. 이전 메뉴로 돌아갑니다.\n");
                return;
            } else {
                table->products = realloced;
            }
            strcpy(table->products[table->length].name, order_product->name); //상품명 저장
            table->products[table->length].price = order_product->price; //가격 저장
            table->products[table->length].amount = order_quantity; //개수 저장
            table->length++;
        }
    }
}

void cancel_order(int table_num) {
    Table *table = &tables[table_num - 1];
    int is_existing_order = 0;
    
    while(1) {
        printf("<상품 취소>\n");
        printf("취소하시겠습니까?\n");
        printf("\t1. 상품 취소\n");
        printf("\t0. 돌아가기\n");
        printf("POS / (상품 취소) - 번호 선택 > ");
        int ret = command_prompt(1);
        if (ret == 0) {
            return;
        } else if (ret == 1) {
            break;
        }
    }
    
    print_receipt(table_num);

    int idx = -1;
    while (1) {
        printf("POS / (상품 취소) - 상품명 입력 > ");
        char *input = read_line();
        trim(input);
        remove_all_space(input); // string 을 입력받고, 공백을 다 없애줌
        to_lower(input); //대소문자 오류 수정

        for (int i = 0; i < table->length; i++) {
            char comparing[16];
            strcpy(comparing, table->products[i].name);
            remove_all_space(comparing);
            to_lower(comparing);
            if (!strcmp(input, comparing)) { // 주문내역에 상품이 이미 있음
                 if (table->products[i].amount == 0) {
                     printf("오류 : 주문내역에 없는 상품명입니다. 주문내역에 있는 상품명을 입력해주세요\n");
                 } else {
                     idx = i;
                     is_existing_order = 1;
                     break;
                 }
             }
        }

        if (!is_existing_order) {
            printf("오류 : 주문내역에 없는 상품명입니다. 주문내역에 있는 상품명을 입력해주세요\n");
        } else {
            break;
        }
    }

    int temp_amount;
    printf("상품을 몇개 취소하시겠습니까?\n");
    while (1) {
        printf("POS / 상품 취소 - 상품 개수 입력 > ");
        temp_amount = read_amount();
        if (temp_amount == -1) {
            printf("오류 : 개수를 입력해주세요.\n");
        }
        else if (temp_amount == -2) {
            printf("오류 : 개수는 다음과 같은 형식으로 입력할 수 있습니다. <횡공백류열0><개수><횡공백류열0>\n");
        }
        else if (temp_amount == -3) {
            printf("오류 : 개수에 숫자가 아닌 것이 포함되어 있습니다. 개수는 숫자로만 입력할 수 있습니다.\n");
        }
        else if (temp_amount == -4) {
            printf("오류 : 개수의 첫글자가 0입니다. 개수는 0으로 시작할 수 없습니다.\n");
        }
        else if (temp_amount == -5) {
            printf("오류 : 개수는 1이상 20이하의 숫자여야 합니다.\n");
        }
        else {
            if (temp_amount > table->products[idx].amount) {
                printf("오류 : 취소하는 상품의 개수가 너무 많습니다. 주문된 상품의 개수 이하로 입력할 수 있습니다.\n");
            }
            else
                break;
        }
    }
    
    while(1) {
        printf("정말로 취소하시겠습니까?\n");
        printf("\t1. 취소\n");
        printf("\t0. 돌아가기\n");
        printf("POS / (상품 취소) - 번호 선택 > ");
        
        int ret = command_prompt(1);
        if (ret == 0) {
            return;
        } else if (ret == 1) {
            break;
        }
    }
    
    all_products.products[idx].amount -= temp_amount;
    table->products[idx].amount -= temp_amount; //주문한 상품의 개수 빼주기
    if (table->products[idx].amount == 0) { //상품이 0개가 되면 realloc 후 땡김
        int64_t updateLength = table->length - 1; //새로운 realloc 크기

        for (int64_t i = idx; i < updateLength; i++) { //앞으로 한칸씩 땡김
            table->products[i].amount = table->products[i + 1].amount;
            table->products[i].price = table->products[i + 1].price;
            strcpy(table->products[i].name, table->products[i + 1].name);
        }

        table->products = safe_realloc_trim(table->products, updateLength * sizeof(Product));
//        void* realloced = realloc(table->products, updateLength * sizeof(Product));
//        if (realloced == NULL) {
//            printf("오류 : 메모리 문제로 취소에 실패하였습니다. 이전 메뉴로 돌아갑니다.\n");
//            return;
//        }
//        else {
//            table->products = realloced;
//        }
        table->length--;
    }
}

int is_empty_table(int table_num) {
    Table *table = &tables[table_num - 1];
    int total_order = 0;
    for (int i = 0; i < table->length; i++) {
        total_order += table->products[i].amount;
    }
    if (total_order == 0) {
        return 1;
    } else {
        return 0;
    }
}

void combine_Table(int table_num) {
    while (1) {
        int selected_table;

        while (1) {
            printf("<%d번 테이블>\n", table_num);
            printf("\t1. 테이블 합치기\n");
            printf("\t0. 돌아가기\n");
            printf("POS / (테이블 합치기) - 번호 선택 > ");
            int ret = command_prompt(1);
            if (ret == 0) {
                return;
            } else if (ret == 1) {
                break;
            }
        }

        while(1) {
            printf("<%d번 테이블>\n", table_num);
            printf("가능한 테이블 목록 :\n");
            for (int i = 0; i < table_amount; i++) {
                if ((table_num - 1) == i) continue;
                if (tables[i].status == kOrdinary) {
                    printf("\t%d. %d번 테이블\n", i + 1, i + 1);
                }
            }
            printf("POS / (테이블 합치기) - 테이블 번호 선택 > ");
            selected_table = read_amount();
            
            if (selected_table == -1) {
                printf("오류 : 테이블 번호를 입력해주세요.\n");
            } else if (selected_table == -2) {
                printf("오류 : 테이블 번호는 다음과 같은 형식으로 입력할 수 있습니다. <횡공백류열0><개수><횡공백류열0>\n");
            } else if (selected_table == -3) {
                printf("오류 : 테이블 번호의 숫자가 아닌 것이 포함되어 있습니다. 개수는 숫자로만 입력할 수 있습니다.\n");
            } else if (selected_table == -4) {
                printf("오류 : 테이블 번호의 첫글자가 0입니다. 개수는 0으로 시작할 수 없습니다.\n");
            } else if (selected_table == -5) {
                printf("오류 : 테이블 번호는 1이상 20이하의 숫자여야 합니다.\n");
            } else if (selected_table > table_amount || tables[selected_table - 1].status != kOrdinary || table_num == selected_table) {
                printf("오류 : 가능한 테이블 목록에 있는 숫자를 입력해주세요.\n");
            } else {
                break;
            }
        }

        while (1) {
            printf("정말로 합치시겠습니까?\n");
            printf("\t1. 합치기\n");
            printf("\t0. 돌아가기\n");
            printf("POS / (테이블 합치기) - 번호 선택 > ");
            int ret = command_prompt(1);
            if (ret == 0) {
                return;
            } else if (ret == 1) {
                break;
            }
        }

        Table *currunt_T = &tables[table_num - 1];
        Table *selected_T= &tables[selected_table - 1];

        if (currunt_T->length == 0 && selected_T->length != 0) {
            if (currunt_T->products != NULL) free(currunt_T->products);
            currunt_T->products = selected_T->products;
            currunt_T->length = selected_T->length;
            selected_T->products = NULL;
            selected_T->length = 0;
        } else if (selected_T->length != 0) {
            for (int i = 0; i < selected_T->length; i++) {
                int combined = 0;
                for (int j = 0; j < currunt_T->length && combined == 0; j++) {
                    if (strcmp(currunt_T->products[j].name, selected_T->products[i].name) == 0) {
                        currunt_T->products[j].amount += selected_T->products[i].amount;
                        combined = 1;
                    }
                }
                if (combined == 0) {
                    currunt_T->length += 1;
                    void* realloced = realloc(currunt_T->products, currunt_T->length * sizeof(Product));
                    if (realloced == NULL) {
                        printf("오류 : 메모리 문제로 테이블 합치기에 실패하였습니다.\n");
                        currunt_T->length -= 1;
                        return;
                    } else {
                        currunt_T->products = realloced;
                        strcpy(currunt_T->products[currunt_T->length - 1].name, selected_T->products[i].name);
                        currunt_T->products[currunt_T->length - 1].amount = selected_T->products[i].amount;
                        currunt_T->products[currunt_T->length - 1].price = selected_T->products[i].price;
                    }

                }
            }
            free(selected_T->products);
            selected_T->products = NULL;
            selected_T->length = 0;
        }

        currunt_T->status = kDelegate;
        selected_T->status = kCombined;
        selected_T->delegate = table_num;

        printf("현재 테이블과 합쳐진 테이블 목록:\n");
        printf("\t%d번 테이블\n", table_num);
        for (int i = 0; i < table_amount; i++) {
            if (tables[i].status == kCombined && tables[i].delegate == table_num) {
                printf("\t%d번 테이블\n", i + 1);
            }
        }

        int possible_combine = 0;
        for (int i = 0; i < table_amount && possible_combine == 0; i++) {
            if (i == table_num - 1) continue;
            if (tables[i].status == kOrdinary) possible_combine = 1;
        }
        if (possible_combine == 0) {
            return;
        }

        while (1) {
            printf("계속하시겠습니까?\n");
            printf("\t1. 계속하기\n");
            printf("\t0. 그만두기\n");
            printf("POS / (테이블 합치기) - 번호 선택 > ");
            int ret = command_prompt(1);
            if (ret == 0) {
                return;
            } else if (ret == 1) {
                break;
            }
        }
    }
}
