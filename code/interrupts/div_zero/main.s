.global a_divzero
a_divzero:
	// non abbiamo bisogno di salvare o caricare registr
	mov (%rsp), %rdi // restituisci IP
	call c_divzero
	iretq
