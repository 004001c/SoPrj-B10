#pragma once

typedef struct _product {
    char name[15]; // ��ǰ ��
    int price; // ����
    int sales; // ���� �� �Ǹ� ����
} Product;

typedef struct _all_product {
    Product* list; // ��� ��ǰ���� ������ Array
    int list_size; // Array Size
} All_Product;

typedef struct _table {
    Product* list; // malloc ���� �迭 �Ҵ� �� �߰� �ֹ����� realloc
    int list_size; // �ֹ��� ��ǰ ����
} Table;