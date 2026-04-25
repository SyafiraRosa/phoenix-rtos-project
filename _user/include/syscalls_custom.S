/*
 * Phoenix-RTOS
 *
 * Custom syscall wrappers for ia32-generic-qemu
 * Implements userspace stubs for new syscalls added to the kernel.
 *
 * These wrappers mirror the pattern from libphoenix/arch/ia32/syscalls.S
 * using the same int $0x80 mechanism and __COUNTER__-based numbering.
 *
 * Syscall numbers (position in SYSCALLS macro, 0-indexed):
 *   setBaseQuanta        = 100
 *   getBaseQuanta        = 101
 *   setQuantaForProcess  = 102
 *   getQuantaForProcess  = 103
 *   setVariable          = 104
 *   getVariable          = 105
 *   maxChildrenInRanges  = 106
 *
 * Copyright 2024 SyafiraRosa
 */

.text

#define SYSCALLDEF(sym, num)   \
.globl sym;                    \
.type sym, @function;          \
.align 4, 0x90;                \
sym:                           \
	.cfi_startproc;            \
	mov $num, %eax;            \
	int $0x80;                 \
	ret;                       \
	.cfi_endproc;              \
.size sym, .-sym

SYSCALLDEF(setBaseQuanta,       100)
SYSCALLDEF(getBaseQuanta,       101)
SYSCALLDEF(setQuantaForProcess, 102)
SYSCALLDEF(getQuantaForProcess, 103)
SYSCALLDEF(setVariable,         104)
SYSCALLDEF(getVariable,         105)
SYSCALLDEF(maxChildrenInRanges, 106)
