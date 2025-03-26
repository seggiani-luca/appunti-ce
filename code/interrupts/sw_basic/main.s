#include <libce.h>

.global a_handler
.extern c_handler

a_handler:
	salva_registri
	call c_handler
	carica_registri
	iretq
