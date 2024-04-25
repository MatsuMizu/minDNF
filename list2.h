#include "condition.h"

enum class io_status
{
	success,
	eof,
	format,
	memory,
	open,
	create,
	empty,
};
class record
{
private:
	int* values = 0;
	int length = 0;
public:
	record() = default;
	~record() {
		if (values) {
			delete[] values;
		}
		values = nullptr;
		length = 0;
	}
	io_status init(int n);
	record(record&& x) = default;
	record& operator= (record&& x) = default;
	record(const record& x) = delete;
	record& operator= (const record&) = delete;
	void print(ordering print_order[] = 0, FILE* fp = stdout) {
		for (int i = 0; i < length; i++) {
			if (values[i] == STAR) {
				printf("* ");
			}
			else {
				printf("%d ", values[i]);
			}
		}
		printf("\n");
	}
	void set_length(int n) { length = n; }
	void set_values(int* val) { values = val; }
	int get_length() { return length; }
	int* get_values() { return values; }
	io_status read(FILE* fp = stdin);
	template <class T>
	friend class list2_node;
};

template <class T> class list2;
template <class T>
class list2_node : public T
{
private:
	list2_node* next = nullptr;
	list2_node* prev = nullptr;
	bool mark_place = false;
public:
	list2_node() = default;
	list2_node(const list2_node& x) = delete;
	list2_node(list2_node&& x) : T((T&&)x) {
		next = x.next;
		prev = x.prev;
		x.next = nullptr;
		x.prev = nullptr;
		mark_place = x.mark_place;
	}
	~list2_node() {
		next = nullptr;
		prev = nullptr;
		mark_place = false;
	}
	list2_node& operator= (const list2_node& x) = delete;
	list2_node& operator= (list2_node&& x)
	{
		next = x.next;
		prev = x.prev;
		mark_place = x.mark_place;
		x.next = nullptr;
		x.prev = nullptr;
		return *this;
	}
	void mark() {
		mark_place = true;
	}
	void unmark() {
		mark_place = false;
	}
	bool marked() { return mark_place; }
	list2_node* get_next() const {
		return this->next;
	}
	list2_node* get_prev() const {
		return this->prev;
	}
	void set_next(list2_node* pot_next) {
		this->next = pot_next;
	}
	void set_prev(list2_node* pot_prev) {
		this->prev = pot_prev;
	}
	friend class list2<T>;
};
template <class T>
class list2
{
private:
	list2_node<T>* head = nullptr;
	list2_node<T>* tail = nullptr;
	unsigned int length = 0;
	int amount_of_ones = -1;
public:
	list2() = default;
	list2(const list2<T>& x) = delete;
	list2& operator= (const list2<T>& r) = delete;
	~list2() {
		erase_list();
	}
	list2_node<T>* get_head() { return head; }
	list2_node<T>* get_tail() { return tail; }
	list2_node<T>* get_nth(int n) {
		if (n >= length) return nullptr;
		list2_node<T>* curr = head;
		for (int i = 0; i < n && curr; i++) {
			curr = curr->next;
		}
		return curr;
	}
	unsigned int get_length() { return length; }
	void set_head(list2_node<T>* x) { head = x; }
	void set_tail(list2_node<T>* x) { tail = x; }
	void set_length(unsigned int x) { length = x; }
	void print(unsigned int r = 10, FILE* fp = stdout) {
		unsigned int i = 0;
		list2_node<T>* curr = nullptr;
		(void)fp;
		if (!head) return;
		if (amount_of_ones != -1) {
			printf("Include %d ones:\n", amount_of_ones);
		}
		for (curr = head; curr; curr = curr->get_next(), i++) {
			curr->print();
			fprintf(fp, "\n");
		}
	}
	void set_amount_of_ones(int i) { amount_of_ones = i; }
	int get_amount_of_ones() { return amount_of_ones; }
	void erase_list() {
		list2_node<T>* curr = nullptr, * next = nullptr;
		if (head) {
			for (curr = head; curr; curr = next) {
				next = curr->next;
				delete curr;
			}
		}
		head = nullptr;
		tail = nullptr;
		length = 0;
	}
	list2_node<T>* delete_node(list2_node<T>* curr) {
		if (!curr) return nullptr;
		if (curr == head) {
			head = curr->next;
		}
		if (curr == tail) {
			tail = curr->prev;
		}
		list2_node<T>* before = curr->prev;
		list2_node<T>* after = curr->next;
		if (before) {
			before->next = after;
		}
		if (after) {
			after->prev = before;
		}
		delete curr;
		return after;
	}
	void add_node(list2_node<T>* curr) {
		length++;
		if (!head) {
			head = curr;
			tail = curr;
			curr->next = nullptr;
			curr->prev = nullptr;
			return;
		}
		tail->next = curr;
		curr->prev = tail;
		curr->next = nullptr;
		tail = curr;
	}
	void insert_node(list2_node<T>* place, list2_node<T>* new_node) {
		if (!place || place == tail) {
			add_node(new_node);
		}
		length++;
		list2_node<T>* after_place = place->next;
		place->next = new_node;
		after_place->prev = new_node;
		new_node->prev = place;
		new_node->next = after_place;
	}
};

