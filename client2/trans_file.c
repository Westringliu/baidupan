#include "func.h"

void sig(int signum)
{
    printf("%d is coming\n",signum);
}

void trans_file(int sfd,char* rela_dir)
{
    signal(SIGPIPE,sig);
    train t;
    int i;
    bzero(&t,sizeof(t));
    strcpy(t.buf,rela_dir);//把文件名放入buf
    t.len=strlen(t.buf)+1;
    sendn(sfd,(char*)&t,4+t.len);//发送文件名火车给对端
    int fd;
    fd=open(rela_dir,O_RDONLY);
    if(-1==fd)
    {
        perror("open");
        return;
    }
    while(bzero(&t,sizeof(t)),(t.len=read(fd,t.buf,sizeof(t.buf)))>0)
    {
        sendn(sfd,(char*)&t,4+t.len);
    }
    t.len=0;
    sendn(sfd,(char*)&t,4);//代表文件结束
}

