#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

struct MemoryStruct{
  char *memory;
  size_t size;
};

struct MemoryStruct chunk;

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
  mem->memory = realloc(mem->memory, mem->size+realsize + 1); //Resize memory by realloc function.

  if (mem->memory == NULL){
    // out of memory
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

int execApi(char url[], char postData[])
{
  CURL *curl;
  CURLcode res;
 
  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  if (curl){
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl);
    if (res != CURLE_OK){
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
  }
  
  curl_easy_cleanup(curl);
  curl_global_cleanup();

  return res;
}

int
main(int argc, char **argv)
{
  char url[] = "https://dts.bustecz.com/dts_api/addsch.php";

  char postData[] =  "depTime=20:30&depDest=เชียงใหม่&depBusno=18-45&depStdCode=13&depStandard=ม.1พ&depPlatform=&depNote=&depDepart=0";
  
  printf("posdata[] = %s\n", postData);
  
  chunk.memory = malloc(1); // Allocate 1 byte and resize later.
  chunk.size = 0;

  if (execApi(url, postData) == 0){
    printf("%s\n", chunk.memory);
    json_object *root = json_tokener_parse(chunk.memory);
    const char *str;
    int i;
    int n = json_object_array_length(root);
    for (i = 0; i<n; i++){
      str = json_object_get_string(json_object_array_get_idx(root, i));
      json_object *sch = json_tokener_parse(str);
      
      json_object *objStatus;
      json_object_object_get_ex(sch, "Status", &objStatus); 
      printf("%s\n", json_object_get_string(objStatus));
      
      json_object_put(objStatus);
      
      /*
      json_object *objTime, *objDest, *objBusno, *objStandard, *objPlatform, *objDepart;
      json_object_object_get_ex(sch, "dep_time", &objTime);
      json_object_object_get_ex(sch, "dep_dest", &objDest);
      json_object_object_get_ex(sch, "dep_busno", &objBusno);
      json_object_object_get_ex(sch, "dep_standard", &objStandard);
      json_object_object_get_ex(sch, "dep_platform", &objPlatform);
      json_object_object_get_ex(sch, "dep_depart", &objDepart);

      printf("%s\t", json_object_get_string(objTime));
      printf("%s\t", json_object_get_string(objDest));
      printf("%s\t", json_object_get_string(objBusno));
      printf("%s\t", json_object_get_string(objStandard));
      printf("%s\t", json_object_get_string(objPlatform));
      printf("%s\n", json_object_get_string(objDepart));
      */

    }
    json_object_put(root);

  }

  free(chunk.memory); 

  return 0;
}
