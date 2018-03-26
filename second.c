#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#define IF_DIR 4096

typedef struct _F {
char name[30];
char fullname[100];
int mode;
unsigned long int size;
} F;

int n = 0;
F *allfiles = 0;
int mode;

void Sort()
{
	if (n>1)
	{
		int min;
		F temp;
		for (int i = 0; i<n-1; i++)
		{
			min = i;
			for (int j = i+1; j<n; j++)
 				if ((mode && strcmp(allfiles[j].name, allfiles[min].name)<0) || (!mode && allfiles[j].size < allfiles[min].size))
					min = j;
			if (min > i)
			{
				temp = allfiles[min];
				allfiles[min] = allfiles[i];
				allfiles[i] = temp;
			}
		}	
	}
}

void Scan(const char *dict)
{
	DIR *dir = opendir(dict);
	if (dir != 0)
	{		
	char *name;
	struct dirent *info1 = readdir(dir);
	struct stat info2;
	while (info1 != 0)
	{
		name = info1->d_name;
		if (strlen(name) > 2)
		{
			char fullname[100];
			strcpy(fullname, dict);
			strcat(fullname, "/");
			strcat(fullname, name);
			stat(fullname, &info2);
			//printf("%s / %d / %o\n", fullname, n, info2.st_mode & 511);
			if (S_ISDIR(info2.st_mode))
				Scan(fullname);
			else
			{
				++n;
				if (n==1)
					allfiles = (F *)malloc(sizeof(F));
				else
					allfiles = (F *)realloc(allfiles, sizeof(F)*n);
				strcpy(allfiles[n-1].name, name);
				strcpy(allfiles[n-1].fullname, fullname);
				allfiles[n-1].mode = info2.st_mode & 511;
				allfiles[n-1].size = info2.st_size;
			}
		}
		info1 = readdir(dir);
	} 
	closedir(dir);
	}
}

void CreateFiles(const char *dict)
{
	char buffer[512];
	int count;
	for (int i = 0; i<n; ++i)
	{
		char inName[100];
		strcpy(inName, dict);
		strcat(inName, "/");
		strcat(inName, allfiles[i].name);
		FILE *in = fopen(allfiles[i].fullname, "rb");
		FILE *out = fopen(inName, "w");
		while ((count = read(fileno(in), buffer, sizeof(buffer))) > 0)
			write(fileno(out), buffer, count);
		fclose(in);
		fclose(out);
		struct stat info2;
		stat(inName, &info2);
		printf("%d\n", info2.st_ino);
	}
}

int main(int numc, char *argv[])
{
	if (numc != 4)
		printf("Неверное число парамтеров\n");
	else
	{
		mode = atoi(argv[2]) - 1;
		Scan(argv[1]);
		printf("..............\n");
		Sort();
		CreateFiles(argv[3]);
		for (int i = 0; i<n; i++)
			printf("%s / %o / %d\n", allfiles[i].name, allfiles[i].mode, allfiles[i].size);
	}
	return 0;
}
