#include <stdlib.h>
#include <stdio.h> 
int main(void)
{
 	//printf("%d\n", RAND_MAX);
	FILE* fp = fopen("input.txt", "w");
	long long int size = (long long int) 1024 * 1024 * 1024 / 4 * 1.5 ;
	for(int i = 0; i < size; i++)
	//for(int i = 0; i < 1024; i++)
	{
		int a = rand();
		if(rand()%2)
			a = -a;
	 	fprintf(fp, "%d\n", a);
	}
	printf("size: %lld\n", size);
	return 0;
}
