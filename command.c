#include <string.h>
#include <stdio.h>
#include "command.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// return 1 if the token is a command separator
// return 0 otherwise
//

void buildCommandArgumentArray(char *token[],Command *cp) {
	int n=(cp->last-cp->first +1);
	if(cp->stdin_file!=NULL)
		n=n-2;
	if(cp->stdout_file!=NULL)
		n=n-2;
	n=n+1;
	if((cp->argv=realloc(cp->argv,sizeof(char *) * n))==NULL)
		fprintf(stderr,"Error allocating");
	int i;
	int k=0;
	for (i=cp->first; i<= cp->last;++i) {
		if(strcmp(token[i],">")==0 || strcmp(token[i],"<")==0)
			++i;
		else {
			cp->argv[k]=token[i];
			++k;
		}
	}
	cp->argv[k]=NULL;
}
void setNull(Command *commands,int size) {
	for(int i=0;i<MAX_NUM_COMMANDS;i++) {
                         commands[i].first=0;
                         commands[i].last=0;
                         commands[i].sep=NULL;
                         commands[i].argv=NULL;
			 commands[i].stdin_file=NULL;
			 commands[i].stdout_file=NULL;
	}

}
int separator(char *token)
{
    int i=0;
    char *commandSeparators[] = {pipeSep, conSep, seqSep, NULL};

    while (commandSeparators[i] != NULL) {
        if (strcmp(commandSeparators[i], token) == 0) {
            return 1; 
        } 
        ++i;
    }
    
    return 0;
}
void searchRedirection(char *token[], Command *cp) {
	int i=0;
	int first=cp->first;
	int last=cp->last;
	for(i=first;i<last;i++) {
		if(strcmp(token[i],">")==0) {
			if((i+1)<=last)
				cp->stdout_file=token[i+1];
		} else if(strcmp(token[i],"<")==0) {
			if((i+1)<=last)
				cp->stdin_file=token[i+1];
		}
	}
}
// fill one command structure with the details
//
void fillCommandStructure(Command *cp, int first, int last, char *sep)
{
     cp->first = first; 
     cp->last = last - 1;
     cp->sep = sep; 
}

int separateCommands(char *token[], Command command[])
{
     int i;
     int nTokens;

     // find out the number of tokens
     i = 0;
     while (token[i] != NULL) ++i; 
     nTokens = i;

     // if empty command line
     if (nTokens == 0) 
          return 0;

     // check the first token
     if (separator(token[0]))
          return -3;

     // check last token, add ";" if necessary 
     if (!separator(token[nTokens-1])) {
          token[nTokens] = seqSep;
          ++nTokens;
     }
          
     int first=0;   // points to the first tokens of a command
     int last;      // points to the last  tokens of a command
char *sep;     // command separator at the end of a command
     int c = 0;         // command index
     for (i=0; i<nTokens; ++i) {
         last = i;
         if (separator(token[i])) {
             sep = token[i];
             if (first==last)  // two consecutive separators
                 return -2;
             fillCommandStructure(&(command[c]), first, last, sep);
     	searchRedirection(token,&command[c]);
	buildCommandArgumentArray(token,&command[c]);
	++c;
             first = i+1; 
         }
     }

     // check the last token of the last command  
     if (strcmp(token[last], pipeSep) == 0) { // last token is pipe separator
          return -4; 
     } 

     // calculate the number of commands
     int nCommands = c;

     return nCommands; 
}
