#include <iostream>

struct node {
	int inf;
	node* next;

	node(int i) : inf(i), next(nullptr) {} 
};

void insert(node* nu, node*& list) {
	node** n_ptr = &list;
	while(*n_ptr) {
		n_ptr = &(*n_ptr)->next;
	}
	*n_ptr = nu;
}

void print(node* list) {
	while(list) {
		std::cout << list->inf << " ";
		list = list->next;
	}
	std::cout << std::endl;
}

int main() {
	node* list = nullptr;
	node* node1 = new node(1);
	node* node2= new node(42);

	print(list);

	insert(node1, list);
	print(list);
	
	insert(node2, list);
	print(list);

	delete node1;
	delete node2;

	return 0;
}
