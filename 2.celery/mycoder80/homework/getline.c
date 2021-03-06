#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define GETLEN		5


ssize_t mygetline(char **lineptr, size_t *n, FILE *stream)
{
	int ret, cnt = 0;
	int len = GETLEN;
	int i;
	char ch;

	if(*lineptr == NULL) {
		*lineptr = malloc(len);
		if(NULL == *lineptr) {
			return -10;
		}
		memset(*lineptr, 0x00, len);
		*n = len;
	}
	
	while(1) {
		ch = fgetc(stream);
		if(ch == EOF){
			printf("EOF\n");
			return -30;
		}
		if(ch == '\n'){
			printf("nnnn\n");
			break;
		}
		cnt++;
	}

	while(1) {
		if(cnt < *n) {
			fseek(stream, -(cnt+1), SEEK_CUR);
			fgets(*lineptr, *n, stream);
			ret = cnt+1;
			break;
		}else {
			len += GETLEN;
			*lineptr = realloc(*lineptr, len);
			if(NULL == *lineptr) {
				return -20;
			}
			*n = len;
		}
	}
	return ret;
}

int main(int argc, char **argv)
{
	FILE *fp;
	char *lineptr = NULL;
	size_t nr = 0;
	int ret;

	if(argc < 2) {
		fprintf(stderr, "Usage...\n");
		exit(1);
	}

	fp = fopen(argv[1], "r");
	if(NULL == fp) {
		perror("fopen()");
		exit(1);
	}
	while(1) {
		ret = mygetline(&lineptr, &nr, fp);
		if(ret < 0)
			break;
		printf("nr = %d\n", nr);
		printf("%d\n", ret);
		printf("%d\n", strlen(lineptr));
		printf("%s",lineptr);
	}

	exit(0);
}
