/*
 ============================================================================
 Name        : processSemaphore.c
 Author      : lingo
 Version     : 
 CreateDate  : 2018下午4:23:42
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include "mysem.h"

#define MY_SHM_ID 9956
#define MY_SEM_KEY1 9957
#define MY_SEM_KEY2 9958
#define BUFF_SIZE 255
#define random(x) (rand()%x)

typedef struct{
	int length;
	int data[BUFF_SIZE];
	int oddNum;
	int evenNum;
}Buff;

void getodd(Buff *buff){
	// buff 长度为1
	for(int i = 0; i < buff->length; i++){
		if(1 == (buff->data[i]%2)){

			printf("getodd num is : %i \n",buff->data[i]);
			// 将索引的值删除，所有索引后的退位
			for(int j = i; j < buff->length -1; j++){
				buff->data[j] = buff->data[j+1];
			}
			buff->length--;
			break;
		}
	}
}
void countodd(Buff *buff){
	buff->oddNum++;
	printf("Now the oddNum=%i \n",buff->oddNum);
}

void geteven(Buff *buff){
	// buff 长度为1
	for(int i = 0; i < buff->length; i++){
		if(0 == (buff->data[i]%2)){
			printf("geteven num is : %i \n",buff->data[i]);
			// 将索引的值删除，所有索引后的退位
			for(int j = i; j < buff->length -1; j++){
				buff->data[j] = buff->data[j+1];
			}
			buff->length--;
			break;
		}
	}
}
void counteven(Buff *buff){
	buff->evenNum++;
	printf("Now the evenNum=%i \n",buff->evenNum);
}
void produce(Buff *buff){
	int temp = random(10);
	buff->data[buff->length] = temp;
	buff->length++;
	printf("produce a num : %i \n",temp);
	sleep(1);
}
int main(int arg,char* argv[]){
	Buff *buff;
	pid_t pid;
	int shmid,empty,full;
	//共享内存
	shmid = shmget(MY_SHM_ID,sizeof(Buff),0666|IPC_CREAT);
	// 创建一个信号量 不同key对应不同信号量
	empty = semget(MY_SEM_KEY1,1,0666|IPC_CREAT);
	full = semget(MY_SEM_KEY2,1,0666|IPC_CREAT);
	if(shmid == -1 || empty == -1 || full == -1){
		printf("error semget / shmget");
	}
	// 初始化empty信号量val初值 当P(empty) empty-- 到0时不再生产  进程处于等待状态 等待另一个进程执行V(empty)
	init_sem(empty,10);
	// 初值为0 进程P(full)处于wait状态 等待另一个进程执行V(full) 操作
	init_sem(full,0);

	// 映射共享内存
	buff = (Buff*)shmat(shmid,NULL,0);
	// 初始化变量
	buff->length = 0;
	buff->oddNum = 0;
	buff->evenNum = 0;


	// 创建三个子进程 p1 produce p2 getodd  p3 geteven
	int i;
	for(i = 0; i < 3; i++){
		pid = fork();
		if(pid == 0 || pid < 0) break;
	}
	if(pid < 0){
		printf("fork error");
	}else if(0 == pid){
		if(i==0){
			printf("Child process P1 is run,processid is %d,parentid is %i  empty = %i full = %i\n",getpid(),getppid(),empty,full);
			// 生产个数 小于等于empty.val 否者生产进程 生产数量满时处于饱腹状态
			for(int j = 0; j < 8; j++){
				P(empty);
				produce(buff);
				V(full);
			}
		}
		if(i==1){
			printf("Child process P2 is run,processid is %d,parentid is %i  empty = %i full = %i\n",getpid(),getppid(),empty,full);
			for(int j = 0; j < 2; j++){
				P(full);
				geteven(buff);
				counteven(buff);
				printf("Now buff'length = %i\n",buff->length);
				V(empty);
			}
		}
		if(i==2){
			printf("Child process P3 is run,processid is %d,parentid is %i  empty = %i full = %i\n",getpid(),getppid(),empty,full);
			for(int j = 0; j < 2; j++){
				P(full);
				getodd(buff);
				countodd(buff);
				printf("Now buff'length = %i\n",buff->length);
				V(empty);
			}
		}
	}else{
		printf("Parent process is run,processid is %d  empty = %i full = %i shmid = %i\n",getpid(),empty,full,shmid);
		sleep(10);
		del_sem(empty);
		del_sem(full);
	}
	return 0;
}
