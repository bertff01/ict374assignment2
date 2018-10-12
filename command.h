struct CommandStruct {
	int first;
	int last;
	char *sep;
	char **argv;
	char *stdin_file;
	char *stdout_file;

};
typedef struct CommandStruct Command;

#define MAX_NUM_COMMANDS  1000

// command separators
#define pipeSep  "|"                            // pipe separator "|"
#define conSep   "&"                            // concurrent execution separator "&"
#define seqSep   ";"                            // sequential execution separator ";"
void setNull(Command *commands,int size);
void searchRedirection(char *token[], Command *cp);
int separateCommands(char *tokens[],Command command[]);
