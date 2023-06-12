#ifndef sell_prompts_h
#define sell_prompts_h

#include "types.h"

void sell_prompt(void);

void table_management_prompt(int table_num);

void print_receipt(int table_num);

void order_product(int table_num);

void add_order(int table_num, Product *order_product, int order_quantity);

void cancel_order(int table_num);

void process_payment(int table_num);

int is_empty_table(int table_num);

void combine_Table(int table_num);

#endif
