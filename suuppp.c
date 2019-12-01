




if ((directory_name = opendir(argv[i])))
{
	printf("%s:	\n", argv[i]);
	do
	{
		if((dirent_dir = readdir(directory_name)) == NULL)
		{
			break;
		}

		if ((!(dirent_dir->d_name[0] == '.')) || found_a)
		{
			void do_ls(char *str, char *file_name); //добавить кол - во жестких ссылок
										//str - argv[i], 
			{
				char *str_path_name = (char *) calloc(strlen(argv[i]) + strlen(dirent_dir->d_name) + 2, sizeof(char));
				strncpy(str_path_name, argv[i], strlen(argv[i])); //str_path_name = argv[i];
				strncat(str_path_name, dirent_dir->d_name, strlen(dirent_dir->d_name));  //'x'  ./Desktop/x;   "x";

				if (stat(str_path_name, &s) == -1)
				{
					printf("/%s/ AAA\n", dirent_dir->d_name); 
					perror("Stat fail");
				}

				const time_t *timep = &(s.st_atime);

				struct passwd *pwd_1 = getpwuid(s.st_uid);
				struct passwd *pwd_2 = getpwuid(s.st_gid);

				strcpy( root_name, "----------" );

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

				printf("%10ld %s %s %s %ld %s %s\n", s.st_ino, root_name, pwd_1->pw_name, pwd_2->pw_name, s.st_size, ctime(timep), dirent_dir->d_name); //ПРимерный вывод с опцией -l
				free(str_path_name);
			}
		}
}