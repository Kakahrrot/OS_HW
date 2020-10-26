all:
	gcc -o generate generate.c
	gcc -o ETL_thread ETL_thread.c -lpthread
