#include "func.h"
#define OLD_DIR "/home/westring/pan/server/"

int cd(char *rela_dir,char* buf)//传入相对路径
{
    char curdir[128]={0};
    char olddir[128]={0}; 
    getcwd(olddir,128);
    strcpy(curdir,olddir);
    sprintf(curdir,"%s/%s",olddir,rela_dir);
    DIR *p_open;
    p_open=opendir(curdir);
    if(NULL==p_open)
    {
        perror("opendir");
        return -1;
    }   
    chdir(curdir);
    getcwd(curdir,128);
    strcpy(buf,curdir);
    return 0;
}
void ls(char* buf)
{
    bzero(buf,4096);
    struct stat buffer;
    DIR *dir;
    char curdir[128];
    getcwd(curdir,sizeof(curdir));
    dir=opendir(curdir);
    if(NULL==dir)
    {
        perror("opendir");
        return;
    }
    struct dirent *p;
    while((p=readdir(dir))!=NULL)
    {
        memset(&buffer,0,sizeof(buffer));
        int ret;
        ret=stat(p->d_name,&buffer);
        if(-1==ret)
        {
            perror("stat");
            return;
        }
        char i=S_ISDIR(buffer.st_mode)?'d':'f';
        if(strcmp(p->d_name,".")&&strcmp(p->d_name,".."))
        {
        sprintf(buf,"%s%c %9ld %.12s %s \n",buf,i,buffer.st_size,4+ctime(&buffer.st_mtime),p->d_name);
        }
    }
    return;
}

void pwd(char* buf)
{
    char* curdir;
    curdir=(char*)calloc(128,sizeof(char));
    getcwd(curdir,128);
    printf("%s\n",curdir);
    strcpy(buf,curdir);
    return;
}

int rm(char *filename,char* buf)
{
    bzero(buf,128);
    int ret;
    char curdir[128]={0};
    chdir(curdir);
    getcwd(curdir,128);
    strcat(curdir,"/");
    strcat(curdir,filename);
    ret=unlink(curdir);
    if(-1==ret)
    {
        perror("unlink");
        strcpy(buf,"Remove failed:No such file or directory");
        return -1;
    }else{
        strcpy(buf,"remove success");
        return 0;
    }
}

