#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.

  // acquire argument from user space
  uint64 base;
  argaddr(0, &base);
  int len;
  argint(1, &len);
  uint64 mask;
  argaddr(2, &mask);
  
  // find the address of the third level page directory
  pagetable_t pagetable = myproc()->pagetable;
  for(int level = 2; level > 0; level--) {
    pte_t pte = pagetable[PX(level, base)];
    if(pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(pte);
    } else {
      return -1;
    }
  }

  // iteratively go through len PTNs, and set mask_kernel accordingly
  int offset = PX(0, base);
  uint64 mask_kernel = 0;
  for (int i = 0; i < len; i++) {
    if (pagetable[offset+i] & (PTE_A)) {
      mask_kernel |= (1L << i);
    }

    // clear PTE_A
    pagetable[offset+i] &= (~PTE_A);
    // be careful about the difference between ~ and !
    // use ! you will erase the whole PTN
  }

  // copyout mask_kernel from kernel to user space
  if (copyout(myproc()->pagetable, mask, (char*)(&mask_kernel), sizeof(mask_kernel)) < 0)
    return -1;

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
