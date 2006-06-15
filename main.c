#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"

extern char edata[], end[];

int
main()
{
  struct proc *p;
  int i;
  
  // clear BSS
  memset(edata, 0, end - edata);

  // partially initizialize PIC
  outb(0x20+1, 0xFF); // IO_PIC1
  outb(0xA0+1, 0xFF); // IO_PIC2
  outb(0x20, 0x11);
  outb(0x20+1, 32);

  cprintf("\nxV6\n\n");

  kinit(); // physical memory allocator
  tinit(); // traps and interrupts

  // create fake process zero
  p = &proc[0];
  curproc = p;
  p->state = WAITING;
  p->sz = PAGE;
  p->mem = kalloc(p->sz);
  memset(p->mem, 0, p->sz);
  p->kstack = kalloc(KSTACKSIZE);
  p->tf = (struct Trapframe *) (p->kstack + KSTACKSIZE - sizeof(struct Trapframe));
  memset(p->tf, 0, sizeof(struct Trapframe));
  p->tf->tf_es = p->tf->tf_ds = p->tf->tf_ss = (SEG_UDATA << 3) | 3;
  p->tf->tf_cs = (SEG_UCODE << 3) | 3;
  p->tf->tf_eflags = FL_IF;
  setupsegs(p);

  p = newproc();

  i = 0;
  p->mem[i++] = 0x90; // nop 
  p->mem[i++] = 0xb8; // mov ..., %eax
  p->mem[i++] = SYS_fork;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0xcd; // int
  p->mem[i++] = T_SYSCALL;
  p->mem[i++] = 0xb8; // mov ..., %eax
  p->mem[i++] = SYS_exit;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0xcd; // int
  p->mem[i++] = T_SYSCALL;
  p->tf->tf_eip = 0;
  p->tf->tf_esp = p->sz;

  swtch();

  return 0;
}