add_library(
	core
	STATIC
	product_management_prompts.c
	login.h
	main_prompts.h
	payment_prompts.h
	pos_main.h
	product.h
	sell_prompts.h
	settlement.h
	settlement_prompts.h
	types.h
	utils.h
	common_prompts.h
	table_amount_prompt.h
	product_management_prompts.h
	login.c
	pos_main.c
	main_prompts.c
	payment_prompts.c
	product.c
	sell_prompts.c
	settlement.c
	settlement_prompts.c
	utils.c
	common_prompts.c
	table_amount_prompt.c
)

target_include_directories(core INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(core PUBLIC dirent)
set_property(TARGET core PROPERTY FOLDER POS)
