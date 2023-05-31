/*
 * Copyright (C) 2023 Kian Cross
 */

#include "../svc.h"

.global	eln0_svc_puts
.type	eln0_svc_puts,	"function"
eln0_svc_puts:
	mov	x8, #SVC_ID_PUTS
	svc	#1
	ret
