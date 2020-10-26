#include <stdio.h>
#include <stdlib.h> 
//#define N 20
//#define N 200000
#define N 5000000
#define Max 50
int main(void)
{
 	FILE *fp = fopen("input.csv", "w");
	for(int i = 0; i < N; i++)
	{
		fprintf(fp, "%d", rand());
		for(int j = 1;j < 20;j++)
			fprintf(fp, "|%d", rand());
		fprintf(fp, "\n");
	}
	fclose(fp);
}
