#include <libce.h>

.global a_keyboard
.extern c_keyboard

a_keyboard:
	salva_registri
	call c_keyboard
	carica_registri
	iretq
