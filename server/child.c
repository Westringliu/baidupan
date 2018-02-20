#include "func.h"

void make_child(pData p,int pro_num)
{
    int i;
    int fds[2];
    pid_t pid;
    for(i=0;i<pro_num;i++)
    {
        //socketpair创建的只适用于父子进程或者线程间通信，不能用于两个进程之间通信
        socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
        pid=fork();
        if(0==pid)
        {
            close(fds[1]);
            child_handle(fds[0]);
        }
        close(fds[0]);
        p[i].pid=pid;
        p[i].pfd=fds[1];
        p[i].busy=0;
        printf("p[i].pfd=%d\n",p[i].pfd);
    }
}

void child_handle(int pfd)
{
    int new_fd,fd;
    char c=0;
    train t;
    char buf[4096]={0};
    char command[64];
    char rela_dir[64];
    int i,len;
    int ret;
    char buffer[10];
    while(1)
    {
        recv_fd(pfd,&new_fd);
        while(1)
        {
            bzero(buf,sizeof(buf));
            bzero(command,sizeof(command));
            bzero(rela_dir,sizeof(rela_dir));
            recvn(new_fd,buf,sizeof(buf));
            strcpy(command,strtok(buf," "));
            strcpy(rela_dir,strtok(NULL," "));
            for(i=0;rela_dir[i]!='\n';i++);
            rela_dir[i]='\0';
            if(!strcmp(command,"puts"))
            {
                len=0;
                bzero(buffer,sizeof(buffer));
                recvn(new_fd,(char*)&len,sizeof(len));
                recvn(new_fd,buffer,len);
                printf("in child,len=%d,buffer=%s\n",len,buffer);
                fd=open(buffer,O_RDWR|O_CREAT,0666);
                if(-1==fd)
                {
                    perror("open");
                    return;
                }
                while(1)
                {
                    recvn(new_fd,(char*)&len,sizeof(len));
                    if(len>0)
                    {
                        bzero(buffer,sizeof(buffer));
                        recvn(new_fd,buffer,len);
                        write(fd,buffer,len);
                    }else{
                        printf("puts success\n");
                        break;
                    }
                }
                close(fd);
            }
            else if(!strcmp(command,"gets"))
            {
                printf("I will send file new_fd=%d\n",new_fd);
                trans_file(new_fd,rela_dir);     
            }
            else if(!strcmp(command,"cd"))
            {
                ret=cd(rela_dir,buf);
                if(-1==ret)
                {
                    strcpy(buf,"No such file or directory");
                    sendn(new_fd,buf,sizeof(buf));
                }else{
                    printf("i am in child,dir=%s\n",buf); 
                    sendn(new_fd,buf,sizeof(buf));
                }
            }
            else if(!strcmp(command,"pwd"))
            {
                pwd(buf);
                sendn(new_fd,buf,sizeof(buf));
            }
            else if(!strcmp(command,"ls"))
            {
                ls(buf);
                sendn(new_fd,buf,sizeof(buf));
            }
            else if(!strcmp(command,"rm"))
            {
                rm(rela_dir,buf);
                sendn(new_fd,buf,sizeof(buf));
            }
            else if(!strcmp(command,"quit"))
            {
                close(new_fd);
                write(pfd,&c,sizeof(c));//通知父进程我完成了文件发送
                break;
            }
            else
            {
                strcpy(buf,"No such command\n");
                sendn(new_fd,buf,sizeof(buf));
            }
        }
    }
}

