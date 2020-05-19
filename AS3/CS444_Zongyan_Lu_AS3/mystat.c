#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>



void append(char* , char);

int get_permission_number(char*);

bool symlink_exists(const char*);

// void test(const char*);


int get_permission_number(char* s){
  int permission_number = 0;
  int owner_digit = 0;
  int group_digit = 0;
  int public_digit = 0;

  if(s[1] == 'r'){
    owner_digit = owner_digit + 4;
  }
  if(s[2] == 'w'){
    owner_digit = owner_digit + 2;
  }
  if(s[3] == 'x'){
    owner_digit = owner_digit + 1;
  }
  if(s[4] == 'r'){
    group_digit = group_digit + 4;
  }
  if(s[5] == 'w'){
    group_digit = group_digit + 2;
  }
  if(s[6] == 'x'){
    group_digit = group_digit + 1;
  }
  if(s[7] == 'r'){
    public_digit = public_digit + 4;
  }
  if(s[8] == 'w'){
    public_digit = public_digit + 2;
  }
  if(s[9] == 'x'){
    public_digit = public_digit + 1;
  }
  permission_number = 100*owner_digit + 10*group_digit + 1*public_digit;
  return permission_number;
}

void append(char* s, char c) {
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}


//Refernece from StackOverflow website:
//https://stackoverflow.com/questions/24297693/how-to-check-if-soft-link-exists-or-not/24299212
bool symlink_exists(const char* path)
{
    int status;
    struct stat block;
    char *link_path;

    if (lstat(path, &block) == 0) {
       link_path = malloc(block.st_size + 1);
       readlink(path, link_path, block.st_size + 1);
       link_path[block.st_size] = '\0';
       if (stat(link_path, &block) == 0){
           status = 1;
       }//end if
       else{
         status = 0;
       }
    }
    return status;
}



int
main(int argc, char *argv[])
{
    int i;
    struct tm *file_time = NULL;
    struct stat stat_block;
    struct passwd *pw;
    struct group *gr;
    char *ownid;
    char *groupid;
    int permission_number;

    char first_place;
    char r, w, x, rg, wg, xg, ro, wo, xo;

    char permission[255] = "";
    char timebuffer[128];
    char linkbuffer[2048];


   if (argc == 1) {
        fprintf(stderr, "Usage: mystat <filename1> ... <filenameN>\n");
        exit(EXIT_FAILURE);
    }


for(i = 1; i < argc; i++){
    //reinitial char array
    memset(permission, 0, sizeof(permission));

     if (lstat(argv[i], &stat_block) == -1) {
          fprintf(stderr, "*** Failed to stat file '%s', skipping.\n", argv[i]);
          fprintf(stderr, "***  File '%s' does not exist or you done have access.\n", argv[i]);
          fprintf(stderr, "*** Could not stat file: No such file or directory\n");
          // perror("stat");
          continue;
          // exit(EXIT_FAILURE);
      }

      //get owner id and group id
      pw = getpwuid(stat_block.st_uid);
      gr = getgrgid(stat_block.st_gid);

      if(pw != 0){
        ownid = pw->pw_name;
      }
      if(gr != 0){
        groupid = gr->gr_name;
      }



     printf("File: %s\n", argv[i]);

     printf("  File type:                ");

     switch (stat_block.st_mode & S_IFMT) {
          case S_IFBLK:
               printf("block device\n");
               break;
          case S_IFCHR:
               printf("character device\n");
               break;
          case S_IFDIR:
               printf("directory\n");
               first_place = 'd';
               break;
          case S_IFIFO:
               printf("FIFO/pipe\n");
               first_place = 'p';
               break;
          case S_IFLNK:
                // printf("bool value: %d\n", symlink_exists(argv[i]));
                if(symlink_exists(argv[i]))
                {
                  readlink(argv[i],linkbuffer,2047);
                  printf("Symbolic link -> %s\n",linkbuffer);
                }
                else{
                  printf("Symbolic link - with dangling destination\n");
                }
               // printf("symlink\n");
               first_place = 'l';
               break;
          case S_IFREG:
               printf("regular file\n");
               first_place = '-';
               break;
          case S_IFSOCK:
               printf("socket\n");
               first_place = 's';
               break;
          default:
               printf("Failed to read!!\n");
               exit(EXIT_FAILURE);
               break;
      }


      //chmod calculation
      r = (stat_block.st_mode & S_IRWXU & S_IRUSR) ? 'r' : '-';
      w = (stat_block.st_mode & S_IRWXU & S_IWUSR) ? 'w' : '-';
      x = (stat_block.st_mode & S_IRWXU & S_IXUSR) ? 'x' : '-';
      rg = (stat_block.st_mode & S_IRWXG & S_IRGRP) ? 'r' : '-';
      wg = (stat_block.st_mode & S_IRWXG & S_IWGRP) ? 'w' : '-';
      xg = (stat_block.st_mode & S_IRWXG & S_IXGRP) ? 'x' : '-';
      ro = (stat_block.st_mode & S_IRWXO & S_IROTH) ? 'r' : '-';
      wo = (stat_block.st_mode & S_IRWXO & S_IWOTH) ? 'w' : '-';
      xo = (stat_block.st_mode & S_IRWXO & S_IXOTH) ? 'x' : '-';

      append(permission, first_place);
      append(permission, r);
      append(permission, w);
      append(permission, x);
      append(permission, rg);
      append(permission, wg);
      append(permission, xg);
      append(permission, ro);
      append(permission, wo);
      append(permission, xo);

      printf("permission: %s\n", permission);

      permission_number = get_permission_number(permission);




     printf("  Devide ID number:         %ld\n", (long) stat_block.st_dev);
     printf("  I-node number:            %ld\n", (long) stat_block.st_ino);
     printf("  Mode:                     %s          (%d in octal)\n", permission, permission_number );
     printf("  Link count:               %ld\n", (long) stat_block.st_nlink);
     printf("  Owner Id:                 %s                (UID = %ld)\n", ownid, (long) stat_block.st_uid);
     printf("  Group Id:                 %s            (GID = %ld)\n", groupid, (long) stat_block.st_gid);
     printf("  Preferred I/O block size: %ld bytes\n", (long) stat_block.st_blksize);
     printf("  File size:                %lld bytes\n", (long long) stat_block.st_size);
     printf("  Blocks allocated:         %lld\n", (long long) stat_block.st_blocks);

     //get access time for a file
     file_time = localtime(&stat_block.st_atime);
     strftime(timebuffer,sizeof(timebuffer),"%F %T %z (%Z) %a",file_time);
     printf("  Last file access:         %s (local)\n", timebuffer);

     file_time = localtime(&stat_block.st_mtime);
     strftime(timebuffer,sizeof(timebuffer),"%F %T %z (%Z) %a",file_time);
     printf("  Last file modification:   %s (local)\n", timebuffer);

     file_time = localtime(&stat_block.st_ctime);
     strftime(timebuffer,sizeof(timebuffer),"%F %T %z (%Z) %a",file_time);
     printf("  Last status change:       %s (local)\n", timebuffer);

// memset(permission, 0, sizeof(permission));

}//end big for loop

     exit(EXIT_SUCCESS);
}
