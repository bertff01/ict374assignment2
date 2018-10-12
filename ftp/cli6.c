/* cli6.c - 	(Topic 11, HX 22/5/1995)
 *		An improved version of "cli5.c". Since TCP does not preserve the message 
 *              boundaries, each message is preceeded by a two byte value which is the
 *              length of the message. 
 * Revised;	06/11/2007
 */

#include  <stdlib.h>
#include  <stdio.h>
#include  <sys/types.h>        
#include  <sys/socket.h>
#include  <netinet/in.h>       /* struct sockaddr_in, htons, htonl */
#include  <netdb.h>            /* struct hostent, gethostbyname() */ 
#include  <string.h>
#include <sys/stat.h>
#include  "stream.h"           /* MAX_BLOCK_SIZE, readn(), writen() */
#include <fcntl.h>
#define   SERV_TCP_PORT  40005 /* default server listening port */

main(int argc, char *argv[])
{
     int sd, n, nr, nw, i=0;
     char buf[MAX_BLOCK_SIZE], host[60];
     unsigned short port;
     struct sockaddr_in ser_addr; struct hostent *hp;

     /* get server host name and port number */
     if (argc==1) {  /* assume server running on the local host and on default port */
          gethostname(host, sizeof(host));
          port = SERV_TCP_PORT;
     } else if (argc == 2) { /* use the given host name */ 
          strcpy(host, argv[1]);
          port = SERV_TCP_PORT;
     } else if (argc == 3) { // use given host and port for server
         strcpy(host, argv[1]);
          int n = atoi(argv[2]);
          if (n >= 1024 && n < 65536) 
              port = n;
          else {
              printf("Error: server port number must be between 1024 and 65535\n");
              exit(1);
          }
     } else { 
         printf("Usage: %s [ <server host name> [ <server listening port> ] ]\n", argv[0]); 
         exit(1); 
     }

    /* get host address, & build a server socket address */
     bzero((char *) &ser_addr, sizeof(ser_addr));
     ser_addr.sin_family = AF_INET;
     ser_addr.sin_port = htons(port);
     if ((hp = gethostbyname(host)) == NULL){
           printf("host %s not found\n", host); exit(1);   
     }
     ser_addr.sin_addr.s_addr = * (u_long *) hp->h_addr;

     /* create TCP socket & connect socket to server address */
     sd = socket(PF_INET, SOCK_STREAM, 0);
     if (connect(sd, (struct sockaddr *) &ser_addr, sizeof(ser_addr))<0) { 
          perror("client connect"); exit(1);
     }
    char cwd[1028];
     while (++i) {
	     if (getcwd(cwd, sizeof(cwd)) == NULL)
                          perror("getcwd() error");


          printf("Client Input[%d]: ", i);
          fgets(buf, sizeof(buf), stdin); nr = strlen(buf); 
          if (buf[nr-1] == '\n') { buf[nr-1] = '\0'; --nr; }

          if (strcmp(buf, "quit")==0) {
               printf("Bye from client\n"); exit(0);
          }

          if (nr > 0) {
		  if(isPwd(buf)) {
			  printf("%s\n",cwd);

		  }else if(isCd(buf)) {
		         continue;
		  }else if (isLDir(buf)) {
		  	pid_t dirn=fork();
			if(dirn>0) {
				wait(NULL);
			} else {
				execlp("ls","ls",NULL);
			}

		  
		  }
		  else if(isPut(buf)) {
			int fp;
				
			char *p=buf;
			 unsigned short size;
			p=p+4;
			  if((fp=open(p,O_RDONLY))<0) {
					  fprintf(stderr,"Error opening file\n");
					  perror("Error: ");
					  continue;
			 }
			  printf("fp %d\n",fp);
			struct stat stbuf;

			stat( p, &stbuf); 
			size=stbuf.st_size;
			writen(sd,buf,strlen(buf)+1);
			  printf("size is %d\n",size);		  
			 unsigned char message[MAX_BLOCK_SIZE];
			  bzero(message,MAX_BLOCK_SIZE);
			  message[0]='A';
			//message[1]=(size >> 8) & 0xFF;
			//message[2]=(size >> 0) & 0xFF;
			unsigned short sizen=htons(size);
			memcpy(message+1,(char *) &sizen,2);	
			printf("%c %d %d\n",message[0],(unsigned short)message[1],(unsigned short)message[2]);
			unsigned short size2=((message[1])<<8)|message[2];

			printf("%d network int",size2);
			  puts("wrote 'a'\n");
			  memcpy(message+3,p,strlen(buf)+1);
			  printf("%d %d %d\n",message[0],message[1],message[2]);
			  printf("%s file %s \n", message+3,p);
			  writen(sd,message,strlen(message+3)+3);
			  puts("wrote nsize\n");
				char opcode;
			  char asciichar;
			 
			  readn(sd,message,sizeof(message));
			  opcode=message[0];
			  asciichar=message[1];
			  if(opcode=='A' && asciichar=='0') 
				  puts("Server ready to accept\n");

			  int byte;
			  char buf2[MAX_BLOCK_SIZE];
			  printf("%d\n",fp);
			while((byte=read(fp,buf2,MAX_BLOCK_SIZE))>0) {
				printf("%d\n",byte);
				writen(sd,buf2,byte);
				bzero(buf2,MAX_BLOCK_SIZE);
			}
				if(byte==0) {
					puts("in terminate client");
				char *end=TERMINATE;
				writen(sd,end,strlen(end)+1);
				}
				else {
					char *errread="Error reading";
					writen(sd,errread,strlen(errread)+1);
				}
			
			 // write(sd,'A',1);
			
		
		  //nw=writen(sd,message,sizeof(message));
		  }else if(isGet(buf)) {
		  int fp;
		  char *p;
		  p=buf+4;
		  writen(sd,buf,strlen(buf)+1);
		  unsigned char message[MAX_BLOCK_SIZE];
		  unsigned short size;
		  bzero(message,MAX_BLOCK_SIZE);
		  readn(sd,message,MAX_BLOCK_SIZE);
		  size=ntohs((short)(message+1));
		  printf("cli size get %d\n",size);
		  if(message[0]=='A' && size>0) {
		  if((fp=open(p,O_WRONLY|O_CREAT|O_EXCL,0766))>0) {
			message[0]='A';
			message[1]='0';
			writen(sd,message,2);
			int byte;
			char buf2[MAX_BLOCK_SIZE];
			while((byte=readn(sd,buf2,MAX_BLOCK_SIZE))>0) {
				if(strcmp(buf2,TERMINATE)!=0)
					write(fp,buf2,byte);
				if(strcmp(buf2,TERMINATE)==0)
					break;
			}	
		  } else {
		  	
		  }
		  }
		  else {
		  	puts("file download failed\n");
		  }
				  
	 	 }else {
               if ((nw=writen(sd, buf, nr)) < nr) {
                    printf("client: send error\n"); exit(1); 
               }
               if ((nr=readn(sd, buf, sizeof(buf))) <=0) {
                    printf("client: receive error\n"); exit(1);
               }
               buf[nr] = '\0';
               printf("Sever Output[%d]: %s\n", i, buf);
	       bzero(buf,MAX_BLOCK_SIZE);
		  }
		  bzero(buf,MAX_BLOCK_SIZE);
          }
     }
}
int isGet(char *buf) {
	char p[] = "get ";
	if(strlen(buf)>4) {
		for(int i=0;i<4;i++) {
			if(buf[i]!=p[i])
				return 0;
		}
		return 1;
	}
	return 0;
}
int isPwd(char *buf) {
        char p[]="lpwd";
        if(strlen(buf)==4) {
                for(int i=0;i<4;i++) {
                        if(buf[i]!=p[i])
                                return 0;
                }
                return 1;
        }
        return 0;
}
int isDir(char *buf) {
        char p[]="dir";
        if(strlen(buf)==3) {
                for(int i=0;i<3;i++) {
                        if(buf[i]!=p[i])
                                return 0;
                }
                return 1;
        }
        return 0;
}
int isLDir(char *buf) {
         char p[]="ldir";
         if(strlen(buf)==4) {
                 for(int i=0;i<4;i++) {
                         if(buf[i]!=p[i])
                                 return 0;
                 }
                 return 1;
         }
         return 0;
 }
int isPut(char *buf) {
	char p[]="put";
	if(strlen(buf)>3) {
		for(int i=0;i<3;i++) {
			if(buf[i]!=p[i])
				return 0;
		}
		return 1;
	}
	return 0;
}

int isCd(char *buf) {
        int err;
        char p[]="lcd ";
        if(strlen(buf)>4) {
                for(int i=0;i<4;i++) {
                        if(buf[i]!=p[i])
                                return 0;
                }
                char *dir=buf+4;
		puts(dir);
                if((err=chdir(dir))==0)
                        return 1;
                else {
                        perror("Error: ");

                        return 1;
                }
        }
        return 0;
}

