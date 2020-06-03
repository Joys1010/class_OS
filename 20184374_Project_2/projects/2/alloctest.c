#include <stdio.h>

extern int my_pallocator;

void test_nextfit();
void test_bestfit();
void test_buddysystem();


void alloctest(void)
{

	printf("implement page allocation test!!!\n");
	
	
	if (my_pallocator == 1) {
	
		
		printf("TEST NEXTFIT\n");
		test_nextfit();
	}
	
	else if (my_pallocator == 2) {
		
		
		printf("TEST BESTFIT\n");
		test_bestfit();
	}
	
	else if (my_pallocator == 3) {
		
			
		printf("TEST BUDDYSYSTEM\n");
		test_buddysystem();
	}

}

void test_nextfit(void) {

	size_t i;
	char * mem[10];

	printf("Initial page status : \n");
	palloc_get_status(0);

	for (i=0; i<3; i++) { //page size 101 * num 5
	
		mem[i] = (char *)malloc(2048 * 200); 
		if(mem[i] == NULL)
			printf("somethings wrong\n");
		memset(mem[i], 0, 2048*200);
		printf("After allocation of 101 page \n");
		printf("page status : \n");\
		palloc_get_status(0);
	}

	free(mem[1]);
	printf("After free of 105 ~ 205 page\n");
	printf("page status : \n");
	palloc_get_status(0);

	
	for (i=4; i<7; i++) { //page size 101 * num 5
	
		mem[i] = (char *)malloc(2048 * 98);
		if(mem[i] == NULL)
			printf("somethings wrong\n");
		memset(mem[i], 0, 2048*98);
		printf("After allocation of 50 page \n");
		printf("page status : \n");\
		palloc_get_status(0);
	}

	free(mem[4]);
	printf("After free of 307 ~ 356 page\n");
	printf("page status : \n");
	palloc_get_status(0);


	free(mem[5]);
	printf("After free of 357 ~ 406 page\n");
	printf("page status : \n");
	palloc_get_status(0);

	for (i=4; i<6; i++) { //page size 101 * num 5
	
		mem[i] = (char *)malloc(2048 * 138);
		if(mem[i] == NULL)
			printf("somethings wrong\n");
		memset(mem[i], 0, 2048*138);
		printf("After allocation of 70 page \n");
		printf("page status : \n");\
		palloc_get_status(0);
	}
	

	
}

void test_bestfit(void) {


	printf("Initial page status : \n");
	palloc_get_status(0);

	size_t i = 0;
	char * mem[10]= {NULL,};

	for (i=0; i<2; i++) {
	
		mem[i] = (char *)malloc(2048 * 200);
		if(mem[i] == NULL)
			printf("somethings wrong\n");
		memset(mem[i], 0, 2048*200);
		printf("After allocation of 101 page \n");
		printf("page status : \n");
		palloc_get_status(0);

	}

	
	for (i=2; i<4; i++) {
	
		mem[i] = (char *)malloc(2048 * 158);
		if(mem[i] == NULL)
			printf("somethings wrong\n");
		memset(mem[i], 0, 2048*158);
		printf("After allocation of 80 page \n");
		printf("page status : \n");
		palloc_get_status(0);

	}


	mem[4] = (char *)malloc(2048 * 98);
	if(mem[4] == NULL)
		printf("somethings wrong\n");
	memset(mem[4], 0, 2048*98);
	printf("After allocation of 50 page \n");
	printf("page status : \n");
	palloc_get_status(0);



	free(mem[1]);
	printf("After free of 105 ~ 205 page\n");
	printf("page status : \n");
	palloc_get_status(0);



	free(mem[3]);
	printf("After free of 286 ~ 365 page\n");
	printf("page status : \n");
	palloc_get_status(0);



	mem[1] = (char *)malloc(2048 * 118);
	if(mem[1] == NULL)
		printf("somethings wrong\n");
	memset(mem[1],0, 2048*118);
	printf("After allocation of 60 page \n");
	printf("page status : \n");
	palloc_get_status(0);


	
	mem[3] = (char *)malloc(2048 * 198);
	if(mem[3] == NULL)
		printf("somethings wrong\n");
	memset(mem[3],0, 2048*198);
	printf("After allocation of 100 page \n");
	printf("page status : \n");
	palloc_get_status(0);



	mem[5] = (char *)malloc(2048 * 28);
	if(mem[5] == NULL)
		printf("somethings wrong\n");
	memset(mem[5],0, 2048*28);
	printf("After allocation of 15 page \n");
	printf("page status : \n");
	palloc_get_status(0);

}

void test_buddysystem(void) {


	printf("Initial page status : \n");
	palloc_get_status(0);

	size_t i;
	char * mem[10];

	for (i=0; i<3; i++) {
	
		mem[i] = (char *)malloc(2048 * 5); // page size = 3 * page num = 3
		if(mem[i] == NULL)
			printf("somethings wrong\n");
		memset(mem[i], 0, 2048*5);
		printf("After allocation of 3 page\n");
		printf("page status : \n");
		palloc_get_status(0);
	}
	


	for (i=3; i<5; i++) {
	
		mem[i] = (char *)malloc(2048 * 10); // page size = 6 * page num = 2
		if(mem[i] == NULL)
			printf("somethings wrong\n");
		memset(mem[i], 0, 2048*10);
		printf("After allocation of 6 page\n");
		printf("page status : \n");
		palloc_get_status(0);


		
	}

		for (i=5; i<9; i++) {
	
		mem[i] = (char *)malloc(2048 * 5); //page size = 3 * page num = 4
		if(mem[i] == NULL)
			printf("somethings wrong\n");
		memset(mem[i], 0, 2048*5);
		printf("After allocation of 3 page\n");
		printf("page status : \n");
		palloc_get_status(0);
	}
	
	

	for (i = 1; i < 3; i++) {

		free(mem[i]); //free page size = 3 * page num = 2
	}

	printf("After free of 9 ~ 16 page\n");
	printf("page status : \n");
	palloc_get_status(0);

	for (i=1; i<3; i++) {
	
		mem[i] = (char *)malloc(2048 * 12); //page size = 7 * page num = 2
		if(mem[i] == NULL)
			printf("somethings wrong\n");
		memset(mem[i], 0, 2048*12);
		printf("After allocation of 7 page\n");
		printf("page status : \n");
		palloc_get_status(0);
	}


	for (i = 3; i < 5; i++) {
		free(mem[i]); //free page size = 6 * page num = 2
	}

	printf("After free of 17 ~ 32 page\n");
	printf("page status : \n");
	palloc_get_status(0);


	for (i=5; i<7; i++) {
	
		mem[i] = (char *)malloc(2048 * 18); //page size = 10 * page num = 2
		if(mem[i] == NULL)
			printf("somethings wrong\n");
		memset(mem[i], 0, 2048*18);
		printf("After allocation of 10 page\n");
		printf("page status : \n");
		palloc_get_status(0);	
	}
}


