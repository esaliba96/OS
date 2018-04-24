	.file	"lwp.c"
	.globl	pid
	.bss
	.align 4
	.type	pid, @object
	.size	pid, 4
pid:
	.zero	4
	.comm	context_to_add,32,32
	.comm	my_sched,8,8
	.comm	orginal_stack_pointer,8,8
	.comm	lwp_ptable,960,32
	.comm	lwp_procs,4,4
	.comm	lwp_running,4,4
	.globl	stopped
	.align 4
	.type	stopped, @object
	.size	stopped, 4
stopped:
	.zero	4
	.text
	.globl	new_lwp
	.type	new_lwp, @function
new_lwp:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movl	lwp_procs(%rip), %eax
	cmpl	$30, %eax
	jle	.L2
	movl	$-1, %eax
	jmp	.L3
.L2:
	movl	lwp_procs(%rip), %eax
	movl	%eax, lwp_running(%rip)
	movl	lwp_procs(%rip), %eax
	addl	$1, %eax
	movl	%eax, lwp_procs(%rip)
	movq	-56(%rbp), %rax
	salq	$3, %rax
	movq	%rax, %rdi
	call	malloc
	movq	%rax, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, context_to_add+8(%rip)
	movl	pid(%rip), %eax
	cltq
	movq	%rax, context_to_add(%rip)
	movq	-56(%rbp), %rax
	movq	%rax, context_to_add+16(%rip)
	movl	pid(%rip), %eax
	addl	$1, %eax
	movl	%eax, pid(%rip)
	movq	-56(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -16(%rbp)
	subq	$8, -16(%rbp)
	movq	-48(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, (%rax)
	subq	$8, -16(%rbp)
	movl	$lwp_exit, %edx
	movq	-16(%rbp), %rax
	movq	%rdx, (%rax)
	subq	$8, -16(%rbp)
	movq	-40(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, (%rax)
	subq	$8, -16(%rbp)
	movq	-16(%rbp), %rax
	movq	$699050, (%rax)
	movq	-16(%rbp), %rax
	movq	%rax, -8(%rbp)
	subq	$56, -16(%rbp)
	movq	-8(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-16(%rbp), %rax
	movq	%rax, context_to_add+24(%rip)
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable, %rax
	movq	context_to_add(%rip), %rdx
	movq	%rdx, (%rax)
	movq	context_to_add+8(%rip), %rdx
	movq	%rdx, 8(%rax)
	movq	context_to_add+16(%rip), %rdx
	movq	%rdx, 16(%rax)
	movq	context_to_add+24(%rip), %rdx
	movq	%rdx, 24(%rax)
	movl	pid(%rip), %eax
.L3:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	new_lwp, .-new_lwp
	.globl	lwp_stop
	.type	lwp_stop, @function
lwp_stop:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
#APP
# 50 "lwp.c" 1
	pushq %rax
# 0 "" 2
# 50 "lwp.c" 1
	pushq %rbx
# 0 "" 2
# 50 "lwp.c" 1
	pushq %rcx
# 0 "" 2
# 50 "lwp.c" 1
	pushq %rdx
# 0 "" 2
# 50 "lwp.c" 1
	pushq %rsi
# 0 "" 2
# 50 "lwp.c" 1
	pushq %rdi
# 0 "" 2
# 50 "lwp.c" 1
	pushq %r8
# 0 "" 2
# 50 "lwp.c" 1
	pushq %r9
# 0 "" 2
# 50 "lwp.c" 1
	pushq %r10
# 0 "" 2
# 50 "lwp.c" 1
	pushq %r11
# 0 "" 2
# 50 "lwp.c" 1
	pushq %r12
# 0 "" 2
# 50 "lwp.c" 1
	pushq %r13
# 0 "" 2
# 50 "lwp.c" 1
	pushq %r14
# 0 "" 2
# 50 "lwp.c" 1
	pushq %r15
# 0 "" 2
# 50 "lwp.c" 1
	pushq %rbp
# 0 "" 2
#NO_APP
	movl	lwp_running(%rip), %edx
#APP
# 51 "lwp.c" 1
	movq  %rsp,%rax
# 0 "" 2
#NO_APP
	movslq	%edx, %rdx
	salq	$5, %rdx
	addq	$lwp_ptable+24, %rdx
	movq	%rax, (%rdx)
	movl	lwp_running(%rip), %eax
	movl	%eax, stopped(%rip)
	movq	orginal_stack_pointer(%rip), %rax
#APP
# 53 "lwp.c" 1
	movq  %rax,%rsp
# 0 "" 2
# 54 "lwp.c" 1
	popq  %rbp
# 0 "" 2
# 54 "lwp.c" 1
	popq  %r15
# 0 "" 2
# 54 "lwp.c" 1
	popq  %r14
# 0 "" 2
# 54 "lwp.c" 1
	popq  %r13
# 0 "" 2
# 54 "lwp.c" 1
	popq  %r12
# 0 "" 2
# 54 "lwp.c" 1
	popq  %r11
# 0 "" 2
# 54 "lwp.c" 1
	popq  %r10
# 0 "" 2
# 54 "lwp.c" 1
	popq  %r9
# 0 "" 2
# 54 "lwp.c" 1
	popq  %r8
# 0 "" 2
# 54 "lwp.c" 1
	popq  %rdi
# 0 "" 2
# 54 "lwp.c" 1
	popq  %rsi
# 0 "" 2
# 54 "lwp.c" 1
	popq  %rdx
# 0 "" 2
# 54 "lwp.c" 1
	popq  %rcx
# 0 "" 2
# 54 "lwp.c" 1
	popq  %rbx
# 0 "" 2
# 54 "lwp.c" 1
	popq  %rax
# 0 "" 2
# 54 "lwp.c" 1
	movq  %rbp,%rsp
# 0 "" 2
#NO_APP
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	lwp_stop, .-lwp_stop
	.globl	lwp_getpid
	.type	lwp_getpid, @function
lwp_getpid:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable, %rax
	movq	(%rax), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	lwp_getpid, .-lwp_getpid
	.globl	lwp_set_scheduler
	.type	lwp_set_scheduler, @function
lwp_set_scheduler:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, my_sched(%rip)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	lwp_set_scheduler, .-lwp_set_scheduler
	.globl	lwp_start
	.type	lwp_start, @function
lwp_start:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	lwp_procs(%rip), %eax
	testl	%eax, %eax
	je	.L14
#APP
# 70 "lwp.c" 1
	pushq %rax
# 0 "" 2
# 70 "lwp.c" 1
	pushq %rbx
# 0 "" 2
# 70 "lwp.c" 1
	pushq %rcx
# 0 "" 2
# 70 "lwp.c" 1
	pushq %rdx
# 0 "" 2
# 70 "lwp.c" 1
	pushq %rsi
# 0 "" 2
# 70 "lwp.c" 1
	pushq %rdi
# 0 "" 2
# 70 "lwp.c" 1
	pushq %r8
# 0 "" 2
# 70 "lwp.c" 1
	pushq %r9
# 0 "" 2
# 70 "lwp.c" 1
	pushq %r10
# 0 "" 2
# 70 "lwp.c" 1
	pushq %r11
# 0 "" 2
# 70 "lwp.c" 1
	pushq %r12
# 0 "" 2
# 70 "lwp.c" 1
	pushq %r13
# 0 "" 2
# 70 "lwp.c" 1
	pushq %r14
# 0 "" 2
# 70 "lwp.c" 1
	pushq %r15
# 0 "" 2
# 70 "lwp.c" 1
	pushq %rbp
# 0 "" 2
# 71 "lwp.c" 1
	movq  %rsp,%rax
# 0 "" 2
#NO_APP
	movq	%rax, orginal_stack_pointer(%rip)
	movl	stopped(%rip), %eax
	testl	%eax, %eax
	jle	.L11
	movl	stopped(%rip), %eax
	movl	%eax, lwp_running(%rip)
	jmp	.L12
.L11:
	movq	my_sched(%rip), %rax
	testq	%rax, %rax
	jne	.L13
	movl	$round_robbin, %edi
	call	lwp_set_scheduler
	movq	my_sched(%rip), %rax
	call	*%rax
	movl	%eax, lwp_running(%rip)
	jmp	.L12
.L13:
	movq	my_sched(%rip), %rax
	call	*%rax
	movl	%eax, lwp_running(%rip)
.L12:
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable+24, %rax
	movq	(%rax), %rax
#APP
# 82 "lwp.c" 1
	movq  %rax,%rsp
# 0 "" 2
# 83 "lwp.c" 1
	popq  %rbp
# 0 "" 2
# 83 "lwp.c" 1
	popq  %r15
# 0 "" 2
# 83 "lwp.c" 1
	popq  %r14
# 0 "" 2
# 83 "lwp.c" 1
	popq  %r13
# 0 "" 2
# 83 "lwp.c" 1
	popq  %r12
# 0 "" 2
# 83 "lwp.c" 1
	popq  %r11
# 0 "" 2
# 83 "lwp.c" 1
	popq  %r10
# 0 "" 2
# 83 "lwp.c" 1
	popq  %r9
# 0 "" 2
# 83 "lwp.c" 1
	popq  %r8
# 0 "" 2
# 83 "lwp.c" 1
	popq  %rdi
# 0 "" 2
# 83 "lwp.c" 1
	popq  %rsi
# 0 "" 2
# 83 "lwp.c" 1
	popq  %rdx
# 0 "" 2
# 83 "lwp.c" 1
	popq  %rcx
# 0 "" 2
# 83 "lwp.c" 1
	popq  %rbx
# 0 "" 2
# 83 "lwp.c" 1
	popq  %rax
# 0 "" 2
# 83 "lwp.c" 1
	movq  %rbp,%rsp
# 0 "" 2
#NO_APP
	jmp	.L8
.L14:
	nop
.L8:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	lwp_start, .-lwp_start
	.globl	lwp_yield
	.type	lwp_yield, @function
lwp_yield:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
#APP
# 87 "lwp.c" 1
	pushq %rax
# 0 "" 2
# 87 "lwp.c" 1
	pushq %rbx
# 0 "" 2
# 87 "lwp.c" 1
	pushq %rcx
# 0 "" 2
# 87 "lwp.c" 1
	pushq %rdx
# 0 "" 2
# 87 "lwp.c" 1
	pushq %rsi
# 0 "" 2
# 87 "lwp.c" 1
	pushq %rdi
# 0 "" 2
# 87 "lwp.c" 1
	pushq %r8
# 0 "" 2
# 87 "lwp.c" 1
	pushq %r9
# 0 "" 2
# 87 "lwp.c" 1
	pushq %r10
# 0 "" 2
# 87 "lwp.c" 1
	pushq %r11
# 0 "" 2
# 87 "lwp.c" 1
	pushq %r12
# 0 "" 2
# 87 "lwp.c" 1
	pushq %r13
# 0 "" 2
# 87 "lwp.c" 1
	pushq %r14
# 0 "" 2
# 87 "lwp.c" 1
	pushq %r15
# 0 "" 2
# 87 "lwp.c" 1
	pushq %rbp
# 0 "" 2
#NO_APP
	movl	lwp_running(%rip), %edx
#APP
# 88 "lwp.c" 1
	movq  %rsp,%rax
# 0 "" 2
#NO_APP
	movslq	%edx, %rdx
	salq	$5, %rdx
	addq	$lwp_ptable+24, %rdx
	movq	%rax, (%rdx)
	movq	my_sched(%rip), %rax
	testq	%rax, %rax
	jne	.L16
	movl	$round_robbin, %edi
	call	lwp_set_scheduler
	movq	my_sched(%rip), %rax
	call	*%rax
	movl	%eax, lwp_running(%rip)
	jmp	.L17
.L16:
	movq	my_sched(%rip), %rax
	call	*%rax
	movl	%eax, lwp_running(%rip)
.L17:
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable+24, %rax
	movq	(%rax), %rax
#APP
# 97 "lwp.c" 1
	movq  %rax,%rsp
# 0 "" 2
# 98 "lwp.c" 1
	popq  %rbp
# 0 "" 2
# 98 "lwp.c" 1
	popq  %r15
# 0 "" 2
# 98 "lwp.c" 1
	popq  %r14
# 0 "" 2
# 98 "lwp.c" 1
	popq  %r13
# 0 "" 2
# 98 "lwp.c" 1
	popq  %r12
# 0 "" 2
# 98 "lwp.c" 1
	popq  %r11
# 0 "" 2
# 98 "lwp.c" 1
	popq  %r10
# 0 "" 2
# 98 "lwp.c" 1
	popq  %r9
# 0 "" 2
# 98 "lwp.c" 1
	popq  %r8
# 0 "" 2
# 98 "lwp.c" 1
	popq  %rdi
# 0 "" 2
# 98 "lwp.c" 1
	popq  %rsi
# 0 "" 2
# 98 "lwp.c" 1
	popq  %rdx
# 0 "" 2
# 98 "lwp.c" 1
	popq  %rcx
# 0 "" 2
# 98 "lwp.c" 1
	popq  %rbx
# 0 "" 2
# 98 "lwp.c" 1
	popq  %rax
# 0 "" 2
# 98 "lwp.c" 1
	movq  %rbp,%rsp
# 0 "" 2
#NO_APP
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	lwp_yield, .-lwp_yield
	.globl	lwp_exit
	.type	lwp_exit, @function
lwp_exit:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	lwp_running(%rip), %eax
	movl	%eax, -4(%rbp)
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable+8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	free
	jmp	.L19
.L20:
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	-4(%rbp), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable, %rax
	movslq	%edx, %rdx
	salq	$5, %rdx
	addq	$lwp_ptable, %rdx
	movq	(%rdx), %rcx
	movq	%rcx, (%rax)
	movq	8(%rdx), %rcx
	movq	%rcx, 8(%rax)
	movq	16(%rdx), %rcx
	movq	%rcx, 16(%rax)
	movq	24(%rdx), %rdx
	movq	%rdx, 24(%rax)
	addl	$1, -4(%rbp)
.L19:
	movl	lwp_procs(%rip), %eax
	cmpl	%eax, -4(%rbp)
	jl	.L20
	movl	lwp_procs(%rip), %eax
	subl	$1, %eax
	movl	%eax, lwp_procs(%rip)
	movl	lwp_running(%rip), %eax
	subl	$1, %eax
	movl	%eax, lwp_running(%rip)
	movl	lwp_procs(%rip), %eax
	testl	%eax, %eax
	jne	.L21
	movl	$0, %eax
	call	lwp_stop
	jmp	.L25
.L21:
	movq	my_sched(%rip), %rax
	testq	%rax, %rax
	jne	.L23
	movl	$round_robbin, %edi
	call	lwp_set_scheduler
	movq	my_sched(%rip), %rax
	call	*%rax
	movl	%eax, lwp_running(%rip)
	jmp	.L24
.L23:
	movq	my_sched(%rip), %rax
	call	*%rax
	movl	%eax, lwp_running(%rip)
.L24:
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable+24, %rax
	movq	(%rax), %rax
#APP
# 122 "lwp.c" 1
	movq  %rax,%rsp
# 0 "" 2
# 123 "lwp.c" 1
	popq  %rbp
# 0 "" 2
# 123 "lwp.c" 1
	popq  %r15
# 0 "" 2
# 123 "lwp.c" 1
	popq  %r14
# 0 "" 2
# 123 "lwp.c" 1
	popq  %r13
# 0 "" 2
# 123 "lwp.c" 1
	popq  %r12
# 0 "" 2
# 123 "lwp.c" 1
	popq  %r11
# 0 "" 2
# 123 "lwp.c" 1
	popq  %r10
# 0 "" 2
# 123 "lwp.c" 1
	popq  %r9
# 0 "" 2
# 123 "lwp.c" 1
	popq  %r8
# 0 "" 2
# 123 "lwp.c" 1
	popq  %rdi
# 0 "" 2
# 123 "lwp.c" 1
	popq  %rsi
# 0 "" 2
# 123 "lwp.c" 1
	popq  %rdx
# 0 "" 2
# 123 "lwp.c" 1
	popq  %rcx
# 0 "" 2
# 123 "lwp.c" 1
	popq  %rbx
# 0 "" 2
# 123 "lwp.c" 1
	popq  %rax
# 0 "" 2
# 123 "lwp.c" 1
	movq  %rbp,%rsp
# 0 "" 2
#NO_APP
.L25:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	lwp_exit, .-lwp_exit
	.globl	round_robbin
	.type	round_robbin, @function
round_robbin:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	lwp_running(%rip), %eax
	addl	$1, %eax
	movl	%eax, lwp_running(%rip)
	movl	lwp_running(%rip), %edx
	movl	lwp_procs(%rip), %eax
	cmpl	%eax, %edx
	jne	.L27
	movl	$0, lwp_running(%rip)
.L27:
	movl	lwp_running(%rip), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	round_robbin, .-round_robbin
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.9) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
