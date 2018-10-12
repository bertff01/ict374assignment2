/*
 *  ser6.c  - 	(Topic 11, HX 22/5/1995)
 *		An improved version of "ser5.c". This version handles the message boundaries
 *              which are not preserved by the TCP. Each message transmitted between the 
 *              client and the server is preceeded by a two byte value which is the length
 *              of the message. The handling of the message length is done in routines readn
 *              and writen.	
 *  revised:	22/05/1996
 *  revised:	18/10/2006
 */

#include  <stdlib.h>     /* strlen(), strcmp() etc */
#include  <stdio.h>      /* printf()  */
#include  <string.h>     /* strlen(), strcmp() etc */
#include  <errno.h>      /* extern int errno, EINTR, perror() */
#include  <signal.h>     /* SIGCHLD, sigaction() */
#include  <syslog.h>
#include  <sys/types.h>  /* pid_t, u_long, u_short */
#include  <sys/socket.h> /* struct sockaddr, socket(), etc */
#include  <sys/wait.h>   /* waitpid(), WNOHAND */
#include  <netinet/in.h> /* struct sockaddr_in, htons(), htonl(), */
/* and INADDR_ANY */
#include  "stream.h"     /* MAX_BLOCK_SIZE, readn(), writen() */
#include <dirent.h>
#include <fcntl.h>

#define   SERV_TCP_PORT   40005   /* default server listening port */


void claim_children()
{
	pid_t pid=1;

	while (pid>0) { /* claim as many zombies as we can */
		pid = waitpid(0, (int *)0, WNOHANG); 
	} 
}


void daemon_init(void)
{       
	pid_t   pid;
	struct sigaction act;

	if ( (pid = fork()) < 0) {
		perror("fork"); exit(1); 
	} else if (pid > 0) {
		printf("Hay, you'd better remember my PID: %d\n", pid);
		exit(0);                  /* parent goes bye-bye */
	}

	/* child continues */
	setsid();                      /* become session leader */
	chdir("/");                    /* change working directory */
	umask(0);                      /* clear file mode creation mask */

	/* catch SIGCHLD to remove zombies from system */
	act.sa_handler = claim_children; /* use reliable signal */
	sigemptyset(&act.sa_mask);       /* not to block other signals */
	act.sa_flags   = SA_NOCLDSTOP;   /* not catch stopped children */
	sigaction(SIGCHLD,(struct sigaction *)&act,(struct sigaction *)0);
	/* note: a less than perfect method is to use 
	   signal(SIGCHLD, claim_children);
	   */
}



void reverse(char *s)
{
	char c;   
	int i, j = strlen(s);

	for (i=0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i]; s[i] = s[j]; s[j] = c;
	}
}


void serve_a_client(int sd)
{   int nr, nw;
	char buf[MAX_BLOCK_SIZE];
	char cd[MAX_BLOCK_SIZE]="Changed Directory";
	char cwd[MAX_BLOCK_SIZE];

	while (1){
		/* read data from client */
		if (getcwd(cwd, sizeof(cwd)) == NULL)
			perror("getcwd() error");

		if ((nr = readn(sd, buf, sizeof(buf))) <= 0) 
			return;   /* connection broken down */
		buf[nr]='\0';
		puts(buf);
		if(isCd(buf)) {
			puts("in cd");
			nw=writen(sd,cd,sizeof(cd));
		}else if(isPwd(buf)) {
			puts("in pwd");
			nw=writen(sd,cwd,sizeof(buf));
		}

		else if(isDir(buf)) {
			puts("in dir");

			DIR *dir;
			struct dirent *ent;
			char *temp,full[MAX_BLOCK_SIZE];
			bzero(full,MAX_BLOCK_SIZE);
			if ((dir = opendir (cwd)) != NULL) {
				/* print all the files and directories within directory */
				int count=1;
				while ((ent = readdir (dir)) != NULL) {
					strcat(full,ent->d_name);
					strcat(full," ");
					if(count++%3==0)
						strcat(full,"\n");


				}
				nw=writen(sd,full,sizeof(full));
				bzero(full,MAX_BLOCK_SIZE);
				closedir (dir);
			}
		}else if(isPut(buf)) {
				
			unsigned char message[MAX_BLOCK_SIZE];
				puts("in put");	
				char opcode;
				unsigned short size;
				char *filename;
				filename=buf+4;
				int ns=readn(sd,message,sizeof(message));
				char *p=message;
				p=p+3;
				opcode=message[0];
				size=ntohs((short)message+1);
				filename=p;
				printf("filename %s\n",p);
				printf(" %d %d %d\n",message[0],message[1],message[2]);
				message[0]='A';
				message[1]='1';

				int fd;
				if((fd	=open(p,O_CREAT|O_WRONLY|O_EXCL,0766))<0)
							message[1]='1';
				else
					message[1]='0';
				printf("%d\n",fd);
				writen(sd,message,sizeof(message));
				int n;
				char buf2[MAX_BLOCK_SIZE];
				
				while((n=readn(sd,buf2,sizeof(buf2)))>0) {
					printf("%d\n",n);
					if(strcmp(buf2,TERMINATE)!=0)
						write(fd,buf2,n);
					if(strcmp(buf2,TERMINATE)==0) {
						puts("in terminate server");	
						break;
					}
				}
				bzero(buf2,MAX_BLOCK_SIZE);

								

				

			}
		else if(isGet(buf)) {
			puts("in get\n");
			char *inget="in get";
			int fp;
			char *p=buf;
			unsigned short size;
			p=p+4;
				printf("size %d\n",size);
			if((fp=open(p,O_RDONLY))<0) {
				char *ferror="Error opening file";
			
			}
			printf("%d file point\n",fp);
			unsigned char message[MAX_BLOCK_SIZE];
			if(fp>0) {
			printf("fp %d\n",fp);
			struct stat stbuf;
			stat(p,&stbuf);
			size=stbuf.st_size;
			printf("size %d\n",size);
			bzero(message,MAX_BLOCK_SIZE);
			message[0]='A';
			unsigned short sizen=htons(size);
			memcpy(message+1,(char *) &sizen,2);
			writen(sd,message,3);
			readn(sd,message,MAX_BLOCK_SIZE);
			if(message[0]=='A' && message[1]=='0') {
				int byte;
				char buf2[MAX_BLOCK_SIZE];
				while((byte=read(fp,buf2,MAX_BLOCK_SIZE))>0) {
					writen(sd,buf2,byte);
					bzero(buf2,MAX_BLOCK_SIZE);
				}
				if(byte==0) {
					puts("in terminate server");
					char *end=TERMINATE;
					writen(sd,end,strlen(end)+1);
				} else {
					char *errread="Error reading";
					writen(sd,errread,strlen(errread)+1);
				}
			}
			}else {
				message[0]='Z';
				message[1]=0;
				message[2]=0;
				writen(sd,message,3);
			}
			
			
		} else {
			char *def="Message not recognised on server\n";
			writen(sd,def,strlen(def)+1);
		}
		
		bzero(buf,sizeof(buf));
		/* process data */

		/* send results to client */
	} 
}
int isGet(char *buf) {
	char p[]="get ";
	if(strlen(buf)>4) {
		for(int i=0;i<4;i++) {
			if(buf[i]!=p[i])
				return 0;
		}
		return 1;
	}
	return 0;
}
int isPut(char *buf) {
	char p[]="put ";
	if(strlen(buf)>4) {
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
int isPwd(char *buf) {
	char p[]="pwd";
	if(strlen(buf)==3) {
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
	char p[]="cd ";
	if(strlen(buf)>3) {
		for(int i=0;i<3;i++) {
			if(buf[i]!=p[i])
				return 0;
		}
		char *dir=buf+3;
		if((err=chdir(dir))==0){
			printf("changed dir %s\n",dir);
			return 1;
		}
		else {
			perror("Error: ");

			return 1;
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int sd, nsd, n;  
	pid_t pid;
	unsigned short port;   // server listening port
	socklen_t cli_addrlen;  
	struct sockaddr_in ser_addr, cli_addr; 

	/* get the port number */
	if (argc == 1) {
		port = SERV_TCP_PORT;
	} else if (argc == 2) {
		int n = atoi(argv[1]);   
		if (n >= 1024 && n < 65536) 
			port = n;
		else {
			printf("Error: port number must be between 1024 and 65535\n");
			exit(1);
		}
	} else {
		printf("Usage: %s [ server listening port ]\n", argv[0]);     
		exit(1);
	}

	/* turn the program into a daemon */
	//     daemon_init(); 

	/* set up listening socket sd */
	if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("server:socket"); exit(1);
	} 

	/* build server Internet socket address */
	bzero((char *)&ser_addr, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);
	ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	/* note: accept client request sent to any one of the
	   network interface(s) on this host. 
	   */

	/* bind server address to socket sd */
	if (bind(sd, (struct sockaddr *) &ser_addr, sizeof(ser_addr))<0){
		perror("server bind"); exit(1);
	}

	/* become a listening socket */
	listen(sd, 5);

	while (1) {

		/* wait to accept a client request for connection */
		cli_addrlen = sizeof(cli_addr);
		nsd = accept(sd, (struct sockaddr *) &cli_addr, &cli_addrlen);
		if (nsd < 0) {
			if (errno == EINTR)   /* if interrupted by SIGCHLD */
				continue;
			perror("server:accept"); exit(1);
		}

		/* create a child process to handle this client */
		if ((pid=fork()) <0) {
			perror("fork"); exit(1);
		} else if (pid > 0) { 
			close(nsd);
			continue; /* parent to wait for next client */
		}

		/* now in child, serve the current client */
		close(sd); 
		serve_a_client(nsd);
		exit(0);
	}
}
