//
//  settlement.c
//  POS
//
//  Created by 정준규 on 2023/04/25.
//

#include "settlement.h"
#include "utils.h"
#include "types.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

extern int table_amount;
extern Product_Array all_products;
extern char base_dir[FILENAME_MAX];
extern int date;

int save_settlement() {
    FILE* fp;
    int sum = 0;

    char datafile_dir[FILENAME_MAX];
    sprintf(datafile_dir ,"%s%08d", base_dir, date);

    fp = fopen(datafile_dir, "w");
    if (fp == NULL) {
        printf("존재하지 않는 파일");
        return -1;
    }
    else {
        for (int i = 0; i < all_products.length; i++) {
            sum += all_products.products[i].price * all_products.products[i].amount;
        }
        fprintf(fp, "%08d\t%d\n", date, sum);
        for (int i = 0; i < all_products.length; i++) {
            fprintf(fp, "%s\t%lld\t%lld\t%lld\n", all_products.products[i].name, all_products.products[i].price, all_products.products[i].amount, all_products.products[i].price * all_products.products[i].amount);
        }
        fclose(fp);
        return 0;
    }
}


void print_settlement(char date[]) {
    FILE* fp;
    char c;

    int day = checkDate(date);
    
    
    if (day == -1) {
        printf("오류 : YYYYMMDD 혹은 YYMMDD형식으로 작성해주세요.\n");
        return;
    }
    else if (day == -2) {
        printf("오류 : 그레고리력에 존재하지 않는 날짜입니다.\n");
        return;
    }
    char datafile_dir[FILENAME_MAX];
    sprintf(datafile_dir ,"%s%08d", base_dir, day);

    fp = fopen(datafile_dir, "r");
    if (fp == NULL) {
        printf("해당 일자의 정산 파일이 존재하지 않습니다.\n");
        return;
    }
    else {
        if (read_settlement_file(datafile_dir) == 0) {
            if ((c = fgetc(fp)) == EOF) {
                printf("오류 : 정산파일에 정산내역이 존재하지 않습니다.\n");
                return;
            }
            putchar(c);
            while ((c = fgetc(fp)) != EOF) {
                putchar(c);
            }
            fclose(fp);
            printf("\n%s의 정산내역을 출력하였습니다.\n", date);
        }
        return;
    }
}

//int read_settlement_file2(char date_input[]) {
//    return 0;
//}

int read_settlement_file(char date_input[]) {
    int c;
    int err = 0;
    FILE* fp = fopen(date_input, "r");
    fpos_t p;

    fgetpos(fp, &p);
    
    switch(read_settlement_line(fp, date_input)){
    case -1:
            err = 1;
            fsetpos(fp, &p);
            printf("오류 : 첫 번째 행은 항상 정산레코드 형식에 맞아야합니다.\n");
            while ((c = fgetc(fp)) != '\n') {
                if (c == EOF) {
                    fclose(fp);
                    return -1;
                }
                putchar(c);
            }
            putchar(c);
            break;
    case -2:
            printf("오류 : 정산파일에 정산내역이 존재하지 않습니다.\n");
        return -1;
    }

    while (1) {
        fgetpos(fp, &p);//잘못된 입력 출력하기 위해
        switch (c = read_settlement_product_line(fp)) {//각 행을 읽어서 올바른 입력인지 파악
        case 2:
            fclose(fp);
            if (err)
                return -1;
            return 0;

        case 1: // 정상 레코드
        case 0: // 횡공 백류열
            break;

        case -1:
            err = 1;
            fsetpos(fp, &p);
            printf("오류 : 레코드 형식이 올바르지 않습니다.\n");
            while ((c = fgetc(fp)) != '\n') {
                if (c == EOF) {
                    fclose(fp);
                    return -1;
                }
                putchar(c);
            }
            putchar(c);
            break;

        case -2:
            err = 1;
            fsetpos(fp, &p);
            printf("오류 : 상품명이 올바르지 않습니다.\n");
            while ((c = fgetc(fp)) != '\n') {
                if (c == EOF) {
                    fclose(fp);
                    return -1;
                }
                putchar(c);
            }
            putchar(c);
            break;

        case -3:
            err = 1;
            fsetpos(fp, &p);
            printf("오류 : 상품가격이 올바르지 않습니다\n");
            while ((c = fgetc(fp)) != '\n') {
                if (c == EOF) {
                    fclose(fp);
                    return -1;
                }
                putchar(c);
            }
            putchar(c);
            break;

        case -4:
            err = 1;
            fsetpos(fp, &p);
            printf("오류 : 상품갯수가 올바르지 않습니다.\n");
            while ((c = fgetc(fp)) != '\n') {
                if (c == EOF) {
                    fclose(fp);
                    return -1;
                }
                putchar(c);
            }
            putchar(c);
            break;

        case -5:
            err = 1;
            fsetpos(fp, &p);
            printf("오류 : 상품정산가격이 올바르지 않습니다.\n");
            while ((c = fgetc(fp)) != '\n') {
                if (c == EOF) {
                    fclose(fp);
                    return -1;
                }
                putchar(c);
            }
            putchar(c);
            break;
        }
    }
}

int read_settlement_line(FILE* fp, char date_input[]) {
    int length;
    char c;
    int price = 0;
    char checkdate[9], date[9];
    int day, checkday;
    memset(date, '\0', sizeof(date));
    memset(checkdate, '\0', sizeof(checkdate));

    c = getc(fp);
    if(c == EOF)
        return -2;
    ungetc(c,fp);
    
    for (length = 0; length < 8; length++) { //최대 8글자까지 읽음
        c = getc(fp);
        if (isdigit(c)) { //날짜는 숫자
            date[length] = c;
        }

        else { //숫자가 아니라면 에러
            return -1;
        }
    }
    
    
    for(int i=7;i>=0;i--){
        checkdate[i]=date_input[strlen(date_input)-(8-i)];
    }
    checkday = atoi(checkdate);
    day = atoi(date);
        if (day!=checkday)
            return -1; // 파일 이름과 정산레코드의 날짜 부분은 같아야함
    if ((c = getc(fp)) != '\t')
        return -1;

    for (length = 0; length < 10; length++) {
        c = getc(fp);
        if (isdigit(c)) { //상품가격은 숫자
            price *= 10;
            price += c - '0';
        }
        else
            break;
    }

    if (length == 9) // 길이가 10이 되었는데
        if (isdigit(c))//마지막 문자가 숫자이면 길이 초과
            return -1;

    if (price % 100)//가격이 100단위가 아님
        return -1;
    if (c == '\n' || c == EOF) {
        return 0;
    }
    return -1;

}

int read_settlement_product_line(FILE* fp) {
    int length;
    char c, name[16];
    int price = 0;

    c = getc(fp);

    if (c == '\n')//횡공백류열만 입력한 행
        return 0;

    if (c == EOF)//파일 끝
        return 2;

    if (!isalpha(c))//상품명에 알파벳이 아닌 글자 포함
        return -2;
    ungetc(c, fp);

    for (length = 0; length < 16; length++) {//최대 15글자까지 읽음
        c = getc(fp);

        if (c == '\t') { // \t읽으면 break
            break;
        }

        if (isalpha(c) || c == ' ') { //상품명은 알파벳 or 표준공백
            name[length] = c;
        }

        else { //위의 둘이 아니라면 에러
            return -2;
        }
    }


    if (c != '\t')
        return -1;



    for (length = 0; length < 7; length++) {
        c = getc(fp);
        if (isdigit(c)) { //상품가격은 숫자
            price *= 10;
            price += c - '0';
        }
        else
            break;
    }

    if (c != '\t')
        return -1;

    if (price % 100)//가격이 100단위가 아님
        return -3;


    for (length = 0; length < table_amount; length++) {
        c = getc(fp);
        if (!isdigit(c)) { //상품개수는 숫자
            break;
        }
    }

    if (c != '\t')
        return -1;

    for (length = 0; length < 10; length++) {
        c = getc(fp);
        if (isdigit(c)) { //상품가격은 숫자
            price *= 10;
            price += c - '0';
        }
        else
            break;
    }

    if (price % 100)//가격이 100단위가 아님
        return -5;

    if (c == '\n' || c == EOF) {
        return 0;
    }
    return -1;
}
