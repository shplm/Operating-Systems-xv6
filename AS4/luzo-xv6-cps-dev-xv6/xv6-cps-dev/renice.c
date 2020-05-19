#include "types.h"
#include "user.h"
// #include "defs.h"

int main(int argc, char** argv)
{
  int nice_value = 0;
  int pid = 0;
  int i = 0;
  // // int proc_exist = 0;
  //
  // int nice_value = atoi(argv[1]);
  //
  if(argc <= 2){
     printf(1, "Usage: renice nice_num [child_1 child_2] ... \n");
    exit();
  }

for(i = 2; i < argc; i++){

  pid = atoi(argv[1]);
  nice_value = atoi(argv[i]);

  renice(nice_value, pid);

}//end for


  // // cprintf(1, "Mother %d created\n", getpid());
  //
  // if(nice_value < 1 || nice_value > 40){
  //   printf(1, "Out of bounds, nice value INVALID!\n");
  //   return 1;
  // }
  //
  //
  // for(i = 1; i < argc; i++){
  //   const int pid = fork();
  //   const int ppid = getppid();
  //   if(pid < 0){
  //     //end fork process
  //     printf(1, "Stillbrith occurred");
  //     exit();
  //   }//end if
  //   if(!pid){
  //     const int t = atoi(argv[i]); //number of pid
  //
  //     printf(1, "argv[i]: %d\n", t);
  //
  //     printf(1, "Input pid: %d\n", pid);
  //
  //     printf(1, "ppid: %d\n", ppid);
  //
  //   }//end if
  //
  //
  //
  // }//end for
  //
  // // printf(1, "hello world!\n");
  //
  // printf(1, "hello world: %d\n", renice());
  //
  // for( j=1; j < argc; j++){
  //   wait();
  // }

  // exit();


  // printf(1, "I callsys_renice(): %d\n", renice());

  exit();
}
