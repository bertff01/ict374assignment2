#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "token.h"
int tokenise (char *inputLine, char *token[]) {
	int size=strlen(inputLine);
	
	int i=0;
	int tokenpos=0;
	char *temp;
	int ntoken=0;
	char **tokenz;
	tokenz=token;

	while(tokenz[i]!=NULL)
		i++;
	ntoken=i;
	
	int length=strlen(inputLine);
	if(length>1) {
		int tokens=1;
		token[0]=inputLine;
	for(i=0;i<length;i++) {
		if(tokens>=MAX_NUM_TOKENS)
			return (-1);
		if(inputLine[i]=='\n' || inputLine[i]==' ' || inputLine[i]=='\t') {
			token[tokens]=inputLine+i+1;
			inputLine[i]='\0';
			tokens++;
		}
	}
	return(tokens);
	}
	return(-1);
}
