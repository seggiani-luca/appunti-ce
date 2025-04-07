#include "libce.h"

.global a_debug
.extern c_debug

a_debug: # handler interruzione int3
	salva_registri

	# passa il puntatore alla pila come primo argomento	
	leaq 120(%rsp), %rdi
	call c_debug

	carica_registri
	iretq
