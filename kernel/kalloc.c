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

struct run {
  struct run *next;
};
struct {
  struct spinlock lock;
  int pageref[PHYSTOP/PGSIZE]; 
}PGref;

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&PGref.lock,"PGref");
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

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  acquire(&PGref.lock);
  if(--PGref.pageref[(uint64)pa/PGSIZE] <= 0) {
    memset(pa, 1, PGSIZE);
    r = (struct run*)pa;
    acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    release(&kmem.lock);
  }
  if(PGref.pageref[(uint64)pa/PGSIZE] <0){
    PGref.pageref[(uint64)pa/PGSIZE]=0;
  }
  release(&PGref.lock);
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

  if(r){
    memset((char*)r, 5, PGSIZE); // fill with junk
    PGref.pageref[(uint64)r/PGSIZE] = 1;
  }
  return (void*)r;
}

void krefpage(void *pa) {
  acquire(&PGref.lock);
  PGref.pageref[(uint64)pa/PGSIZE] ++;
  release(&PGref.lock);
}

void *kcopyfromref(void *pa) {
  acquire(&PGref.lock);

  if(PGref.pageref[(uint64)pa/PGSIZE] <= 1) { // 只有 1 个引用，无需复制
    release(&PGref.lock);
    return pa;
  }

  // 分配新的内存页，并复制旧页中的数据到新页
  uint64 newpa = (uint64)kalloc();
  if(newpa == 0) {
    release(&PGref.lock);
    return 0; // out of memory
  }
  memmove((void*)newpa, (void*)pa, PGSIZE);

  // 旧页的引用减 1
  PGref.pageref[(uint64)pa/PGSIZE] --;

  release(&PGref.lock);
  return (void*)newpa;
}


