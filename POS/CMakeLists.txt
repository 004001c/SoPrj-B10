add_library(
	core
	STATIC
	add_product_prompts.h
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
	add_product_prompts.c
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
)

target_include_directories(core INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(core PUBLIC dirent)
set_property(TARGET core PROPERTY FOLDER POS)
