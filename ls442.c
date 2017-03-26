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
	// Count the number of the files
	if((dir_p = opendir(dirstr)) != NULL)
	{
		while((readdir(dir_p)) != NULL)
			(*numOfFiles)++;
	}
	// Return the directory pointers 
	if((dir_p = opendir(dirstr)) != NULL)
	{
		return dir_p;
	}
	
}
// This function checks wheter given string(str) is substring of any string in the given string array(array)
int IsStrinArray(const char** array,char *str)
{
	int i = 1;

	// For each string in the array
	while(array[i])
	{
		// Check str is the substring of the string
		if(strstr(array[i], str) != NULL )
		{
			// If it is, return the index of larger string in the array
			return i;
		}
		i++;
	}
	// In no match case, return 0
	return 0;
}
// This function is written for the modularity of the printing results
void PrintResults(	struct stat fs,	struct dirent *file_p,const char* commands[])
{
	// Checks for the hidden file or not
	if(file_p->d_name[0] == '.' &&  IsStrinArray(commands,"a") ==0)
		return;
	else
	{
		// Print inode number
		if(IsStrinArray(commands,"i") != 0)
		{
			printf("%-8d ",(int)fs.st_ino);
		}

		// Print permissions. (This code block is directly taken from 
		// http://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c)
		// It basically takes the permission by operating AND between predefined masks and file mode variable
		// For example, st_mode is 8bit number as 10010101 in binary
		// and S_IRUSR is a 8 bit mask as 00000001 in binary
		// The and operation of them masks the related bit to user read permission
		// In this case that would output 1 and condition ?...:... selects the first choice ("r")
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
		
		// Print hardlinks
		printf(" %d ",(int)fs.st_nlink );
		
		// Checks -g command and if it is not entered, prints the owner name
		if(IsStrinArray(commands,"g") == 0)
		{
			struct passwd * userinfo;
			userinfo = getpwuid(fs.st_uid );
			printf("%-8s ",userinfo -> pw_name);
		}
		// Checks -G command and if it is not entered, prints the group name
		if(IsStrinArray(commands,"G") == 0)
		{
			struct group * groupinfo;
			groupinfo = getgrgid(fs.st_gid);
			printf("%-8s ",groupinfo->gr_name );
		}

		// Print the size
		printf("%-8d ",(int)fs.st_size );

		// Print the modification time in proper format
		char buffer[7];
		struct tm * timeinfo;
		timeinfo = localtime(&fs.st_mtime);
		strftime(buffer,7,"%d %b",timeinfo );
		printf("%-8s ", buffer);

		// Checks -Q command and if it is entered, puts a quotation marks
		if(IsStrinArray(commands,"Q") != 0)
			printf("\"%s\" \n", file_p -> d_name);
		else
			printf("%s \n", file_p -> d_name);
	}
}
// This function is used for lowering all the characters in a string
// It is necessary for the alphabetical sorting purposes
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
// Alphabetical selection sort is implemented here
void AlphabeticalSorting(struct dirent **file_p_arr,struct stat *fs_arr,int numOfFiles,const char **commands)
{
	// Initilize temporary and counter variables
	struct dirent * file_p_temp;
	struct stat fs_temp;
	int i,j;

	// For each file in the directory
	for(i = 0; i < numOfFiles; ++i)
		for (j = i+1; j < numOfFiles; ++j)
		{
			// Check ith file name is before from jth file name in the dictionary
			if(strcmp(StrToLower(file_p_arr[i]->d_name),StrToLower(file_p_arr[j]->d_name)) > 0)
			{
				// If it is swap their places
				file_p_temp = file_p_arr [i];
				file_p_arr[i] = file_p_arr[j];
				file_p_arr[j] = file_p_temp;

				fs_temp = fs_arr [i];
				fs_arr[i] = fs_arr[j];
				fs_arr[j] = fs_temp;					 
			}
		}
}
// Sorting by size is implemented here
// Selection sort is again preferred due to its ease to implement
void SortBySize(struct dirent **file_p_arr,struct stat *fs_arr,int numOfFiles,const char **commands)
{
	// Initilize temporary and counter variables
	struct dirent * file_p_temp;
	struct stat fs_temp;
	int i,j;
	// For each file
	for(i = 0; i < numOfFiles; ++i)
	{
		for (j = i; j < numOfFiles; ++j)
		{
			// Compare the sizes
			if(fs_arr[i].st_size < fs_arr[j].st_size)
			{
				// if ith is smaller from jth
				// Swap them
				file_p_temp = file_p_arr [i];
				file_p_arr[i] = file_p_arr[j];
				file_p_arr[j] = file_p_temp;

				fs_temp = fs_arr [i];
				fs_arr[i] = fs_arr[j];
				fs_arr[j] = fs_temp;					 
			}
			// In case of same size, make a alphabetical orders
			else if(fs_arr[i].st_size == fs_arr[j].st_size)
			{
				// Check ith file name is before from jth file name in the dictionary

				if(strcmp(StrToLower(file_p_arr[i]->d_name),StrToLower(file_p_arr[j]->d_name)) > 0)
				{

					// If it is swap their places

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

	// For taking directory other than current directory
	if(DirVar = IsStrinArray(argv,"/") )
	{
		// If there is directory in the commands 
		// Parse that directory
		dirstr = argv[DirVar];
		// Remove from command list
		argv[DirVar] = "/";
		// Take the directory pointer related to this directory
		dir_p = ListOfFiles(&numOfFiles,dirstr);
	}
	else 
	{
		// Default current directory
		dir_p = ListOfFiles(&numOfFiles,"./");
	}
	// Read files one by one

	// File and status arrays for the found files
	struct dirent **file_p_arr;
	struct dirent *file_p;
	struct stat *fs_arr;
	struct stat fs;

	// Initialization of counters and flags
	int i = 0;
	int j = 0;
	int k = 0;
	int comnum = 1;
	int SortF = 0;
	int UnsortF = 0;

	// Defined constant variables and pointers for the comparison
	char const char_U = 'U';
	char const char_S ='S';
	
	char*  UnsortP;
	char*  SortP;

	// Dynamic memory allocation for the file and status arrays
	file_p_arr =(struct dirent **)malloc(numOfFiles*sizeof(struct dirent *));
	fs_arr = (struct stat *)malloc(numOfFiles*sizeof(struct stat));

	// Loop until all files are read
	while((file_p = readdir(dir_p)) != NULL)
	{
		// Memory allocation for the full path 
		char *path = (char*)malloc(sizeof(file_p->d_name)+sizeof(dirstr));
		// Create a full path for current file
		strcpy(path,dirstr);
		strcat(path,file_p->d_name);

		// Take file statistics for each file in current directory
		if ( stat(path,&fs) < 0)
		{
			perror("Could not read the files");
			return 1;
		}

		// Put the file and status to the arrays
		file_p_arr[i] = file_p;
		fs_arr[i] = fs;
		// Increment counter
		i++;

		// Deallocate the path variable
		free(path);
	}

	// If Unsort is not required, sort in alphabetic
	if(IsStrinArray(argv,"U") == 0)
		AlphabeticalSorting(file_p_arr,fs_arr,numOfFiles,argv);

	// Loop until commands are finished
	while(argv[comnum] != NULL)
	{
		// Strrchr finds the last occurence of a constant char in a char array
		// It returns a char pointer that points the last occured position
		if(UnsortP = strrchr(argv[comnum],(char_U)) )
		{
			// Unsort command is occured
			UnsortF = comnum;
		}

		if (SortP = strrchr(argv[comnum],(char_S)) )
		{
			// Sort command is occured
			SortF = comnum;
		}
		comnum++;
	}
	// Sort occured last 
	if(SortF > UnsortF)
		SortBySize(file_p_arr,fs_arr,numOfFiles,argv);
	// Sort occured last in same command
	else if (SortF == UnsortF && SortP > UnsortP )
		SortBySize(file_p_arr,fs_arr,numOfFiles,argv);

	// Print Results
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
		// Reverse 
		k = numOfFiles-1;
		while(k > -1)
		{
			PrintResults(fs_arr[k],file_p_arr[k],argv);
			k--;
		}
	}
	// Deallocate the dynamic arrays
	free(file_p_arr);
	free(fs_arr);
	return 0;
}