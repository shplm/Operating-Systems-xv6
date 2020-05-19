#include "rand.h"
#include "types.h"
#include "user.h"
// #include "defs.h"



static unsigned long next = 1;

/* RAND_MAX assumed to be 1 << 31 */
uint gen_random(void){
  gen_srandom(next);
  next = next * 1103515245 + 12345;
  return((unsigned)(next/65536) % RAND_MAX);
}
void gen_srandom(unsigned int seed){
  next = seed;
  return;
}

// int sys_random()
//
// int main(int argc, char **argv){
//   // srandom();
//   printf(1, "rand number is: %d\n", random());
//
//   exit();
//
//   return 0;
// }
