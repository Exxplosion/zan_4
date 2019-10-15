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


#define WRITE_END 1
#define READ_END 0

int main()
{
	int i = 1;
	int j = 1;
	int pid = 12;				//!!!!!Пока что работает только с одним "|" !!!!
	char *s;
	int pipefd[2];				//Это все взято у Альберта и аппнуто до требуемой проги
	int b = 1000;
	char *cmd[b]; //команды и опции до |
	char *cmd2[b]; //соотвественно после |
	//char cmd1[] = "hello";
	//char *cmd2 = cmd1;
	char c;

	for(;;)
	{
		printf ("$");

		s = (char *) calloc(1000, sizeof(char));  // Не обязательно, можно сразу выделить в обьявлении

		fgets(s, 999, stdin);


		for (int m = 0; m<999; m++)  //это костыль, но очень сука важный костыль. Гетс считывает и \n, он не нужен!!(кому надо обьясню)
		{
			if (s[m] == '\n')
			{
				s[m] = '\0';
			}
		}

		cmd[0] = strtok(s, " ");

		while ((cmd[i] = strtok(NULL, " ")) != NULL)    //здесь формируются cmd и сmd2
		{
			if (*cmd[i] == '|')
			{
				cmd[i] = (char *) NULL; // без этого вгонит | в поток второго

				cmd2[0] = strtok(NULL, " ");

				while ((cmd2[j] = strtok(NULL, " '\n'")) != NULL)
				{
					j++;
				}

			//	printf("cmd2 ,%s, \n", cmd2[0]);  //Проверочка

				break;
			}

			//ungetc(c, 0);
			i++;
		}

		//printf("Failed to execute2printf,%s, ,%s,", cmd2[0], cmd[0]);

		//b = i + 1;
		if (pipe(pipefd) < 0) { //делаем канал
			perror("pipe");
			return -1;
		}

		pid = fork(); 
		if (pid == 0)
		{
			dup2(pipefd[WRITE_END], STDOUT_FILENO);   //очевидно эти строки взяты с stackoverflow...
			close(pipefd[READ_END]);
    		close(pipefd[WRITE_END]);
			execvp(cmd[0], cmd);
			fprintf(stderr, "Failed to execute1 %s", cmd[0]);
			exit(1);
		}
		
		pid = fork();
		//cmd1[10] = 
		if (pid == 0)
		{
        	dup2(pipefd[READ_END], STDIN_FILENO);
        	close(pipefd[WRITE_END]);
        	close(pipefd[READ_END]);
			execvp(cmd2[0], cmd2);
			fprintf(stderr, "Failed to execute2 ,%s, ,%s,", cmd2[0], cmd[0]);
    		exit(1);
		}

		int status;
        close(pipefd[READ_END]);
        close(pipefd[WRITE_END]); //тоже важный момент, тоже стек
        waitpid(pid, &status, 0);
		//free(s);
		i = 1;
		j = 1;
	//	wait(NULL);
	}
}