#include <stdio.h>
#include "./threads/thread.h"

void test(){
        int i=0;
        while(i<500) {
		thread_sleep(1);
		if (i%25 == 0) {
        		printf("%d : %s\n",i,thread_name());
		}
		i++;
	}
        return;
}
void synctest(void)
{
        printf("implement synchronize test!!!\n");
	

	tid_t thread_ids[4];
	const char *thread_names[4] = {"Thread1","Thread2","Thread3","Thread4"};
	int i;
//joys

	for (i=0;i<4;i++) {
		
		thread_ids[i] = thread_create(thread_names[i],PRI_DEFAULT,test,NULL);	
	}

       
	for (i=0;i<4;i++) {
		thread_join(thread_ids[i]);
        	printf("%s is done !\n",thread_names[i]);
	}

        printf("FINISH!!!\n");
        return ;
}

