#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

struct MemoryStruct{
  char *memory;
  size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
  mem->memory = realloc(mem->memory, mem->size+realsize + 1);

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

int
main(int argc, char **argv)
{
  char url[] = "https://dts.bustecz.com/dts_api/";
  char postData[] = "date=2022-05-06&station=10001&route=18";

  CURL *curl;
  CURLcode res;

  struct MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;
  
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
    }else{
      //printf("%lu bytes retrieved\n", (long)chunk.size);
      printf("%s\n", chunk.memory);
    }
  }
  
  curl_easy_cleanup(curl);
  free(chunk.memory);
  curl_global_cleanup();

  return 0;
}
