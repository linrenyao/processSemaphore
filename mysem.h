/*
 * mysem.h
 *
 *  Created on: 2018年10月19日
 *      Author: lin
 */

#ifndef MYSEM_H_
#define MYSEM_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

// 定义信号量结构
union semaphore{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

/*声明函数*/

/*初始化信号量
 * sem_id 信号量的标识id
 * init_val 信号量的pv操作的数值
 * */
int init_sem(int sem_id,int init_val){
	union semaphore sem;
	sem.val = init_val; //设置初值
	if(semctl(sem_id,0,SETVAL,sem)==-1){
		printf("init semaphore error \n");
		return -1;
	}
	return 0;
}
/*删除指定标识id的信号量*/
int del_sem(int sem_id){
	union semaphore sem;
	if(semctl(sem_id,0,IPC_RMID,sem)==-1){
		return -1;
	}
	return 0;
}
/* 将指定标识的信号量-1 若信号量值为0时执行p操作的进程会处于阻塞等待的状态*/
int P(int sem_id){ //信号量-1
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = -1;
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id,&sem_b,1)==-1){
		return -1;
	}
	return 0;
}
/* 将指定标识的信号量+1 若型号量的值为0执行完v操作后 唤醒处于阻塞等待的进程处于就绪状态等待cpu调用*/
int V(int sem_id){ //信号量+1
	struct sembuf sem_b;
	sem_b.sem_num = 0;
	sem_b.sem_op = 1;
	sem_b.sem_flg = SEM_UNDO;
	if(semop(sem_id,&sem_b,1)==-1){
		return -1;
	}
	return 0;
}

#endif /* MYSEM_H_ */
