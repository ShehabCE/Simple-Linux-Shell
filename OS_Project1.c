//Author: Shehab Abdel-Salam
//Operating Systems Course: Project 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <string.h>

char *mygetline(FILE *f);		//Reading the buffer
char **insertstring( char **margv, char *string );
int arraysize(char **margv);
char **parse( char *line);		//parsing the arguments of the command

#define MAX_COMMANDS 50		//Maximum number of commands to accept

int main(int argc, char**argv){

    char *line;
    char **args;
    int stat;	//status of parent process.
    int is_concurrent = 0;	//Initialised for no concurrency unless command specifies.
    int should_run = 1;		//flag 
    int Command_count = 1; 	//The current ith Command
    char *myhistory[MAX_COMMANDS] = {NULL};	//Static array of commands initialised with NULL
    char *rm;
    do{
     is_concurrent = 0;
     printf("My Shell->");
     line=mygetline(stdin);	//read the buffer
     if((rm = strchr(line, '&')) != NULL)
	{
	is_concurrent = 1;	//Concurrency enabled!
	*rm = '\0';
	}
		

	//If Command passes validation, add it to the history array.
     if( ((args=parse(line)) != NULL) && (strcmp(args[0],"exit") != 0) && (strcmp(args[0],"history") != 0) 
		&& (args[0][0] != '!') )
	{
		if(Command_count <= MAX_COMMANDS) 
		{
			//store the command in the history array and increment counter
			myhistory[Command_count]= line;
			Command_count += 1;
		}
		else
		{
			printf("Exceeded the maximum number of commands to execute!\n");
			printf("My Shell-> Bye!\n");
			return 0;
		}
	}
	//new lines.
     if((args=parse(line)) == NULL)
        continue; //Empty line
	
	//exit command.
     if(strcmp(args[0],"exit") == 0 ){
	printf("My Shell-> Bye!\n");
	return 0;
     }

	//If the user entered (history), show the most recent 10 commands.
     if(strcmp(args[0],"history") == 0 )
    {
	int start = 1, end = Command_count;
	if(end>=12)
		start = end - 10;
	if(end == start)
		printf("No commands found in history. \n");
        while(end >= start)
	{	
		if(myhistory[end] == NULL)
			end--;

		else
		{
			printf("%d %s \n", end, myhistory[end]);
			end--;			
		}
	}
     }

	//If the user entered (!!), show the most recent command.
     if(strcmp(args[0],"!!") == 0 )
	{
		if(Command_count == 1)
			printf("No commands found in history.\n");
		else
			printf("%d %s \n", Command_count-1, myhistory[Command_count-1]);
	}

		//If the user entered (!ith), show the ith command.
     if(args[0][0] == '!' && args[0][1] != '!')
	{	//convert to integer for index of the command. (From ASCII Table)
		int index = args[0][1] - '0';
		if(index < 1 || index > MAX_COMMANDS)
			printf("Command number out of range from 1 to %d.\n", MAX_COMMANDS);
		if(myhistory[index] == NULL)
			printf("No such command in history.\n");
		else
			printf("%d %s \n", index, myhistory[index]);
	}

     if(fork() == 0) {
        execvp(args[0],args);
	return 0;
	}

     if(is_concurrent == 0)
     	wait(&stat);
     //else, concurrency is enabled so parent process won't wait.

    }while(1);

	return 0;
}



char *mygetline(FILE * f){

	char c,*line=NULL;
	int size=0;
	//Keep reading characters
	for(;;){
	   c=(char)fgetc(f);
	   if(c == EOF){
		break;
	   }
		//if character is end of line. Re-allocate new size.
	   if( c == '\n' ){
		line=realloc(line,size+2);
		line[size]=c;
		line[size+1]='\0';
		break;
	   }
	   line=realloc(line,size+2);
	   line[size]=c;
	   line[size+1]='\0';
	   size=strlen(line);
	}

	return line;
}


char **insertstring(char **margv, char *string ){
	int margc=0;
	if(margv != NULL)
	     while(margv[margc] != NULL )margc++;
	      
	  if ((margv = (char **)realloc( margv,(margc + 2)*sizeof(char *))) == NULL){
		fprintf(stderr,"error allocating memory in realloc\n");
		exit(1);
	  }
	  margv[margc]=string;
	  margv[margc+1]=NULL;
    
    
    
	  return margv;
}

int arraysize(char **margv){
	int margc=0;
	if(margv != NULL)
	while(margv[margc] != NULL )
		margc++;
	return margc;
}

char ** parse(char *line){
	  char *argstring = strdup(line);
	  char **margv=NULL;
    
	  if(strtok(argstring," \t\n") == NULL ){
	     //empty line or new line.
	      return margv; //return NULL
	  }
    
		//else do a loop of inserting the string arguments
	  margv=insertstring(margv,argstring);
	  while(( argstring=strtok(NULL," \t\n"))!= NULL){
	    margv=insertstring(margv,argstring);
	  }
    
    //while (*margv) {
    //    printf("%s\n", *margv++);
    //}
	  return margv;
}
