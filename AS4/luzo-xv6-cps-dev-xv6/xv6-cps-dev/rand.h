#ifndef __RAND_H
# define __RAND_H

#ifndef RAND_MAX
# define RAND_MAX (1 << 31)
#endif



unsigned int gen_random(void);
void gen_srandom(unsigned int seed);

// static unsigned long next = 1;

#endif // __RANDOM_H
