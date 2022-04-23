#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/reboot.h>

int main(int argc, char *argv[]){
  char pidline[1024];
  pidline[0] = '\0';
  char *pid;
  int i = 0;
  int pidno[64];
  FILE *fp = popen("pidof dts-simple", "r");
  fgets(pidline, 1024, fp);
  //printf(">>%s", pidline);
  pid = strtok(pidline, " ");

  if (pid == NULL){
    reboot(RB_AUTOBOOT);
  }else{
    //printf("%s\n", pid);
    //kill(atoi(pid), SIGSEGV);
  }

  while (pid != NULL){
    pidno[i] = atoi(pid);
    printf("%d\n", pidno[i]);
    pid = strtok(NULL, " ");
    i++;
  }
  pclose(fp);

  return 0;
}
