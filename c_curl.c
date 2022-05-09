#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>
#include <json-c/json.h>

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
  char url[] = "https://dts.bustecz.com/dts_api/getsch.php";
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
      //printf("result:\n%s\n", chunk.memory);

      json_object *root = json_tokener_parse(chunk.memory);

      const char *str;
      int i;
      int n = json_object_array_length(root);
      for (i = 0; i<n; i++){
        str = json_object_get_string(json_object_array_get_idx(root, i));
        json_object *sch = json_tokener_parse(str);
        /*
        json_object *depTime = json_object_object_get(sch, "dep_time");
        json_object *depDest = json_object_object_get(sch, "dep_dest");
        json_object *depBusno = json_object_object_get(sch, "dep_busno");
        json_object *depStandard = json_object_object_get(sch, "dep_standard");
        json_object *depPlatform = json_object_object_get(sch, "dep_platform");
        json_object *depDepart = json_object_object_get(sch, "dep_depart");
        printf("%s\t", json_object_get_string(depTime));
        printf("%s\t", json_object_get_string(depDest));
        printf("%s\t", json_object_get_string(depBusno));
        printf("%s\t", json_object_get_string(depStandard));
        printf("%s\t", json_object_get_string(depPlatform));
        printf("%s\n", json_object_get_string(depDepart));

        json_object_put(sch);
        json_object_put(depTime);
        json_object_put(depDest);
        json_object_put(depBusno);
        json_object_put(depStandard);
        json_object_put(depPlatform);
        json_object_put(depDepart);
        */

        json_object *objTime = json_object_object_get(sch, "dep_time");
        json_object *objDest = json_object_object_get(sch, "dep_dest");
        json_object *objBusno = json_object_object_get(sch, "dep_busno");
        json_object *objStandard = json_object_object_get(sch, "dep_standard");
        json_object *objPlatform = json_object_object_get(sch, "dep_platform");
        json_object *objDepart = json_object_object_get(sch, "dep_depart");

        printf("%s\t", json_object_get_string(objTime));
        printf("%s\t", json_object_get_string(objDest));
        printf("%s\t", json_object_get_string(objBusno));
        printf("%s\t", json_object_get_string(objStandard));
        printf("%s\t", json_object_get_string(objPlatform));
        printf("%s\n", json_object_get_string(objDepart));

        json_object_put(sch);
        json_object_put(objTime);
        json_object_put(objDest);
        json_object_put(objBusno);
        json_object_put(objStandard);
        json_object_put(objPlatform);
        json_object_put(objDepart);

      }

      json_object_put(root);

    }
  }

  curl_easy_cleanup(curl);
  free(chunk.memory);
  curl_global_cleanup();

  return 0;
}
