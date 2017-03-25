#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <string.h>
// Opens directory and returns directory pointers
DIR* ListOfFiles(int* numOfFiles, char const *dirstr)
{
	DIR *dir_p;
	if((dir_p = opendir(dirstr)) != NULL)
	{
		while((readdir(dir_p)) != NULL)
			(*numOfFiles)++;
	}
	if((dir_p = opendir(dirstr)) != NULL)
	{
		return dir_p;
	}
	
}
int IsStrinArray(const char** array,char *str)
{
	int i = 1;

	while(array[i])
	{
			
		if(strstr(array[i], str) != NULL )
		{
			return i;
		}
		i++;
	}
	return 0;
}
void PrintResults(	struct stat fs,	struct dirent *file_p,const char* commands[])
{
	if(file_p->d_name[0] == '.' &&  IsStrinArray(commands,"a") ==0)
		return;
	else
	{
		if(IsStrinArray(commands,"i") != 0)
		{
			printf("%-8d ",(int)fs.st_ino);
		}

		printf( (S_ISDIR(fs.st_mode)) ? "d" : "-");
		printf( (fs.st_mode & S_IRUSR) ? "r" : "-");
		printf( (fs.st_mode & S_IWUSR) ? "w" : "-");
		printf( (fs.st_mode & S_IXUSR) ? "x" : "-");
		printf( (fs.st_mode & S_IRGRP) ? "r" : "-");
		printf( (fs.st_mode & S_IWGRP) ? "w" : "-");
		printf( (fs.st_mode & S_IXGRP) ? "x" : "-");
		printf( (fs.st_mode & S_IROTH) ? "r" : "-");
		printf( (fs.st_mode & S_IWOTH) ? "w" : "-");
		printf( (fs.st_mode & S_IXOTH) ? "x" : "-");
		
		printf(" %d ",(int)fs.st_nlink );
		
		if(IsStrinArray(commands,"g") == 0)
		{
			struct passwd * userinfo;
			userinfo = getpwuid(fs.st_uid );
			printf("%-8s ",userinfo -> pw_name);
		}
		if(IsStrinArray(commands,"G") == 0)
		{
			struct group * groupinfo;
			groupinfo = getgrgid(fs.st_gid);
			printf("%-8s ",groupinfo->gr_name );
		}

		printf("%-8d ",(int)fs.st_size );

		char buffer[7];
		struct tm * timeinfo;
		timeinfo = localtime(&fs.st_mtime);
		strftime(buffer,7,"%d %b",timeinfo );
		printf("%-8s ", buffer);
		if(IsStrinArray(commands,"Q") != 0)
			printf("\"%s\" \n", file_p -> d_name);
		else
			printf("%s \n", file_p -> d_name);
	}
}
char * StrToLower(char* str)
{
	char* out;
	out =(char *)malloc(sizeof(str));
	int i = 0;

	while(i < (int)sizeof(str) )
	{
		out[i] = tolower(str[i]);
		i++;
	}

	return out;
}
void AlphabeticalSorting(struct dirent **file_p_arr,struct stat *fs_arr,int numOfFiles,const char **commands)
{
	struct dirent * file_p_temp;
	struct stat fs_temp;
	int i,j;

	for(i = 0; i < numOfFiles; ++i)
		for (j = i+1; j < numOfFiles; ++j)
		{
			//printf("%d %d \n",i,j );
			if(strcmp(StrToLower(file_p_arr[i]->d_name),StrToLower(file_p_arr[j]->d_name)) > 0)
			{
				file_p_temp = file_p_arr [i];
				file_p_arr[i] = file_p_arr[j];
				file_p_arr[j] = file_p_temp;

				fs_temp = fs_arr [i];
				fs_arr[i] = fs_arr[j];
				fs_arr[j] = fs_temp;					 
			}
		}


}
void SortBySize(struct dirent **file_p_arr,struct stat *fs_arr,int numOfFiles,const char **commands)
{
	struct dirent * file_p_temp;
	struct stat fs_temp;
	int i,j;

	for(i = 0; i < numOfFiles; ++i)
	{
		for (j = i; j < numOfFiles; ++j)
		{
			if(fs_arr[i].st_size < fs_arr[j].st_size)
			{
				file_p_temp = file_p_arr [i];
				file_p_arr[i] = file_p_arr[j];
				file_p_arr[j] = file_p_temp;

				fs_temp = fs_arr [i];
				fs_arr[i] = fs_arr[j];
				fs_arr[j] = fs_temp;					 
			}
			else if(fs_arr[i].st_size == fs_arr[j].st_size)
			{
				if(strcmp(StrToLower(file_p_arr[i]->d_name),StrToLower(file_p_arr[j]->d_name)) > 0)
				{
					file_p_temp = file_p_arr [i];
					file_p_arr[i] = file_p_arr[j];
					file_p_arr[j] = file_p_temp;

					fs_temp = fs_arr [i];
					fs_arr[i] = fs_arr[j];
					fs_arr[j] = fs_temp;					 
				}
			}
		}
	}
	
}

int main(int argc, char const *argv[])
{
	// Create a directory pointer for current directory
	int numOfFiles = 0;
	int DirVar;
	DIR *dir_p;
	char const *dirstr;
	if(DirVar = IsStrinArray(argv,"/") )
	{
		//dir_p = opendir("~/Downloads");
		//dir_p = opendir(argv[DirVar]);
		dirstr = argv[DirVar];
		argv[DirVar] = "/";
		dir_p = ListOfFiles(&numOfFiles,dirstr);

	}
	else 
	{
		dir_p = ListOfFiles(&numOfFiles,"./");
	}
	// Read files one by one
	struct dirent **file_p_arr;
	struct dirent *file_p;
	struct stat *fs_arr;
	struct stat fs;
	int i = 0;
	int j = 0;
	int k = 0;
	int comnum = 1;
	int SortF = 0;
	int UnsortF = 0;
	char const char_U = 'U';
	char const char_S ='S';
	
	char*  UnsortP;
	char*  SortP;


	file_p_arr =(struct dirent **)malloc(numOfFiles*sizeof(struct dirent *));
	fs_arr = (struct stat *)malloc(numOfFiles*sizeof(struct stat));
	while((file_p = readdir(dir_p)) != NULL)
	{

		char *path = (char*)malloc(sizeof(file_p->d_name)+sizeof(dirstr));
		strcpy(path,dirstr);
		//printf("%s\n",file_p->d_name );
		strcat(path,file_p->d_name);
		//printf("%s\n",path );
		// Take file statistics for each file in current directory
		if ( stat(path,&fs) < 0)
		{
			perror("Could not read the files");
			return 1;
		}
		file_p_arr[i] = file_p;
		fs_arr[i] = fs;
		i++;
		//printf("i:%d\n",i );
		free(path);
	}
	//printf("%s\n",file_p_arr[94]->d_name );

	if(IsStrinArray(argv,"U") == 0)
		AlphabeticalSorting(file_p_arr,fs_arr,numOfFiles,argv);

	while(argv[comnum] != NULL)
	{

		if(UnsortP = strrchr(argv[comnum],(char_U)) )
		{
			UnsortF = comnum;
			// Continue to other command
		}
		if (SortP = strrchr(argv[comnum],(char_S)) )
		{
			// Sorting by size
			SortF = comnum;
		}
		comnum++;
	}
	if(SortF > UnsortF)
		SortBySize(file_p_arr,fs_arr,numOfFiles,argv);
	else if (SortF == UnsortF && SortP > UnsortP )
		SortBySize(file_p_arr,fs_arr,numOfFiles,argv);


	if (IsStrinArray(argv,"r") == 0)
	{
		while(k < numOfFiles)
		{
			PrintResults(fs_arr[k],file_p_arr[k],argv);
			k++;
		}
	}

	else 
	{
		k = numOfFiles-1;
		while(k > -1)
		{
			PrintResults(fs_arr[k],file_p_arr[k],argv);
			k--;
		}
	}
	free(file_p_arr);
	free(fs_arr);
	return 0;
}