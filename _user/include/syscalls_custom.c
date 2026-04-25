/*
 * Phoenix-RTOS
 *
 * Custom syscall wrappers for ia32-generic-qemu
 *
 * Implements userspace stubs for new syscalls added to the kernel.
 * Uses __attribute__((naked)) to produce bare assembly (no prolog/epilog)
 * identical to what libphoenix/arch/ia32/syscalls.S generates.
 *
 * Stack layout at int $0x80 (cdecl, no frame):
 *   [esp+0] = return address
 *   [esp+4] = arg1, [esp+8] = arg2, ...
 * Kernel reads args via GETFROMSTACK(ustack, ...) from esp.
 *
 * Syscall numbers (0-indexed position in SYSCALLS macro):
 *   setBaseQuanta=100  getBaseQuanta=101
 *   setQuantaForProcess=102  getQuantaForProcess=103
 *   setVariable=104  getVariable=105
 *   maxChildrenInRanges=106
 *
 * Copyright 2024 SyafiraRosa
 */

#include "sys/scheduling.h"
#include "sys/getsetvariable.h"
#include "sys/maxchildreninranges.h"


__attribute__((naked)) int getBaseQuanta(void)
{
	__asm__("movl $101, %eax; int $0x80; ret");
}


__attribute__((naked)) int setBaseQuanta(int quanta)
{
	__asm__("movl $100, %eax; int $0x80; ret");
}


__attribute__((naked)) int getQuantaForProcess(pid_t pid)
{
	__asm__("movl $103, %eax; int $0x80; ret");
}


__attribute__((naked)) int setQuantaForProcess(pid_t pid, int quanta)
{
	__asm__("movl $102, %eax; int $0x80; ret");
}


__attribute__((naked)) int getVariable(void)
{
	__asm__("movl $105, %eax; int $0x80; ret");
}


__attribute__((naked)) void setVariable(int value)
{
	__asm__("movl $104, %eax; int $0x80; ret");
}


__attribute__((naked)) int maxChildrenInRanges(int x, int y, int *pid, int *count)
{
	__asm__("movl $106, %eax; int $0x80; ret");
}


/* Step 28: Group management wrappers (syscall 107=setProcessGroup, 108=getProcessGroup) */
__attribute__((naked)) int setProcessGroup(pid_t pid, int group)
{
	__asm__("movl $107, %eax; int $0x80; ret");
}


__attribute__((naked)) int getProcessGroup(pid_t pid)
{
	__asm__("movl $108, %eax; int $0x80; ret");
}
