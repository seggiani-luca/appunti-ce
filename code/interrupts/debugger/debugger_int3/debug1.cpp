#include <libce.h>

// conterra' il bit da salvare
char saved_byte;

void foo() {
	printf("sono la funzione foo\n");
}

// questa funzione interrompe all'int3
extern "C" void a_debug();
extern "C" void c_debug(void** p) {
	// p contiene il puntatore a %rsp
	
	// mette in pausa
	pause();

	// poi rimette tutto a posto
	auto func_p = reinterpret_cast<char**>(p);
	--(*func_p); // e' l'istruzione precedente
	**func_p	= saved_byte;
}


// questa funzione mette il breakpoint
void add_breakpoint(void (*func)(void)) {
	// salva in saved_byte
	auto func_p = reinterpret_cast<char*>(func);
	saved_byte = *func_p;
	printf("saved_byte: %x\n", saved_byte);

	// al suo posto mette 0xcc (int3)
	*func_p = 0xcc;
}

extern "C" void main() {
	// inizializza il gate int3 con a_debug()
	gate_init(3, a_debug); 

	// aggiungi il breakpoint
	add_breakpoint(foo);

	// qui arresta
	foo();
	// qui no
	foo();

	pause();
}
