#include <libce.h>

char saved_byte;
char* saved_byte_addr;

void foo() {
	printf("foo e' in esecuzione\n\n");
}

// questa funzione interrompe all'int3
extern "C" void a_debug(); 
extern "C" void c_debug(void** p) {
	// p contiene il puntatore a %rsp
	
	// *p e' %rip, decrementa (vogliamo ripartire da rip - 1, e questo sara' 
	// l'indirizzo salvato nello stack che iretq andra' a riprenersi)
	(*p)--;
	
	// prende il vecchio %rip come indirizzo del byte salvato
	saved_byte_addr = reinterpret_cast<char*>(*p);

	pause();

	// poi rimettette tutto a posto
	*saved_byte_addr = saved_byte;
}

// questa funzione interrompe al single step
extern "C" void a_sstep(); 
extern "C" void c_sstep() {
	// saved_byte_conterra' il byte che abbiamo reinserito
	
	// rimette 0xcc (int3)
	*saved_byte_addr = 0xcc;
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
	// inizializza il gate single_step() con a_sstep() 
	gate_init(1, a_sstep); 

	// aggiungi il breakpoint
	add_breakpoint(foo);

	// qui arresta
	foo();
	
	// qui pure
	foo();
	
	pause();
}
