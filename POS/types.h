#ifndef types_h
#define types_h

#include <stdint.h>

typedef struct _product {
    char name[16]; // 상품 명, 15 글자 + \0
    int64_t price; // 가격
    int64_t amount; // 금일 총 판매 갯수
} Product;

typedef struct _prodcut_arr {
    Product* products; // malloc 으로 배열 할당 후 추가시마다 realloc
    int64_t length; // Array 크기
} Product_Array;

typedef enum _table_status {
    kOrdinary,
    kDelegate,
    kCombined
} TableStatus;

typedef struct _table {
    Product* products; // malloc 으로 배열 할당 후 추가시마다 realloc
    int64_t length; // Array 크기
    TableStatus status; // 테이블의 현재 상태
    int delegate; // 합쳐진 경우 대표 태이블 번호
} Table;
#endif
