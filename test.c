#include <stdio.h>
#include <errno.h>
#include <glob.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "token.h"
#include "command.h"
#define MAXLINE 256

char **glob_pattern(char *buf,int *l);


int main() {
	char buf[MAXLINE];
	char cwd[1028];
	char prompt[256];
	prompt[0]='%';
	prompt[1]='\0';
	while(1) {
		if (getcwd(cwd, sizeof(cwd)) == NULL)
			perror("getcwd() error");
		printf("%s ",prompt);
		bzero(buf,MAXLINE);
		if(fgets(buf,MAXLINE,stdin)>0){

			if(strlen(buf)>0)
				buf[strlen(buf)-1]='\0';

			if(isPrompt(prompt,buf)) {
				continue;
			}
			if(isPwd(buf)){
				printf("%s\n",cwd);
				continue;
			}
			if(isCwd(buf)) {
				continue;
			}

			//testGlob(buf);
			//continue;
			if(strcmp(buf,"exit")==0) {
				exit(0);
			}
			char *token[MAX_NUM_TOKENS];
			bzero(token,MAX_NUM_TOKENS);
			int tokens=tokenise(buf,token);
			tokens=expandGlob(tokens,token);
			Command commands[MAX_NUM_COMMANDS];

			setNull(commands,MAX_NUM_COMMANDS);
			int commandsn=separateCommands(token,commands);

			int i;

			int pipes[commandsn][2];
			char *myfifo="/tmp/myfifo";
			unlink(myfifo);
			if (mkfifo(myfifo,0777)<0)
				perror("can't make it");
			int fd_in=0;
			int p[2];
			for(i=0;i<commandsn;i++) {
				pipe(p);
				if(strcmp(commands[i].sep,"|")==0) {
                                        if(pipe(pipes[i])==-1)
						 printf("pipe failed");

				}

				pid_t pid;
				if((pid=fork())== -1) {
					exit(EXIT_FAILURE);
				}
				if(pid==0) {
					int fd;
					int fd2;
					if(i!=0) {
						if(((strcmp(commands[i-1].sep,"|")==0)&&(commands[i].stdout_file!=NULL))){
							printf("Error: Cannot pipe and direct file input");
							return (1);
						}}

					if(commands[i].stdin_file!=NULL) {



						fd=open(commands[i].stdin_file,O_RDONLY);
						if(fd<0){
							perror("Error: ");
							return (1);
						}
						
						close(fd);
					}
					if(strcmp(commands[i].sep,"|")==0) {
						//if(fd=open(myfifo,O_WRONLY)<0)
						//	perror("Error: ");
						close(pipes[i][0]);
						//fprintf(stderr,"process %d output pipe",i);

						dup2(p[1],STDOUT_FILENO);
						//close(pipes[i][0]);
						//close(pipes[i][0]);
					}
					if(i!=0) {
						if(strcmp(commands[i-1].sep,"|")==0) {
							//if(fd=open(myfifo,O_RDONLY)<0)
						close(pipes[i-1][1]);
							//	perror("Error: ");
							dup2(fd_in,STDIN_FILENO);
							//close(pipes[i-1][1]);
						//	close(pipes[i][1]);

						}
					}
					if(commands[i].stdout_file!=NULL) {
						fd2=open(commands[i].stdout_file,O_WRONLY|O_CREAT,0642);
						if(fd2<0) {
							perror("Error: ");
							return  (1);
						}
						dup2(fd2,STDOUT_FILENO);
						close(fd2);
					}
					int nullfile=open("/dev/null",O_WRONLY);
					if(strcmp(commands[i].sep,"&")==0)
						dup2(nullfile,STDOUT_FILENO);
						fflush(stderr);
					execvp(token[commands[i].first],commands[i].argv);
					exit(1);
				}
				if(pid>0) {
					//close(pipes[i][0]);
					//close(pipes[i][1]);
					wait(NULL);
					close(p[1]);
					fd_in=p[0];
				}
				if((pid>0 && (i==(commandsn-1))||(strcmp(commands[i].sep,";")==0))) {
					int status;
					//wait(NULL);

				}
			}
			setTokenNull(token,tokens);
			bzero(commands,sizeof(Command)*commandsn);
			commandsn=0;
		}
	}
}
int isCwd(char *buf) {
	int err;
	char p[]="cd ";
	if(strlen(buf)>3) {
		for(int i=0;i<3;i++) {
			if(buf[i]!=p[i])
				return 0;
		}
		char *dir=buf+3;
		if((err=chdir(dir))==0)
			return 1;
		else {
			perror("Error: ");

			return 1;
		}
	}
	return 0;
}
void setTokenNull(char **token,int num) {
	for(int i=0;i<num;i++) {

		free(token[i]);
	}
	free(token);
}
int expandGlob(int tokens,char **token) {
	char *temp[MAX_NUM_TOKENS];
	int i;
	int newtokencount=0;
	for(i=0;i<tokens;i++) {
		int num;
		char **gl=glob_pattern(token[i],&num);
		int p=0;
		for(p=0;p<num;p++) {
			temp[newtokencount++]=gl[p];
		}
	}
	for(i=0;i<newtokencount;i++) {
		token[i]=temp[i];
	}



	return newtokencount;
}





char **glob_pattern(char *wildcard,int *num)
{
	char **gfilename;
	size_t cnt, length;
	glob_t glob_results;
	char **p;
	length=0;

	glob(wildcard, GLOB_NOCHECK, 0, &glob_results);
	gfilename=malloc(sizeof(char *)*glob_results.gl_pathc); 
	/* How much space do we need?  */
	*num=glob_results.gl_pathc;
	/* Allocate the space and generate the list.  */
	bzero(gfilename,(sizeof(char *)*glob_results.gl_pathc));
	int i;
	p=glob_results.gl_pathv;
	for (i=0;i<glob_results.gl_pathc;i++)
	{
		gfilename[i]=malloc(strlen(glob_results.gl_pathv)+1);
		bzero(gfilename[i],strlen(glob_results.gl_pathv)+1);
		strcpy(gfilename[i], p[i]);
	}

	globfree(&glob_results);
	return gfilename;
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
int isPrompt(char *prompt, char *buf) {
	char p[]="prompt ";
	for(int i=0;i<7;i++) {
		if(buf[i]!=p[i])
			return (0);
	}
	int z=0;
	for(int i=7;i<strlen(buf);i++) {
		prompt[z++]=buf[i];
	}
	return 1;
}
