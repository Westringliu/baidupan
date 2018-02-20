#include "func.h"

int main(int argc,char *argv[])
{
    if(argc!=3)
    {
        printf("error args\n");
        return -1;
    }
    int sfd;
    sfd=socket(AF_INET,SOCK_STREAM,0);
    if(-1==sfd)
    {
        perror("socket");
        return -1;
    }
    struct sockaddr_in ser;
    bzero(&ser,sizeof(ser));
    ser.sin_family=AF_INET;
    ser.sin_port=htons(atoi(argv[2]));//一定要用htons
    ser.sin_addr.s_addr=inet_addr(argv[1]);
    int ret=connect(sfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
    if(-1==ret)
    {
        perror("connect");
        return -1;
    }
    char buf[4096]={0};
    char buffer[10]={0};
    char command[64];
    char rela_dir[64];
    int len;
    int i;
    int fd;
    while(1)
    {
        bzero(buf,sizeof(buf));
        bzero(command,sizeof(command));
        bzero(rela_dir,sizeof(rela_dir));
        read(0,buf,sizeof(buf));
        //给pwd，ls末尾加空格
        if(!(buf[0]=='p'&&buf[1]=='u'||buf[0]=='c'||buf[0]=='r'||buf[0]=='g'))
        {
            for(i=0;buf[i]!='\n';i++);
            buf[i]=' ';
            buf[i+1]='\n';
        }
        sendn(sfd,buf,sizeof(buf));
        strcpy(command,strtok(buf," "));
        strcpy(rela_dir,strtok(NULL," "));
        for(i=0;rela_dir[i]!='\n';i++);
        rela_dir[i]='\0';
        if(!strcmp(command,"gets"))
        {
            len=0;
            bzero(buffer,sizeof(buffer));
            recvn(sfd,(char*)&len,sizeof(len));//接收文件名
            recvn(sfd,buffer,len);
            printf("in client,len=%d,buffer=%s\n",len,buffer);
            fd=open(buffer,O_RDWR|O_CREAT,0666);
            if(-1==fd)
            {
                perror("open");
                return -1;
            }
            while(1)
            {
                recvn(sfd,(char*)&len,sizeof(len));
                if(len>0)
                {
                    bzero(buffer,sizeof(buffer));
                    recvn(sfd,buffer,len);
                    write(fd,buffer,len);
                }else{
                    printf("gets success\n");
                    break;
                }
            }
            close(fd);
        }
        else if(!strcmp(command,"puts"))
        {
            printf("i will send file=%s\n",rela_dir);
            trans_file(sfd,rela_dir);    
            printf("puts success\n");
        }
        else if(!strcmp(command,"cd"))
        {
            system("clear");
            recvn(sfd,buf,sizeof(buf));
            printf("%s\n",buf);
        }
        else if(!strcmp(command,"pwd"))
        {
            system("clear");
            recvn(sfd,buf,sizeof(buf));
            printf("%s\n",buf);
        }
        else if(!strcmp(command,"ls"))
        {
            system("clear");
            recvn(sfd,buf,sizeof(buf));
            printf("%s\n",buf);
        }
        else if(!strcmp(command,"rm"))
        {
            system("clear");
            recvn(sfd,buf,sizeof(buf));
            printf("%s\n",buf);
        }
        else if(!strcmp(command,"quit"))
        {
            system("clear");
            close(sfd);
            return 0;
        }
        else
        {
            system("clear");
            recvn(sfd,buf,sizeof(buf));
            printf("%s",buf);
        }
    }
}


