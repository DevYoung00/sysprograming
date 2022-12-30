#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_USER_IN 100
#define MAX_NUM_ARGS 5
#define MAX_ARG_LEN 20

//put string user types 
static char in_buf[MAX_USER_IN];
//parse the string to arguments
static char parse_args[MAX_NUM_ARGS][MAX_ARG_LEN];

//add
static int cmd_index = 0;
static int cmd_is = 0;

//given code; keyboard input to buffer
int user_in ();
//your implementation: buffer (1t arg) to parsed args (2nd arg)
int parse_cmd (char* in_buf, char parse_args[MAX_NUM_ARGS][MAX_ARG_LEN]);
//your implementation: execute using parsed args (2nd args)
void execute_cmd (int num_args, char parse_args[MAX_NUM_ARGS][MAX_ARG_LEN]);


void main()
{
	int num_args;

	while((num_args = user_in()) != EOF)
	{
		execute_cmd(num_args, parse_args);
	}
	printf("\n");
}


int user_in ()
{
	int c, i;
	char cwd[1024];

	//initialization
	int count = 0;
	memset(in_buf, 0, MAX_USER_IN);
	for(i=0; i<MAX_NUM_ARGS; i++) {
		memset(parse_args[i], 0, MAX_ARG_LEN);
	}

	//display current directory prompt
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		printf("%s$ ", cwd);

	//fill in_buf from user input
	while(1)
	{
		//get a character 
		c = getchar();
		in_buf[count] = c;
	
		//ctrl+D, then exit
		if(c  == EOF) {
			return EOF;
		}
	
		//return, then stop user input
		if(c == '\n') {
			in_buf[count] = '\0';
			break;
		}

		count ++;
	}

	//call parse_cmd()
	int num_args = parse_cmd(in_buf, parse_args);

	return num_args;
}


/**
  * get a string (in_buf) and parse it into multiple arguments (parse_args) 
  * input: in_buf (1st arg)
  * output1: parse_args (2nd args)
  * output2: num_args (return val)
**/
int parse_cmd(char *in_buf, char parse_args[MAX_NUM_ARGS][MAX_ARG_LEN])
{
	int i = 0;
	int num_args = MAX_NUM_ARGS;

	char *tok;

	printf("input:%s\n", in_buf);


		tok = strtok(in_buf, " ");
		while(tok!=NULL){
			if(strcmp(tok, "|") == 0){
				cmd_index = i;
				cmd_is = 1;}
			if(strcmp(tok, "cd") == 0){
				cmd_index = i;
				cmd_is = 2;}
			if(strcmp(tok, "&") == 0){
			       	cmd_index = i;
				cmd_is = 3;}

			strcpy(parse_args[i++],tok);
			tok = strtok(NULL, " ");
		}
		num_args = i;

	printf("output:\n");
	for(i=0; i<num_args; i++) {
		printf("arg[%d]:%s\n", i, parse_args[i]);
	}
	return num_args;

}

/**
  * get parsed args (parse_args) and execute using them
  * input1: num_args (1st arg)
  * input2: parse_args (2nd args)
**/
void execute_cmd(int num_args, char parse_args[MAX_NUM_ARGS][MAX_ARG_LEN])
{
	int i = 0;
	int j=0;
	int status;
	char *args[num_args+1];
	pid_t pid;
	int p[2];
	char *pipe_cmd1[num_args+1];
	char *pipe_cmd2[num_args+1];
	for (;i < num_args;i++)
		args[i] = parse_args[i];

	args[i] = NULL;

	//create pipe	
        if(pipe(p) < 0){
                printf("pipe create error\n");}

	// pipe
	if(cmd_is == 1){
		for(;j<cmd_index; j++)
                	pipe_cmd1[j] = args[j];
		
		pipe_cmd1[j] = NULL;
		
		j=0;
        	for(;j<(num_args-cmd_index)-1; j++)
                	pipe_cmd2[j] = args[j + (cmd_index+1)];	
		pipe_cmd2[j] = NULL;

		pid = fork();
		switch(pid) //first cmd
        	{
                case -1:
                        perror("Fork Error");
                        break;
                case 0:
			dup2(p[1],1);

			close(p[0]);
			close(p[1]);

			if((execvp(pipe_cmd1[0], pipe_cmd1)<0)){
				perror("execvp error\n");}
			break;
			}

			
		pid = fork();
		switch(pid) //second cmd
                {
                case -1:
                        perror("Fork Error");
                        break;
                case 0:
			dup2(p[0],0);

			close(p[0]);
		 	close(p[1]);	

			if((execvp(pipe_cmd2[0], pipe_cmd2)) <0){
				perror("execvp error\n");}
                        break;
				
                }

		close(p[0]);
		close(p[1]);
		waitpid(-1,&status,0);
	} 
	//cd
	else if(cmd_is ==2) 
	{
		if(chdir(args[1]) < 0){
			printf("chdir error\n");
		}
	}

	//background
	else if(cmd_is == 3){
		pid = fork();
		switch(pid) 
                {
                case -1:
                        perror("Fork Error");
                        break;
                case 0:
                        execvp(args[cmd_index], args);
                        break;
                default:
                        waitpid(-1,&status,WNOHANG);
                	printf("%d\n",pid);
		}
	
	}	
}
