#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>

int main()
{
	//declaring all nessisary components

	int pipefd1[2];
	int pipefd2[2];
	int returnstatus1;
	int returnstatus2;
	int pid1;
	int pid2;

	DIR *dir1;
	dir1 = opendir("d1");
	
	DIR *dir2;
	dir2 = opendir("d2");

	struct dirent *FileD1;
	struct dirent *FileD2;

	FILE *fp;
	FILE *fp2;

	char ReadNames[100];
	char ReadContents[1000];
	char ReadNames2[100];
	char ReadContents2[1000];

	char D1FileNames[100][100];
	char D1FileContents[100][1000];
	char D2FileNames[100][100];
	char D2FileContents[100][1000];

	char *line_buffer = NULL;
	size_t line_buf_size = 0;
	int line_count = 0;
	ssize_t line_size;

	char *line_buffer2 = NULL;
	size_t line_buf_size2 = 0;
	int line_count2 = 0;
	ssize_t line_size2;

	returnstatus1 = pipe(pipefd1);
	returnstatus2 = pipe(pipefd2);

	int filesd1 = 0;
	int filesd2 = 0;


	//error checking for opening of the pipes
	if(returnstatus1 == -1 || returnstatus2 == -1)
	{
		printf("Error 1: Unable to create pipe");

		return(1);

	}


	//error checking for opening the directories
	if(dir1 == NULL || dir2 == NULL)
	{
		perror("Unable to read directory");
		return(1);
	}

	//intitializing the first fork
	pid1 = fork();

	//This is the child process
	if(pid1 == 0)
	{
		//evaluating the directory
		while((FileD1 = readdir(dir1)))
		{
			//creating the path to open the files within the directory
			char FilePathd1[] = "/home/osc/Projects/Project1/d1/";
		
			strcpy(D1FileNames[filesd1], FileD1 -> d_name);
			
			strcat(FilePathd1, D1FileNames[filesd1]);

			fp = fopen(FilePathd1, "r");
			
			//error checking for opening the file
			if(fp == NULL)
			{
				perror("Error while opening file. \n");
				return(1);
			}
			
			//getting each line withing the file and adding it to the array
			line_size = getline(&line_buffer, &line_buf_size, fp);

			while (line_size >= 0)
			{
				line_count++;

				strcat(D1FileContents[filesd1], line_buffer);

				line_size = getline(&line_buffer, &line_buf_size, fp);
			
			}
			
			//freeing up the space
			free(line_buffer);
			line_buffer = NULL;	
			
			//incrementing the files and closing the file
			filesd1++;
			fclose(fp);
		
		}	

		//Now both lists should be completed for d1

		//Writing everything to pipe 1
		for (int x = 0; x < filesd1; x++)
		{
		  write(pipefd1[1], D1FileNames[x], sizeof(D1FileNames[x]));			 
		  write(pipefd1[1], D1FileContents[x], sizeof(D1FileContents[x]));
		}


		//Read names from pipe2, open file, fill with contents from pipe
		for (int y = 0; y < 2; y++)
		{

		  //getting the filepath so the new file can be created and opened
		  char FilePathd1[] = "/home/osc/Projects/Project1/d1/";
		  read(pipefd2[0], ReadNames, sizeof(ReadNames));

		  strcat(FilePathd1, ReadNames);

		  fp = fopen(FilePathd1, "w");	

		  //error checking to remove directory files
		  if(fp == NULL)
		  {
		    perror("File not found\n");		  
		    return(1);

		  }
		 
		  //reading contents from the pipe and adding it to the file
		  read(pipefd2[0], ReadContents, sizeof(ReadContents));

		  fprintf(fp, "%s" , ReadContents);
		
		}
		
		//closing the file and directory
		fclose(fp);
		closedir(dir1);
	}

	else
	{

		//doing the second fork command
		pid2 = fork();
		
		
		if(pid2 == 0)
		{
			
			//This is the second Child Process
			//iterate through the second directory
			while((FileD2 = readdir(dir2)))
			{

				//set up the filepath to open all files int he directory
				char FilePathd2[] = "/home/osc/Projects/Project1/d2/";

				strcpy(D2FileNames[filesd2], FileD2 -> d_name);

				strcat(FilePathd2, D2FileNames[filesd2]);

				fp2 = fopen(FilePathd2, "r");

				//error checking if the files dont exsist
				if(fp2 == NULL)
				{
					perror("error while opening file. \n");
					return(1);
				}

				//iterating through the file and getting the contents
				line_size2 = getline(&line_buffer2, &line_buf_size2, fp2);

				while(line_size2 >= 0)
				{
					line_count2++;
	
					strcat(D2FileContents[filesd2], line_buffer2);

					line_size2 = getline(&line_buffer2, &line_buf_size2, fp2);

				}
				
				//freeing up the excess space
				free(line_buffer);
				line_buffer = NULL;
	
				//incrementing files and closing the file
				filesd2++;

				fclose(fp2);
			}

			//Now both lists should be completed for d1

			//write all the data to the pipe
			for(int x = 0; x < filesd2; x++)
			{
			
			 write(pipefd2[1], D2FileNames[x], sizeof(D2FileNames[x]));
			 write(pipefd2[1], D2FileContents[x], sizeof(D2FileContents[x]));
				
			}

			//read the data from the pipe and add the files to the other directory
			for(int y = 0; y < 3; y++)
			{
				char FilePathd2[] = "/home/osc/Projects/Project1/d2/";		
				read(pipefd1[0], ReadNames2, sizeof(ReadNames2));
				
				strcat(FilePathd2, ReadNames2);

				fp2 = fopen(FilePathd2, "w");

				if(fp2 == NULL)
				{
					perror("File not found\n");
					return(1);
				}

				read(pipefd1[0], ReadContents2, sizeof(ReadContents2));

				fprintf(fp2, "%s", ReadContents2);
			}

			//close the file
			fclose(fp2);

		}
		//close the directory
		closedir(dir2);
	}
	
	return 0;
}
