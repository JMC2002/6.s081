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
  proc* p = myproc();

  uint64 va;             // 待检测页表起始地址
  int num_pages;         // 待检测页表的页数
  uint64 access_mask;    // 记录检测结果掩码的地址

  // 从用户栈中获取参数
  argaddr(0, &va);  
  argint(1, &num_pages);
  argaddr(2, &access_mask);

  if (num_pages <= 0 || num_pages > 512)
  {
    return -1;
  }

  uint mask = 0;

  // 遍历页表
  for (int i = 0; i < num_pages; i++)
  {
    pte_t* pte = walk(p->pagetable, va + i * PGSIZE, 0);
    if (pte && (*pte & PTE_V) && (*pte & PTE_A))
    {
      *pte &= ~PTE_A;  // 清除访问位
      mask |= (1 << i);
    }
  }

  // 将检测结果写入用户栈
  copyout(p->pagetable, access_mask, (char*)&mask, sizeof(mask));
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
