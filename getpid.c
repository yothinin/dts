#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/reboot.h>
#include <curl/curl.h>

int test_connect()
{
  CURL *curl;
  CURLcode res = 0;
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (curl){
    curl_easy_setopt(curl, CURLOPT_URL, "https://dts.bustecz.com/dts_api/getsch.php");
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }

  return res;
}

int main(int argc, char *argv[]){
  char pidline[1024];
  pidline[0] = '\0';
  char *pid;
  FILE *fp = popen("pidof dts-simple", "r");
  fgets(pidline, 1024, fp);
  pid = strtok(pidline, " ");

  if (pid == NULL){
    if (test_connect() == 0){ //connect succeed.
      reboot(RB_AUTOBOOT);
    }else {
      printf("Please check internet connection!!!\n");
    }
  }

  pclose(fp);

  return 0;
}
