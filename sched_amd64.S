/*
  void gosave(Gobuf *);
  void gogo(Gobuf *);
 */

.globl gosave
gosave:
  /* %rsp */
  leaq 8(%rsp), %rcx
  movq %rcx, (%rdi)
  /* %rip */
  movq (%rsp), %rcx
  movq %rcx, 8(%rdi)
  /* callee-saved */
  movq %rbx, 16(%rdi)
  movq %rbp, 24(%rdi)
  movq %r12, 32(%rdi)
  movq %r13, 40(%rdi)
  movq %r14, 48(%rdi)
  movq %r15, 56(%rdi)
  /* first arg */
  movq %rdi, 64(%rdi)

  /* return 0 */
  xorq %rax, %rax
  ret


.globl gogo
gogo:
  /* callee-saved */
  movq 56(%rdi), %r15
  movq 48(%rdi), %r14
  movq 40(%rdi), %r13
  movq 32(%rdi), %r12
  movq 24(%rdi), %rbp
  movq 16(%rdi), %rbx
  /* %rsp */
  movq (%rdi), %rsp
  /* %rip */
  movq 8(%rdi), %rcx
  /* first arg */
  movq 64(%rdi), %rdi

  /* return 1 */
  movq $1, %rax
  jmp *%rcx
