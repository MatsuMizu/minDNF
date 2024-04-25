#include "methods.h"



int main(int argc, char* argv[]) {
	double time = 0;
	char* file_STU = 0;
	if (!(argc == 2)) {
		printf("Usage %s: file\n", argv[0]);
		return ERROR_INPUT;
	}
	file_STU = argv[1];
	FILE* STU;
	STU = fopen(file_STU, "r+");
	if (!STU) {
		printf("Cannot open %s\n", file_STU);
		return ERROR_OPEN;
	}
	list2<list2<record>>* huge_list = new list2<list2<record>>; // includes records divided into several lists depending on amount of 1 in a record
	if (!huge_list) {
		printf("Not enough memory!\n");
		fclose(STU);
		return ERROR_MEMORY;
	}
	list2<record>* conjuctions = new list2<record>;
	if (!conjuctions) {
		printf("Not enough memory!\n");
		delete huge_list;
		fclose(STU);
		return ERROR_MEMORY;
	}
	io_status reading_res = read_function(huge_list, STU);
	if (reading_res != io_status::success) {
		delete huge_list;
		delete conjuctions;
		fclose(STU);
		return ERROR_READ;
	}
	huge_list->print();
	io_status merge_res = do_total_merge(huge_list, conjuctions);
	if (reading_res != io_status::success) {
		delete conjuctions;
		fclose(STU);
		return ERROR_MEMORY;
	}
	fclose(STU);
	delete conjuctions;
	return SUCCESS;
}
