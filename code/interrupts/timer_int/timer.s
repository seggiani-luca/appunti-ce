#include <libce.h>

.extern c_handler
.global a_handler
a_handler:
	salva_registri
	call c_handler
	carica_registri
	iretq
