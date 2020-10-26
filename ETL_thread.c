#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <time.h>
#include <unistd.h>

typedef struct node
{
	int *ptr;
	struct node* next;
}Node;

int size = 0;
Node* first = NULL;
Node* last = NULL;

void push(Node* nd)
{
	if(!first)
	{
		first = nd;
		last = nd;
	}
	else
	{
		last->next = nd;
		last = nd;
	}
	size++;
}

Node* pop()
{
	if(!first)
		return NULL;
	else
	{
		Node* tmp = first;
		first = first->next;
		size--;
		tmp->next = NULL;
		return tmp;
		//free(tmp->ptr);
		//free(tmp);
		//return;
	}
}

int indx[20];
int finished = 0;
int waiting = 0;
FILE *fpin, *fpout;
pthread_cond_t cond;
pthread_mutex_t mut;

void* readfile(void* arg)
{
	long ID = (long) arg;
	//printf("reading ID: %ld\n", ID);
	static int counter = 0;
	fpin = fopen("input.csv", "r");
	int* tmp = malloc(sizeof(int) * 20);
	while(fscanf(fpin, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", tmp, tmp+1,tmp+2, tmp+3, tmp+4, tmp+5, tmp+6, tmp+7, tmp+8, tmp+9, tmp+10, tmp+11, tmp+12, tmp+13, tmp+14, tmp+15, tmp+16, tmp+17, tmp+18, tmp+19) != EOF)
		//while(1)
	{
		/*
		   int* tmp = malloc(sizeof(int) * 20);
		   if(fscanf(fpin, "%d", tmp) == EOF)
		   {
		   free(tmp);
		   finished = 1;
		   printf("break\n");
		   break;
		   }
		   for(int i = 1; i < 20; i++)
		   fscanf(fpin, "|%d", tmp + i);
		 */
		Node* t = malloc(sizeof(Node));
		t->ptr = tmp;
		t->next = NULL;
		//if(!first && waiting)
		pthread_mutex_lock(&mut);
		push(t);
		if(waiting)
		{
			pthread_cond_signal(&cond);
			//printf("	signaled size = %d\n", size);
			waiting = 0;
		}
		//printf("	reading file counter: %d tmp: %d, size: %d\n", counter++, tmp[0], size);
		counter++;
		pthread_mutex_unlock(&mut);
		tmp = malloc(sizeof(int) * 20);
	}
	free(tmp);
	finished = 1;
	fclose(fpin);
	pthread_cond_signal(&cond);
	pthread_exit(NULL);
}

void* writefile(void* arg)
{
	long ID = (long) arg;
	//printf("writing ID: %ld\n", ID);
	fpout = fopen("output.json", "w");
	fprintf(fpout, "[\n");
	//	for(;first;pop())
	static int counter = 0;
	while(1)
	{
		//printf("writing file%d\n", counter++);
		pthread_mutex_lock(&mut);
		/*
		   if(size == 0)
		   {
		   printf("size 0!!\n");
		   for(Node* t = first;t;t = t->next)
		   {
		   for(int i = 0; i < 20; i++)
		   printf("%d ", t->ptr[i]);
		   printf("\n");
		   }
		   }
		 */
		if(!first && finished)
		{
			pthread_mutex_unlock(&mut);
			break;
		}
		while(!first)
		{
			waiting = 1;
			//printf("waiting size = %d\n", size);
			pthread_cond_wait(&cond, &mut);
			//printf("signal received size = %d\n\n", size);
		}
		Node* tmp = pop();

		//size--;
		pthread_mutex_unlock(&mut);
		counter++;
		fprintf(fpout, "\t{\n");
		for(int j = 0; j < 19; j++)
			fprintf(fpout, "\t\t\"col_%d\":%d,\n", indx[j], tmp->ptr[j]);
		fprintf(fpout, "\t\t\"col_%d\":%d\n", indx[19], tmp->ptr[19]);
		//release resource
		free(tmp->ptr);
		free(tmp);

		//may have prob.
		if(size != 0)
			fprintf(fpout, "\t},\n");
		else
			fprintf(fpout, "\t}\n");
	}
	fprintf(fpout, "]\n");
	//printf("writing counter:%d\n", counter);
	pthread_exit(NULL);
}

int stop = 0;

void* running(void* arg)
{
	//printf("%lld\n", (long long int)arg);
	int counter = 0;
	while(!stop)
	{
		counter++;
	}
	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		printf("please specify # of threads\n");
		exit(-1);
	}
	int thread_num = atoi(argv[1]);
	if(thread_num <= 1)
	{
		printf("# of thread not allowed");
		exit(-1);
	}
	thread_num -= 2;
	for(int i = 0; i < 20; i++)
		indx[i] = i + 1;
	pthread_t threads[2];
	pthread_attr_t attribute;
	pthread_attr_init(&attribute);
	pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE);
	pthread_mutex_init(&mut, NULL);
	pthread_cond_init(&cond, NULL);
	time_t start = clock();

	pthread_create(threads, &attribute, readfile, (void*)1);
	pthread_create(threads + 1, &attribute, writefile, (void*)2);

	void* status;
	pthread_t* runthread = malloc(sizeof(pthread_t) * (thread_num));
	printf("start\n");
	for(long long  i = 0; i < thread_num; i++)
		pthread_create(runthread + i, &attribute, running, (void*)i);
	pthread_join(threads[0], &status);
	pthread_join(threads[1], &status);
	stop = 1;
	for(int i = 0; i < thread_num; i++)
		pthread_join(runthread[i], &status);


	double duration = (double)(clock() - start) / CLOCKS_PER_SEC;
	printf("%f sec\n", duration);
	//printf("Main: program completed. Exiting.\n");
	pthread_attr_destroy(&attribute);
	pthread_mutex_destroy(&mut);
	pthread_cond_destroy(&cond);
	pthread_exit(NULL);
}


