#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>    //тут есть неюзаемые библиотеки, лень редачить
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>



typedef struct cmd cmd_t;

struct cmd
{
  char *argv_str[128];
}; 

const int MAX_SIZE_ARGV = 256;


void get_proc (int in, int out, cmd_t *cmd)
{
	pid_t pid;

	if ((pid = fork ()) == 0)
	{
		if (in != STDIN_FILENO)
		{
			dup2 (in, STDIN_FILENO);
			close (in);
		}

		if (out != STDOUT_FILENO)
		{
			dup2 (out, STDOUT_FILENO);
			close (out);
		}

		if (execvp(cmd->argv_str [0], (char * const *)cmd->argv_str) == -1)
			perror("Cmd not found");
	}
}



void do_pipes (int n, cmd_t *cmd)
{
	int i;
	pid_t pid;
	int in, fd [2];

	in = STDIN_FILENO; //first process

	for (i = 0; i < n -1; ++i)
	{
		pipe (fd);
		get_proc (in, fd [1], cmd + i);
		close (fd [1]);
		in = fd [0];
	}
	if ((pid = fork ()) == 0)  //last process
	{
		if (in != STDIN_FILENO)
		{
			dup2 (in, STDIN_FILENO);
			close(in);
			//printf("HELLO I AM HERE\n");
		}
		if (execvp (cmd [i].argv_str [0], (char * const *)cmd [i].argv_str) == -1)
			perror("Cmd not found");
	}
}



int main()
{
while (1)
	{
		int i = 0; 
		int j = 0;
			pid_t pid;

		int LENGHT_ARGV = 0;
		int count_commands = 0;
		char *s = (char *) calloc(MAX_SIZE_ARGV, sizeof(char));
		cmd_t *command = (cmd_t *) calloc(128, sizeof(cmd_t));

		//printf ("\nmyshell$:"); //dont work

		while (!fgets(s, MAX_SIZE_ARGV, stdin))
		{
			//printf ("\nmyshell$:");
		}


		LENGHT_ARGV = strlen(s);
		s[LENGHT_ARGV - 1] = '\0';


		char *supp = NULL;

		command[j].argv_str[i++] = strtok(s, " ");

		while (supp = strtok(NULL, " "))
		{
			if(supp[0] == '|')
			{
				j++;
				i = 0;
				continue;
			}
			command[j].argv_str[i] = supp;
			command[j].argv_str[i+1] = NULL;
		
			i++;
		}

		if (j == 0)
		{
			//printf("not |\n");
			if ((pid = fork ()) == 0)
			{
				if (execvp (command [j].argv_str [0], (char * const *)command [j].argv_str) == -1)
				perror("Cmd not found");
			}
			free(s);
			free(command);
			continue;
		}

		command[j].argv_str[i++] = supp;
		command[j].argv_str[i++] = NULL;

		command = (cmd_t *) realloc(command, j);

		count_commands = j+1;

		do_pipes (count_commands, command);
		wait(NULL);
		free(s);
		free(command);
	}
}

