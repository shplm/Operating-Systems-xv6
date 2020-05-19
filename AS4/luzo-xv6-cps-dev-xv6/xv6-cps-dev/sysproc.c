#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "rand.h"

int
sys_fork(void)
{
  return fork();
}

#ifdef GETPPID
int
sys_getppid(void)
{
    int ppid = 1;

    if (myproc()->parent) {
        ppid = myproc()->parent->pid;
    }
    return ppid;
}
#endif

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

int
sys_kdebug(void)
{
    extern uint debugState;

    int tof = FALSE;

    if (argint(0, &tof) < 0)
        return -1;
    debugState = tof;
    return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_halt(void){
	outb(0xf4, 0x00);
	return 0;
}


int
sys_random(void){
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  gen_srandom(xticks);
  // srandom(next);
  // // cprintf("The rand number is: %d\n", get_rand());
  // next = next * 1103515245 + 12345;
  // return((unsigned)(next/65536) % RAND_MAX);
  return gen_random();
}

int sys_renice(void){
  int pid = 0;
  int nice_value = 0;
  int return_value = 0;

  // cprintf("I am in sys_renice here!, here is my pid: %d\n", sys_getpid());

  // if(argint(0, &pid) < 1 || argint(0, &nice_value) > 40){
  //   cprintf("Out of bounds, nice value INVALID!\n");
  //   return 1;
  // }

  argint(0, &pid);
  argint(1, &nice_value);

  // cprintf("pid: %d\n", pid);
  // cprintf("nice_value:  %d\n", nice_value);

  return_value = renice(nice_value, pid);

  // if(nice_value < 1 || nice_value > 40){
  //   printf(1, "Out of bounds, nice value INVALID!\n");
  //   return 1;
  // }
  //else if(){
    //pid not exists

    //return 2;

  //}



  //succeeds
  return return_value;
}
