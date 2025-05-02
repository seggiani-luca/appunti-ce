#include <libce.h>

.extern c_int
.global a_int
a_int:
	salva_registri
	call c_int
	carica_registri
	iretq
