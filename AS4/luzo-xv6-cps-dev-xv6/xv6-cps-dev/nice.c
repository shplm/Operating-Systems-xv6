#include "types.h"
#include "user.h"


int main(int argc, char** argv){

  // printf(1, "This is nice func\n");

  if(argc < 3){
    printf(1, "Insufficient argument.\n");
    exit();
  }

  // int pid = getpid();

  renice(getpid(), atoi(argv[1]));

  exec(argv[2], &(argv[2]));

  // renice(pid, atoi(argv[1]));
  // renice(atoi(argv[1]), getpid());

  // exec(argv[2], &(argv[2]));



  exit();
}
