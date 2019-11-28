
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
#include <errno.h>

int stat(const char *file_name, struct stat *buf);
int fstat(int filedes, struct stat *buf);
int lstat(const char *file_name, struct stat *buf);
DIR *opendir(const char *name);
struct dirent *readdir(DIR *dir);
void print_ls(bool found_l, bool found_i, bool found_n, struct stat s, char *file_name_);

	// C -a работает некорректно.

void do_ls(char *str, bool found_l, bool found_i, bool found_n, bool found_a, bool found_R)
{
	DIR *directory_name;
	struct dirent *dirent_dir; 

	if ((directory_name = opendir(str)))
	{
		printf("\n %s:	\n", str);
		do
		{
			if((dirent_dir = readdir(directory_name)) == NULL)
			{
				if (errno != 0) 
					perror("readdir");
				break;
			}

			if ((!(dirent_dir->d_name[0] == '.')) || found_a)
			{
				struct stat s;
				char *str_path_name = (char *) calloc(strlen(str) + strlen(dirent_dir->d_name) + 2, sizeof(char));
				strncpy(str_path_name, str, strlen(str)); //str_path_name = argv[i];
				strncat(str_path_name, dirent_dir->d_name, strlen(dirent_dir->d_name));  //'x'  ./Desktop/x;   "x";

				if (stat(str_path_name, &s) == -1)
				{
					perror("Stat fail");
					break;
				}

				print_ls(found_l, found_i, found_n, s, dirent_dir->d_name);
				free(str_path_name);
			}
		}
		while(dirent_dir);
	}
	else
	{
		if (errno != ENOTDIR) //((errno == EACCES) || (errno == EADDRNOTAVAIL) || (errno == ENOENT)) 
		{
			printf("ls: cannot open file: %s\n ", str);
					perror("opendir");
		}
	}

	closedir(directory_name);

	if ((directory_name = opendir(str)) && found_R)
	{
		do
		{
			if ((dirent_dir = readdir(directory_name)))
			{
				if ((!(dirent_dir->d_name[0] == '.')) || (found_a && ((dirent_dir->d_name[1] != '\0') && (dirent_dir->d_name[1] != '.')))) //|| (found_a && (dirent_dir->d_name[1] != '.')))
				{
					char *str_path_name = (char *) calloc(strlen(str) + strlen(dirent_dir->d_name) + 3, sizeof(char));
					strncpy(str_path_name, str, strlen(str));
					strncat(str_path_name, dirent_dir->d_name, strlen(dirent_dir->d_name)); //str_path_name = argv[i];
					strncat(str_path_name, "/", 1);	
					do_ls(str_path_name, found_l, found_i, found_n, found_a, found_R);
					free(str_path_name);
				}
			}
		}
		while(dirent_dir);
	}

	closedir(directory_name);
}



void print_ls(bool found_l, bool found_i, bool found_n, struct stat s, char *file_name_)
{
	const time_t *timep = &(s.st_atime);

	struct passwd *pwd_1 = getpwuid(s.st_uid);
	struct passwd *pwd_2 = getpwuid(s.st_gid);

	char root_name[] = "----------";

    if ( S_ISDIR(s.st_mode) )  root_name[0] = 'd';
    if ( S_ISCHR(s.st_mode) )  root_name[0] = 'c';
    if ( S_ISBLK(s.st_mode) )  root_name[0] = 'b';

    if ( s.st_mode & S_IRUSR ) root_name[1] = 'r';
    if ( s.st_mode & S_IWUSR ) root_name[2] = 'w';
    if ( s.st_mode & S_IXUSR ) root_name[3] = 'x';

    if ( s.st_mode & S_IRGRP ) root_name[4] = 'r';
    if ( s.st_mode & S_IWGRP ) root_name[5] = 'w';
    if ( s.st_mode & S_IXGRP ) root_name[6] = 'x';

	if ( s.st_mode & S_IROTH ) root_name[7] = 'r';
	if ( s.st_mode & S_IWOTH ) root_name[8] = 'w';
	if ( s.st_mode & S_IXOTH ) root_name[9] = 'x';

	char *time_str = ctime(timep);
	time_str[strlen(time_str)-1] = '\0';
	
	if (found_n)
	{
		if (found_i)
			printf("%ld %s %ld %d %d %6ld %s %s\n", s.st_ino, root_name, s.st_nlink, s.st_uid, s.st_gid, s.st_size, time_str, file_name_);
		else
			printf("%s %ld  %d %d %6ld %s %s\n", root_name, s.st_nlink, s.st_uid, s.st_gid, s.st_size, time_str, file_name_);
		return;
	}

	if (found_l)
	{
		if (found_i)
			printf("%ld %s %ld %s %s %6ld %s %s\n", s.st_ino, root_name, s.st_nlink, pwd_1->pw_name, pwd_2->pw_name, s.st_size, time_str, file_name_);
		else
			printf("%s %ld %s %s %6ld %s %s\n", root_name, s.st_nlink, pwd_1->pw_name, pwd_2->pw_name, s.st_size, time_str, file_name_);
		return;
	}

	if (found_i)
			printf("%ld %s\n", s.st_ino, file_name_);
	else
			printf("%s\n", file_name_);
	return;
} 

int main(int argc, char *argv[])
{
	struct stat s;

	bool found_a = false;
	bool found_l = false;
	bool found_i = false;
	bool found_n = false;
	bool found_R = false;

	int i;
	int opt;

	DIR *directory_name;
	struct dirent *dirent_dir; 
	


	while((opt = getopt(argc, argv, "lanidR")) != -1) 
	{
		switch(opt)
		{
			case 'a':
				found_a = true;
				break;
			case 'l':
				found_l = true;
				break;
			case 'i':
				found_i = true;
				break;
			case 'R':
				found_R = true;
				break;
			case 'n':
				found_n = true;
				break;
			default: 
				continue;
		}
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
			}
		}
	}

	for (i = optind; i < argc; i++)
	{
		do_ls(argv[i], found_l,  found_i, found_n, found_a, found_R);
		printf("\n");
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