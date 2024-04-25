#include "list2.h"

list2_node<list2<record>>* find_place(list2<list2<record>>* huge_list, int n) {
	list2_node<list2<record>>* answer = huge_list->get_head();
	while (answer && answer->get_next() && answer->get_amount_of_ones() < n) {
		answer = answer->get_next();
	}
	if (answer && answer == huge_list->get_tail() && answer->get_amount_of_ones() < n) {
		return nullptr;
	}
	return answer;
}

io_status read_function(list2<list2<record>>* huge_list, FILE* file) {
	char buf[LEN];
	if (!fgets(buf, LEN, file)) {
		printf("Cannot read line\n");
		return io_status::format;
	}
	if (!feof(file)) {
		printf("File contains smth apart from line\n");
		return io_status::format;
	}
	int buf_len = (int)strlen(buf);
	if (buf[buf_len] == '\n') {
		buf[buf_len] = 0;
		buf_len--;
	}
	if (buf[buf_len] == '\r') {
		buf[buf_len] = 0;
		buf_len--;
	}
	int possible_n = 1;
	while ((possible_n < max_variables) && (1 << possible_n < buf_len)) {
		possible_n++;
	}
	if (1 << possible_n != buf_len) {
		printf("Length of line should be some power of 2\n");
		return io_status::format;
	}
	int curr_amount_of_ones = 0;
	for (int i = 0; i < buf_len; i++) {
		if (buf[i] == '1') {
			list2_node<record>* curr = new list2_node<record>;
			if (!curr) {
				return io_status::memory;
			}
			curr->set_length(possible_n);
			int* val = new int[possible_n];
			if (!val) {
				delete curr;
				return io_status::memory;
			}
			curr->set_values(val);
			curr_amount_of_ones = 0;
			for (int j = 0; j < possible_n; j++) {
				if ((1 << j & i) == 1 << j) {
					curr_amount_of_ones++;
					val[possible_n - 1 - j] = 1;
				}
				else {
					val[possible_n - 1 - j] = 0;
				}
			}
			list2_node<list2<record>>* place = find_place(huge_list, curr_amount_of_ones);
			if (!place) {
				list2_node<list2<record>>* new_place = new list2_node<list2<record>>;
				if (!new_place) {
					delete curr;
					return io_status::memory;
				}
				huge_list->add_node(new_place);
				new_place->add_node(curr);
				new_place->set_amount_of_ones(curr_amount_of_ones);
			}
			else if (place->get_amount_of_ones() == curr_amount_of_ones) {
				place->add_node(curr);
			}
			else if (place->get_amount_of_ones() > curr_amount_of_ones) {
				list2_node<list2<record>>* new_place = new list2_node<list2<record>>;
				if (!new_place) {
					delete curr;
					return io_status::memory;
				}
				huge_list->insert_node(new_place->get_prev(), new_place);
				new_place->add_node(curr);
				new_place->set_amount_of_ones(curr_amount_of_ones);
			}
		}
		else if (buf[i] != '0') {
			printf("Line should contain 0 or 1 only\n");
			return io_status::format;
		}
	}
	return io_status::success;
}


list2_node<record>* merge_conj(list2_node<record>* conj1, list2_node<record>* conj2) {
	list2_node<record>* new_conj = new list2_node<record>;
	if (!new_conj) {
		return nullptr;
	}
	new_conj->set_length(conj1->get_length());
	int* val = new int[conj1->get_length()];
	if (!val) {
		delete new_conj;
		return nullptr;
	}
	new_conj->set_values(val);
	bool diff = false;
	int * values1 = conj1->get_values(), * values2 = conj2->get_values();
	for (int i = 0; i < conj1->get_length(); i++) {
		if (values1[i] != values2[i]) {
			if (values1[i] == STAR || values2[i] == STAR) {
				delete new_conj;
//				printf("* vs 1/0\n");
				return nullptr;
			}
			else {
				if (diff) {
//					printf("Too much difference\n");
					delete new_conj;
					return nullptr;
				}
				else {
					new_conj->get_values()[i] = STAR;
					diff = true;
				}
			}
		}
		else {
			new_conj->get_values()[i] = values1[i];
		}
	}
//	new_conj->print();
	return new_conj;
}

bool conj_equal(list2_node<record>* conj1, list2_node<record>* conj2) {
	int* values1 = conj1->get_values(), * values2 = conj2->get_values();
	for (int i = 0; i < conj1->get_length(); i++) {
		if (values1[i] != values2[i]) {
			return false;
		}
	}
	return true;
}

void add_node_no_repeat(list2<record>* group, list2_node<record>* new_conj) {
	list2_node<record>* curr = group->get_head();
	while (curr) {
		if (conj_equal(new_conj, curr)) {
//			printf("\nAlready has\n");
//			curr->print();
			return;
		}
		curr = curr->get_next();
	}
	group->add_node(new_conj);
}


io_status merge_groups(list2<record>* group1, list2<record>* group2, list2<record>* new_group, list2<record>* conjuctions, bool& new_group_used) {
	if (!group1 || !group2 || !conjuctions) {
		return io_status::memory;
	}
	list2_node<record>* conj1 = group1->get_head(), * conj2 = group2->get_head(), * air_prev = nullptr, * air_next = nullptr, * future_next = nullptr, * another_future_next = nullptr;
	while (conj1) {
		another_future_next = conj1->get_next();
		conj2 = group2->get_head();
		while (conj2) {
//			printf("\nMerging:\n");
//			conj1->print();
//			conj2->print();
			future_next = conj2->get_next();
			list2_node<record>* new_conj = merge_conj(conj1, conj2);
			if (new_conj) {
				add_node_no_repeat(new_group, new_conj);
				new_group_used = true;
//				printf("\nCurr state of new group\n");
//				new_group->print();
				conj1->mark();
				conj2->mark();
			}
			conj2 = future_next;
		}
		conj1 = another_future_next;
	}
//	printf("\nNew group:\n");
//	new_group->print();
	return io_status::success;
}

io_status merge_lists(list2<list2<record>>* old_huge_list, list2<list2<record>>* new_huge_list, list2<record>* conjuctions, bool& new_huge_list_used) {
	if (!old_huge_list || !conjuctions) {
		return io_status::memory;
	}
	list2<record>* group1 = old_huge_list->get_head();
	if (!group1) {
		return io_status::success;
	}
	list2_node<list2<record>>* group2 = old_huge_list->get_nth(1), * future_group = nullptr;
	io_status res = io_status::success;
	bool new_group_used = false;
	while (group2) {
		future_group = group2->get_next();
		list2_node<list2<record>>* new_group = new list2_node<list2<record>>;
		if (!new_group) {
			return io_status::memory;
		}
		new_group->set_amount_of_ones(group1->get_amount_of_ones());
		new_group_used = false;
		res = merge_groups(group1, group2, new_group, conjuctions, new_group_used);
		if (res != io_status::success) {
			return res;
		}
		if (new_group_used) {
			new_huge_list->add_node(new_group);
			new_huge_list_used = true;
		}
		else {
			delete new_group;
		}
		group1 = group2;
		group2 = future_group;
	}
	list2_node<record>* conj1 = group1->get_head(), * air_prev = nullptr, * air_next = nullptr, * future_next = nullptr;
	while (conj1) {
		future_next = conj1->get_next();
		conj1 = future_next;
	}
	return io_status::success;
}

io_status do_total_merge(list2<list2<record>>* huge_list, list2<record>* conjuctions) {
	if (!huge_list || !conjuctions) {
		return io_status::memory;
	}
	list2<list2<record>>* old_huge_list = huge_list, * new_huge_list = nullptr;
	bool new_huge_list_used = false;
	while (old_huge_list && old_huge_list->get_head() && old_huge_list->get_head()->get_head()) {
//		old_huge_list->get_head()->print();
		new_huge_list = nullptr;
		new_huge_list = new list2<list2<record>>;
		if (!new_huge_list) {
			return io_status::memory;
		}
		new_huge_list_used = false;
		io_status res = merge_lists(old_huge_list, new_huge_list, conjuctions, new_huge_list_used);
		if (res != io_status::success) {
			if (huge_list) {
				delete huge_list;
				delete new_huge_list;
			}
			return res;
		};
//		printf("\nOld list:\n");
//		old_huge_list->print();
//		printf("\nNew list:\n");
//		new_huge_list->print();
		if (!new_huge_list_used) {
			delete new_huge_list;
			new_huge_list = nullptr;
		}
		if (old_huge_list) {
			list2_node<list2<record>>* curr_group = old_huge_list->get_head();
			list2_node<record>* curr_record = nullptr, * air_prev = nullptr, * air_next = nullptr, * future_next = nullptr;
			while (curr_group) {
				curr_record = curr_group->get_head();
				while (curr_record) {
					future_next = curr_record->get_next();
					if (!curr_record->marked()) {
						air_prev = curr_record->get_prev();
						air_next = curr_record->get_next();
						if (air_prev) {
							curr_record->get_prev()->set_next(air_next);
						}
						else {
							curr_group->set_head(air_next);
						}
						if (air_next) {
							curr_record->get_next()->set_prev(air_prev);
						}
						else {
							curr_group->set_tail(air_prev);
						}
						if (!air_prev && !air_next) {
							curr_group->set_head(nullptr);
							curr_group->set_tail(nullptr);
						}
						curr_group->set_length(curr_group->get_length() - 1);
						conjuctions->add_node(curr_record);
					}
					else {
						curr_record->unmark();
					}
					curr_record = future_next;
				}
				curr_group = curr_group->get_next();
			}
		}
//		printf("\nSaved conjunctions:\n");
//		conjuctions->print();
		delete old_huge_list;
		old_huge_list = new_huge_list;
	}
	printf("\nSaved conjunctions:\n");
	conjuctions->print();
	if (old_huge_list) {
		delete old_huge_list;
	}
	if (new_huge_list) {
		delete new_huge_list;
	}
	return io_status::success;
}