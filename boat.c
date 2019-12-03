#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <sys/sem.h>


int main(int ac, char* av[])
{
	int k = atoi(av[1]);//количество мест
	int m = atoi(av[2]);//количество человек
	int n = atoi(av[2]);//число поездок ботика

	if (k > m)
	{
		k = m;//если мест больше чем людей, то количество мест= количеству людей
	}

	int c=0;
	int idsem = semget(IPC_PRIVATE,6,IPC_CREAT | 0777);//инициализируем 6 семафоров
	pid_t pid;
	enum sem{freeplaces,freeplaces2,bareer,progulka,katat,end};//названия семафоров
	struct sembuf posadka[3] = {
	{end,0,0},
	{freeplaces,-1,0},//если есть свободные места
	{bareer,-1,0}//если барьер открыт-закрыть
	};
	struct sembuf barlock ={bareer,-1,0};//закрыть барьер
	struct sembuf barunlock ={bareer,1,0};// открыть
	struct sembuf boatwait[3] ={
	{freeplaces,0,0},//чтобы не было свободных мест
	{freeplaces2,0,0},//чтобы на лодке не было покатавшихся людей
	{bareer,-1,0}// поднять трап
	};
	struct sembuf endtrip={end,1,0};//костыль на конец поездки

	struct sembuf kat={katat,k,0};
	struct sembuf katopen={katat,0,0};
	struct sembuf katclose={katat,-1,0};

	struct sembuf progwait = {progulka,0,0};
	struct sembuf progplus = {progulka,1,0};
	struct sembuf progminus = {progulka,-1,0};

	struct sembuf free2wait={freeplaces2,0,0};//чтобы на лодке не было покатавшихся людей
	struct sembuf freeplplus = {freeplaces2,1,0};
	struct sembuf freeplminus = {freeplaces2,-1,0};
	struct sembuf freeplus = {freeplaces,1,0};
	struct sembuf freeplusextra = {freeplaces,k,0};
	semop(idsem,&freeplusextra,1);

	//passangers
	for(int i = 1;i <= atoi(av[2]);i++){
	pid = fork();
	if (pid == 0){
	while(1){
	if((semop(idsem,posadka,3))==-1)//если есть свободные места, если барьер открыт, если не конец поездки
	{
	return 0;
	}
	printf("зашёл на борт\n");
	semop(idsem,&barunlock,1);
	semop(idsem,&progwait,1);//ждут отправления
	printf("кататься\n");
	semop(idsem,&freeplplus,1);//считает сколько покатались
	semop(idsem,&katclose,1);//вычитает пока все не покатаются
	semop(idsem,&barlock,1);
	printf("сойти на берег\n" );
	if((semop(idsem,&freeplminus,1))==-1)//вычитает пока все кто катался не сойдут на берег
	{
	return 0;
	}

	semop(idsem,&freeplus,1);//прибавляет количество свободных мест при выходе
	semop(idsem,&barunlock,1);

	}

	}

	}

	//boat

	pid=fork();
	if (pid==0){
	for (int k = 0;k < (atoi(av[3])+1);k++)
	{
	semop(idsem,&katopen,1);//семафор, который не дает лодке приплыть раньше , чем дети покатаются
	semop(idsem,&progplus,1);//семафор, который не дает кататься
	printf("приплыл\n");
	printf("опустить трап\n");
	semop(idsem,&barunlock,1);//опускаю трап
	if (k<atoi(av[3]))
	{
	semop(idsem,boatwait,3);//описано выше
	printf("поехали\n");
	semop(idsem,&kat,1);//сколько должны покататься
	semop(idsem,&progminus,1);//отправление
	}
	if(k==atoi(av[3])){
	semop(idsem,&endtrip,1);// лодка причаливает и выпускает пассажиров
	}
	}
	semop(idsem,&free2wait,1);//ждет пока все сойдут и уплывает
	printf("Прощайте:(\n");
	printf("Я ухожу, так будет лучше для всех(((((\n");
	semctl(idsem,IPC_RMID,0);//удаляем семафоры
	return 0;
	}

	for (int g=0;g<(m+1);g++)//ждем смерти пассажиров и лодки
	{
	wait(NULL);
	}

	return 0;
}