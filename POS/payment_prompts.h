#ifndef payment_prompts_h
#define payment_prompts_h

void process_payment(int table);

void pay_all_at_once(int table);

void pay_with_ratio(int table);

void pay_partially(int table);

// 현재 테이블의 총 주문금액을 반환
int get_total_price(int table);

// 입력받은 비율에 따른 개개인 결제금액 계산
void calculate_ratio(int tablenum, int number_of_people, int ratio[], int pay_individual_param[]);

// 테이블 내 "전체" 상품 결제 완료 후
// 전역 allproduct 결제수량 더해주고 테이블 주문내역 초기화
void end_purchase(int table);

// 입력된 상품들 부분적으로 결제 후 결제된 금액 혹은
// 오류 발생시 오류 코드를 반환
int partial_pay(int table, char *string);

void uncombine_Table(int table_num);
#endif
