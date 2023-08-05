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

  uint64 va;             // �����ҳ����ʼ��ַ
  int num_pages;         // �����ҳ���ҳ��
  uint64 access_mask;    // ��¼���������ĵ�ַ

  // ���û�ջ�л�ȡ����
  argaddr(0, &va);  
  argint(1, &num_pages);
  argaddr(2, &access_mask);

  if (num_pages <= 0 || num_pages > 512)
  {
    return -1;
  }

  uint mask = 0;

  // ����ҳ��
  for (int i = 0; i < num_pages; i++)
  {
    pte_t* pte = walk(p->pagetable, va + i * PGSIZE, 0);
    if (pte && (*pte & PTE_V) && (*pte & PTE_A))
    {
      *pte &= ~PTE_A;  // �������λ
      mask |= (1 << i);
    }
  }

  // �������д���û�ջ
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
