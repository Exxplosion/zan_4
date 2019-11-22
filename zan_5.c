
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>    //тут есть неюзаемые библиотеки, лень редачить
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <stdbool.h>
#include <pwd.h>



//#define L
//#define l
//#define N
//#define n
//#define I 
//#define i

//#define PRINT_LS(L,l,A,a,N,n,I,i)\
	printf("(l)   (n)   (i)  %s\n",(L) (N) (i) dirent_dir->d_name); \

//-l -a -n -i -d -R
//struct DIR;
int stat(const char *file_name, struct stat *buf);
int fstat(int filedes, struct stat *buf);
int lstat(const char *file_name, struct stat *buf);
DIR *opendir(const char *name);
struct dirent *readdir(DIR *dir);


char * settime(struct tm *u)
{
  char s[40];
  char *tmp;
  for (int i = 0; i<40; i++) s[i] = 0;
  int length = strftime(s, 40, "%d.%m.%Y %H:%M:%S, %A", u);   //функция для перевода дохуища секунд в адекватный формат времени
  tmp = (char*)malloc(sizeof(s));
  strcpy(tmp, s);
  return(tmp);
}


int main(int argc, char *argv[])
{
	struct passwd *pwd_1, *pwd_2; //для определения имени пользователя и группы (без них выводит как с функцией -n)
	bool found_a = false;
	char *root_name = (char *) malloc(10*sizeof(char)); //переводит числовое значчение рут дотсупа в строчное (см далее)
	int size_name; //длина имени выводимого файла (не нужно)
	struct tm *u; //для времени
	char *f;
	time_t timer;
	int i;
	DIR *directory_name;
	struct dirent *dirent_dir; 
	struct stat s; //= (stat *) malloc(sizeof(stat));
	int opt;
	//char *opts = "a:l:n:i:d:R:i:";	//char *opts = «a:b:o:»; /

	while((opt = getopt(argc, argv, "lanidR:")) != -1) 
	{
		switch(opt)
		{
			case 'a':
				found_a = true;
				break;
			case 'l':
				//#define L 
				//#define l %s
				break;					//ТУТ ПОКА ЧТО ВСЁ МЕРТВО
			case 'i':
				break;
			case 'd':
				break;
			case 'R':
				break;
			default: 
				continue;
		}//*/
	}

	for (i = optind; i < argc; i++)
	{
		if (!(directory_name = opendir(argv[i])))
		{
			if((stat(argv[i], &s)) == -1)
			{
				printf("%s", argv[i]);
				perror("Not direcroty or file");
			}
			else
			{
				printf("%s \n", argv[i]);
				//printf("%ld", s.st_ino);
			}
		}
	}

	for (i = 1; i < argc; i++)
	{
		if ((directory_name = opendir(argv[i])))
		{
			printf("%s:	\n", argv[i]);
			dirent_dir = readdir(directory_name);
			stat(dirent_dir->d_name, &s);
			timer = s.st_atime;
			u = localtime(&timer);
			f = settime(u);
			do
			{
				if((!(dirent_dir->d_name[0] == '.')) || found_a)
				{
					pwd_1 = getpwuid(s.st_uid);
					pwd_2 = getpwuid(s.st_gid);
					size_name = strlen(dirent_dir->d_name);
					switch(s.st_mode)
					{
						case 33204:
							root_name = "-rw-rw-r--";
							break;
						case 16893:
							root_name = "drwxrwxr-x"; //ЗДЕСЬ ЕЩЕ НЕ ВСЁ
							break;
						default :
							;
					}//самое сложное - понять что в принтфе....
					printf("%10ld %s %s %s %ld %30s %.*s\n", s.st_ino, root_name, pwd_1->pw_name, pwd_2->pw_name, s.st_size, f, size_name, dirent_dir->d_name); //ПРимерный вывод с опцией -l
				}
				dirent_dir = readdir(directory_name);

				if (stat(dirent_dir->d_name, &s) == -1)
				{
					printf("/%s/ SUKA SHVAL\n",dirent_dir->d_name); //ОТРЫКРЫТИЕ В ЧАС НОЧИ - СТАТ ОТКРЫВАЕТ ФАЙЛЫ ТОЛЬКО В ДИРЕКТОРИИ, В КОТОРОЙ НАХОДИТСЯ ПРОГРАММА!!!()
					perror("SUKA BYAD\n");							//(откуда она запускается) НУЖНО КАЖДЫЙ РАЗ ПЕРЕХОДИТЬ В ДИРЕКТОРИИ КОТОРЫЙ ПИШЕТ ПОЛЬЗОВАТЕЛЬ И ОТТУДА
				}													//УЖЕ ВЫВОДИТЬ ИНФУ о ФАЙЛЕ (наверно такое решение)

				timer = s.st_atime;
				u = localtime(&timer);
				f = settime(u);
			}
			while (dirent_dir);
			printf("\n");
		}
	}
}

//struct stat {
//    dev_t         st_dev;      /* устройство */
//    ino_t         st_ino;      /* inode */
//    mode_t        st_mode;     /* режим доступа */
//    nlink_t       st_nlink;    /* количество жестких ссылок */
//    uid_t         st_uid;      /* идентификатор пользователя-владельца */
//    gid_t         st_gid;      /* идентификатор группы-владельца */
//    dev_t         st_rdev;     /* тип устройства */
//                              /* (если это устройство) */
//    off_t         st_size;     /* общий размер в байтах */
//    blksize_t     st_blksize;  /* размер блока ввода-вывода */
//                               /* в файловой системе */
//    blkcnt_t      st_blocks;   /* количество выделенных блоков */
//    time_t        st_atime;    /* время последнего доступа */
//    time_t        st_mtime;    /* время последней модификации */
//    time_t        st_ctime;    /* время последнего изменения */
//};