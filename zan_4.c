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

typedef int mtype;

struct my_msgbuf
{
	long mtype;
	int mtext;
} message;

const int length = sizeof(struct my_msgbuf) - sizeof(mtype); //длина читаемого сообщения

int run(int id, struct my_msgbuf *a) // возвращаемым значением функции является порядковый номер слудющего бегущего (i+1)
{

	if (msgrcv(id, a, length, 0,  0) == -1)    //читаем следующие сообщение из очереди
	{
		perror("error msgrcv"); 
		return -3;	
	}

	a->mtext = a->mtext + 1;

	if (msgsnd(id, a, length, 0) == -1)  //заносим сообщение с i+1 "палочкой"(хуялочкой)
	{
		perror("error msgsnd");
		return -3;
	}

	printf(" %d \n", a->mtext);  //здесь я просто проверяю, все адекватно робит можно убрать

	return a->mtext;
} 



void judge_end(int id)
{
	if ((msgctl(id, IPC_RMID, 0)) == -1)  //удалить очередь сообщений
	{
		perror ("mgctl error");
	}
	printf("competition end");  //judje выносит вердикт 
}

int main(int argc, char *argv[])
{
	int N = atoi(argv[1]); 
	int i;
	int pid = 1;
	int id;
	int msgkey;

	msgkey = ftok(".", 100);  //формирование ключа

	message.mtype = 1;

	if ((id = msgget(msgkey, IPC_CREAT | 0660)) == -1)   //формирование очереди
	{
		perror("msssget error");
		return -2;
	}

	//msgget();

/* judge create */
	pid = fork();

	if (pid == -1)
	{
		perror("pid error");
		return -1;
	}

	if (pid == 0)
	{
		message.mtext = 1;

		if (msgsnd(id, &message, length, 0) == -1)  //"даём палку" первому бегуну
		{
			perror("error msgsnd");
			return -3;
		}	
	}
	//pid = fork();
	//if (pid == -1)
	//{
	//	perror("pid error");
	//	return -1;
	//}

	for (i = 1; i <= N; i++)
	{

		if (pid != 0)
		{
			pid = fork();

			if (pid == 0)
			{
				//printf ("%d %d \n", i, N);
				message.mtext = i;

				if (run(id, &message) == N + 1)  //следующие сообщение было адресовано N+1 -> выходим, вызываем конец соревнования
				{
					judge_end(id);
					return 0;
				}
				wait(NULL);
				return 0;
			}
		}

	}
	wait(NULL);
}