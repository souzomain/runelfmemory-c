#define _GNU_SOURCE
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

//execl("/bin/sh","teste",(char* )NULL);
size_t get_file_size(const char *filename){
    struct stat st;
    if(stat(filename,&st) == -1){
        return -1;
    }
    return st.st_size;
}
int main(int argc, char *argv[]){
    char *buffer =0;
    FILE *f = fopen(argv[1],"rb");
    if(f==NULL){
        fclose(f);
        perror("FILE ");
        exit(1);
    }
    fseek(f,0,SEEK_END);
    long len = ftell(f);
    fseek(f,0,SEEK_SET);
    buffer = malloc(len);
    if(buffer == NULL){
        fclose(f);
        perror("MALLOC ");
        exit(1);
    }
    fread(buffer,1,len,f);
    fclose(f);
    printf("BUFFER: %s",buffer);
    char a[200] = "";
    
    int fd = memfd_create("",MFD_CLOEXEC);
    sprintf(a,"/proc/self/fd/%d",fd);
    write(fd,buffer,len);
    execl(a,"TESTE",(char *)NULL);
}
