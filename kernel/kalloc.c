// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

// 引用计数的锁和保存值
struct spinlock cow_ref_lock;
int cow_cnt[(PHYSTOP - KERNBASE) / PGSIZE];
#define PA2IDX(pa) (((uint64)(pa) - KERNBASE) / PGSIZE)

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
inc_ref(void* pa) // 自增引用计数
{
  acquire(&cow_ref_lock);
  cow_cnt[PA2IDX(pa)]++;
  release(&cow_ref_lock);
}

void
dec_ref(void* pa) // 自减引用计数
{
  acquire(&cow_ref_lock);
  cow_cnt[PA2IDX(pa)]--;
  release(&cow_ref_lock);
}

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&cow_ref_lock, "cow_ref_lock");  // 初始化引用计数的锁
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  if (cow_cnt[PA2IDX(r)] > 1) // 只有引用计数为1时才释放
  { 
    dec_ref(r);
    return;
  }

  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
  {
    cow_cnt[PA2IDX(r)] = 1;      // 将引用计数置1
    memset((char*)r, 5, PGSIZE); // fill with junk
  }
  return (void*)r;
}
