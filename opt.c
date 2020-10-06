#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <time.h>

void sort(int* array, int start, int end)
{
	if(end - start <= 100)//insertion sort
	{
		int* arr = array + start;
		int size = end - start + 1;
		for (int i = 1; i < size; i++) 
		{
        	int key = arr[i];
        	int j = i - 1;
        	while (key < arr[j] && j >= 0) 
			{
            	arr[j+1] = arr[j];
           	 	j--;
        	}
       	 	arr[j+1] = key;
    	}
		return;
	}
	int middle = (start + end)/2;
	sort(array, start, middle);
	sort(array, middle + 1, end);
	int i, j;
	int *left = malloc(sizeof(int) *( middle - start + 1));
	int *right = malloc(sizeof(int) * (end - middle));
	int n = 0;
	for(i = start; i <= middle; i++)
		left[n++] = array[i];
	for(n = 0, i = middle + 1; i <= end; i++)
		right[n++] = array[i];
	/*
	   printf("before: %d to %d\n", start, end);
	   for(i = start; i <= end; i++)
	   printf("%d ", array[i]);
	   printf("\n");
	*/
	for(n = start, i = 0, j = 0;
			i < middle - start + 1 && j < end - middle;)
	{
		if(left[i] < right[j])
			array[n++] = left[i++];
		else
			array[n++] = right[j++];
	}
	while(i < middle - start + 1)
		array[n++] = left[i++];
	while(j < end - middle)
		array[n++] = right[j++];
	free(left);
	free(right);
	/*
	   printf("%d to %d\n", start, end);
	   for(i = start; i <= end; i++)
	   printf("%d ", array[i]);
	   printf("\n\n\n");
	 */
}

int main(void)
{
	clock_t start, finish;
	double duration;
	int internal_size = 1024 * 1024 * 1024 / 4;//1GB
	//int internal_size = 1024 * 1024 / 4;//MB
	int k = 0;
	FILE* fp = fopen("input.txt", "r");
	int counter = 0;
	int * array = malloc(sizeof(int) * internal_size);
	start = clock();
	while(fscanf(fp, "%d", array + counter++) != EOF)
	{
		if(counter == internal_size)
		{
			k++;// 1 to k, k-way merge
			sort(array, 0, counter - 1);
			//saving file
			int length = snprintf(NULL, 0, "%d", k);
			char* str = malloc(length + 1);
			snprintf(str, length + 1, "%d", k);
			FILE *tmp = fopen(str, "w");
			for(int i = 0; i < internal_size;i++)
				fprintf(tmp, "%d\n", array[i]);
			fclose(tmp);
			free(str);
			counter = 0;
		}
	}
	sort(array, 0, counter - 2);
	k++;// 1 to k, k-way merge
	//saving file
	int length = snprintf(NULL, 0, "%d", k);
	char* str = malloc(length + 1);
	snprintf(str, length + 1, "%d", k);
	FILE *tmp = fopen(str, "w");
	for(int i = 0; i < counter - 1;i++)
		fprintf(tmp, "%d\n", array[i]);
	fclose(tmp);
	free(str);
	printf("K: %d\n", k);
	FILE **files = malloc(sizeof(FILE*) * k);
	for(int i = 0; i < k; i++)
	{
		int length = snprintf(NULL, 0, "%d", i + 1);
		char* str = malloc(length + 1);
		snprintf(str, length + 1, "%d", i + 1);
		files[i] = fopen(str, "r");
		free(str);
	}

	//int buffer_size = internal_size / k / 1024;//MB
	int buffer_size = internal_size / k / 256;
	int buffer[k][buffer_size];
	int end[k];
	for(int i = 0; i < k; i++)
		end[i] = buffer_size;
	for(int i = 0; i < k; i++)
		for(int j = 0; j < buffer_size;j++)
			if(fscanf(files[i], "%d", buffer[i] + j) == EOF)
			{
				end[i] = j;
				break; 	
			}
	int position[k];
	for(int i = 0; i < k; i++)
		position[i] = 0;
	int min = buffer[0][0];
	int min_position = 0;
	int out_position = 0;
	long long int total = (k - 1) * internal_size + counter - 1;
	long long int out_counter = 0;
	FILE *output = fopen("output.txt", "w");

	while(1)
	{
		int finished = 1;
		for(int i = 0; i < k; i++)
		 	if(end[i] != 0)
			{
			 	finished = 0;
				break;
			}
		if(finished)
		{
			finish = clock();
			duration = (double)(finish- start) / CLOCKS_PER_SEC;
			fclose(output);
			break;
		}
		for(int i = 0; i < k; i++)
			if(position[i] < end[i] && buffer[i][position[i]] < min)
			{
				min = buffer[i][position[i]];
				min_position = i;
			}
		position[min_position]++;

		//write data
		array[out_position++] = min;
		out_counter++;
		if(out_counter == total)
			for(int i = 0; i < out_position; i++)
				fprintf(output, "%d\n", array[i]);
		else if(out_position == internal_size)
		{
			for(int i = 0; i < internal_size; i++)
				fprintf(output, "%d\n", array[i]);
		 	out_position = 0;
		}
				
		//fprintf(output, "%d\n", min);
		if(end[min_position] < buffer_size && position[min_position] == end[min_position])
		 	end[min_position] = 0;
		else if(position[min_position] == end[min_position])
		{
			int temp = 0;
			position[min_position] = 0;
			while(fscanf(files[min_position], "%d", buffer[min_position] + temp) != EOF)
			{
				temp++;
				if(temp == buffer_size)
					break;
			}
			if(temp != buffer_size)
			 	end[min_position] = temp;
		}
		for(int i = 0; i < k; i++)
			if(end[i] != 0)
			{
				min = buffer[i][position[i]];
				min_position = i;
				break;
			}
	}
	printf("%f s\n", duration);
	
	/*
	output = fopen("output.txt", "r");
	int t;
	int last_t = t;
	int tc = 0;
	while(fscanf(output, "%d", &t) != EOF)
	{
		tc++;
		//printf("%d\n",t);
		if(tc != 1 && last_t > t)
		{
			printf("False\n");
			break;
		}
	
		last_t = t;
	}
	printf("output size: %d\n", tc);
	printf("caculated size: %lld\n", total);
	*/
	free(array);
	for(int i = 1; i <= k;i++)
	{
		int length = snprintf(NULL, 0, "%d", i);
		char* str = malloc(length + 1);
		snprintf(str, length + 1, "%d", i);
		remove(str);
		free(str);
	}
}
