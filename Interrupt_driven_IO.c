#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<semaphore.h>
#include<sys/wait.h>
int pending_req=0;
sem_t iarr_input;
sem_t iarr_output;
pthread_mutex_t kbrd_lock[3];
pthread_mutex_t iarr_lock;
pthread_mutex_t mainLock;
struct interrupt{
	int device;
	int addr;
	int offset;
	int priority;
	int mask;
};
struct interrupt iarr[5];
int iarr_in=0;
int iarr_out=0;
/*struct queue{
	int buff[10];
	int qf;
	int qb;

};
void init(struct queue* q1){
	q1->qf=-1;
	q1->qb=-1;
}
void pop(struct queue* q1){
	if(q1->qb!=-1){
		q1->qb--;
	}
}
void push(struct queue* q1,int element){
	if(q1->qf==-1)q1->qf++;
	
	if(q1->qf!=9){
		q1->buff[q1->qf++]=element;
	}
}
int back(struct queue* q1){
	return q1->buff[q1->qb];
}
void reset(int idx){
	iarr[idx].addr=-1;
	iarr[idx].offset=-1;
	iarr[idx].priority=-1;
	iarr[idx].mask=-1;
}*/
int IO_buff[10];
int buff_counter=0;
void generate_interrupt(int device, int addr,int offset,int priority,int mask){
	sem_wait(&iarr_input);
	pthread_mutex_lock(&iarr_lock);
	iarr[iarr_in].device=device;
	iarr[iarr_in].addr=addr;
	iarr[iarr_in].offset=offset;
	iarr[iarr_in].priority=priority;
	iarr[iarr_in].mask=mask;
	printf("interrupt generated : %d\n",iarr_in);
	iarr_in++;
	iarr_in%=5;
	pthread_mutex_unlock(&iarr_lock);
	sem_post(&iarr_output);
}
void *kbrd(){
	int i=0;
	for(i=0;i<3;i++){
		IO_buff[buff_counter+i]=3;
	}
	generate_interrupt(0,buff_counter,i,0,1);
	buff_counter+=i;
	
	
}
void *hdd(){
	int i=1;
	
	generate_interrupt(1,buff_counter,i,3,0);
	buff_counter+=i;
}

void *smthing(){
	int i=1;
	generate_interrupt(1,buff_counter,i,1,1);
	buff_counter+=i;
}
void ioready(int idx){
	printf("interrupt handler for Keyboard Buffer. ID: %d\n",idx);
}
void hdd_crash(int idx){
	printf("interrupt handler for HDD crash. ID: %d\n",idx);
}
void smthing_happened(int idx){
	printf("interrupt handler for Something. ID: %d\n",idx);
}

void (*func_ptr_array[])()={(void*)(kbrd),(void*)(hdd),(void*)(smthing)};
void (*interrupt_handlers[])()={ioready,hdd_crash,smthing_happened};




void *divide0(){
	
}
void *IOreq(void *devAddr){
	
	int *d=(int*)devAddr;
	pthread_mutex_lock(&kbrd_lock[*d]);
	printf("param size now :%d\n",*((int*)devAddr));
	(*func_ptr_array[*d])();
	pthread_mutex_unlock(&kbrd_lock[*d]);
	/*sem_wait(&iarr_output);
	pthread_mutex_lock(&iarr_lock);
	(*interrupt_handlers[iarr[iarr_out].device])(iarr[iarr_out].device);
	//reset(iarr_out);
	iarr_out++;
	iarr_out%=5;
	pthread_mutex_unlock(&iarr_lock);
	sem_post(&iarr_input);*/			
}
void *dummyProcess(){
	int counter=0;
	int idx=0;
	pthread_t thrd1[3];
	pthread_mutex_lock(&kbrd_lock[0]);
	int param=1;
	int *ptr;
	ptr=&param;
	while(1){
		printf("dummy process %d\n",counter);
		counter++;
		if(pending_req<3){
			idx=pthread_create(&thrd1[pending_req],NULL,IOreq, ptr);
			printf("param size %d\n", *ptr);
			pending_req++;
			
		}
		if(sem_trywait(&iarr_output)==0){
			//printf("bhetore dhukse\n");
			pthread_mutex_lock(&iarr_lock);
			(*interrupt_handlers[iarr[iarr_out].device])(iarr[iarr_out].device);
			//reset(iarr_out);
			iarr_out++;
			iarr_out%=5;
			pthread_mutex_unlock(&iarr_lock);
			sem_post(&iarr_input);
		}
		if(counter==2)pthread_mutex_unlock(&kbrd_lock[0]);
		if(counter>=20)break;
		//sleep(10);
		
	};
	//pthread_mutex_unlock(&kbrd_lock[0]);
	for(int i=0;i<3;i++)pthread_join(thrd1[i],NULL);
}
int main(){
	for(int i=0;i<3;i++)
		pthread_mutex_init(&kbrd_lock[i],NULL);
	pthread_t thArr[5];
	pthread_mutex_init(&iarr_lock,NULL);
	pthread_mutex_init(&mainLock,NULL);
	sem_init(&iarr_input,0,5);
	sem_init(&iarr_output,0,0);
	int idx[5];
	int param=0;
	idx[0]=pthread_create(&thArr[0],NULL,dummyProcess,NULL);
	//idx[1]=pthread_create(&thArr[0],NULL,IOreq,&param);
	pthread_join(thArr[0],NULL);
	
}


















