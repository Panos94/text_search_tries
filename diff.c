#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

int main(char * name1, char * name2){
	
	char * wordIn = malloc(128*sizeof(char));
	int difs=0;
	char * wordOut = malloc(128*sizeof(char));
	FILE* in = fopen("small.result", "r"); //opening input file
	//strcpy(buffer,"\0");

	FILE* out = fopen("results.txt", "r"); //opening input file
	//strcpy(buffer,"\0");

	if(in == NULL){
		perror("Error opening input file");
		return -1;
	}
	if(out == NULL){
		perror("Error opening output file");
		return -1;
	}

	char *lineIn = NULL;
	char *lineOut = NULL;
	size_t len = 0;
	ssize_t read;
	char *word;
	int line=0;
	while ((read = getline(&lineIn, &len, in)) != -1){
		line++;
		read = getline(&lineOut, &len, out);
		wordIn = strtok (lineIn,"|");
		wordOut = strtok (lineOut,"|");
		while((wordIn!=NULL)&&(wordOut!=NULL))
		{
			if(strcmp(wordIn,wordOut)!=0){
				printf("Difference\n");
				printf("Words: \"%s\" <-> \"%s\" \n",wordIn,wordOut);
				difs++;
				printf("In line %d\n",line);
				return -1;
			}
			//
			wordIn=strtok(NULL,"|");
			wordOut=strtok(NULL,"|");
		}
	}
free(lineIn);
free(lineOut);
printf("Found %d differences\n",difs);
return 0;
}
