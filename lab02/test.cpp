#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
//#include <error.h>
#define SIZE 1024
#include <cstdlib>

#include <sys/types.h>
#include <string.h>
#include <iostream>
using namespace std;
/*
int main()

{

    int shmid ;
    char *shmaddr ;
    struct shmid_ds buf ;
    int flag = 0 ;
    int pid ;

	shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT|0600 ) ;

    if ( shmid < 0 )
    {
            perror("get shm  ipc_id error") ;
            return -1 ;
    }

    pid = fork() ;

    if ( pid == 0 )
    {
        shmaddr = (char *)shmat( shmid, NULL, 0 ) ;
        strcpy( shmaddr, "Hi, I am child process!\n") ;
        shmdt( shmaddr ) ;
        return  0;

    } else if ( pid > 0) {

        sleep(3) ;
        flag = shmctl( shmid, IPC_STAT, &buf) ;
        if ( flag == -1 )
        {
            perror("shmctl shm error") ;
            return -1 ;
        }

        printf("shm_segsz =%d bytes\n", buf.shm_segsz ) ;
        printf("parent pid=%d, shm_cpid = %d \n", getpid(), buf.shm_cpid ) ;
        printf("chlid pid=%d, shm_lpid = %d \n",pid , buf.shm_lpid ) ;

        shmaddr = (char *) shmat(shmid, NULL, 0 ) ;


        printf("%s", shmaddr) ;
        shmdt( shmaddr ) ;
        shmctl(shmid, IPC_RMID, NULL) ;

    }else{

        perror("fork error") ;
        shmctl(shmid, IPC_RMID, NULL) ;
    }

 

    return 0 ;

}
*/
/*
typedef struct{	char name[8] ;	int age ;	} people;

int main(int argc, char** argv)
{
    int shm_id,i;
    key_t key;
    char temp[8];
    people *p_map;
    char pathname[30] ;

    strcpy(pathname,".") ;
    key = ftok(pathname,0x03);
    if(key==-1){
		
        perror("ftok error");
        return -1;
    }

    printf("key=%d\n",key) ;
    shm_id=shmget(key,4096,IPC_CREAT|IPC_EXCL|0600); 
    if(shm_id==-1)
    {
        perror("shmget error");
        return -1;
    }

    printf("shm_id=%d\n", shm_id) ;
    p_map=(people*)shmat(shm_id,NULL,0);
    memset(temp, 0x00, sizeof(temp)) ;
    strcpy(temp,"test") ;
    temp[4]='0';

    for(i = 0;i<10;i++)
    {
        temp[4]+=1;
        strncpy((p_map+i)->name,temp,5);
        (p_map+i)->age=0+i;
		cout<<i<<"succeed"<<endl;
    }
    shmdt(p_map) ;

    return 0 ;

}







int main(int argc, char** argv)
{

    int shm_id,i;
    key_t key;
    people *p_map;
    char pathname[30] ;

    strcpy(pathname,".") ;
    key = ftok(pathname,0x03);
    if(key == -1)
    {
        perror("ftok error");
        return -1;
    }

    printf("key=%d\n", key) ;
    shm_id = shmget(key,0, 0);   
    if(shm_id == -1)
    {
        perror("shmget error");
        return -1;
    }

    printf("shm_id=%d\n", shm_id) ;
    p_map = (people*)shmat(shm_id,NULL,0);
    for(i = 0;i<10;i++)
    {
        printf( "name:%s\n",(*(p_map+i)).name );
        printf( "age %d\n",(*(p_map+i)).age );
    }

    if(shmdt(p_map) == -1)
    {
        perror("detach error");
        return -1;
    }

    return 0 ;
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

unsigned long long int pro1(int n,int shmida,int shmidc);
unsigned long long int pro2(int n,int shmida,int shmidc);
unsigned long long int pro3(int n,int shmida,int shmidc);
unsigned long long int pro4(int n,int shmida,int shmidc);
unsigned long long int pro5(int n,int shmida,int shmidc);
unsigned long long int pro6(int n,int shmida,int shmidc);
unsigned long long int pro7(int n,int shmida,int shmidc);
unsigned long long int pro8(int n,int shmida,int shmidc);
unsigned long long int pro9(int n,int shmida,int shmidc);
unsigned long long int pro10(int n,int shmida,int shmidc);
unsigned long long int pro11(int n,int shmida,int shmidc);
unsigned long long int pro12(int n,int shmida,int shmidc);
unsigned long long int pro13(int n,int shmida,int shmidc);
unsigned long long int pro14(int n,int shmida,int shmidc);
unsigned long long int pro15(int n,int shmida,int shmidc);
unsigned long long int pro16(int n,int shmida,int shmidc);

int main()
{

int i,dimen;
unsigned long long int sum;
struct timeval start,end;
int shmida,shmidc;
shmida=shmget(IPC_PRIVATE,sizeof(int)*900*900,IPC_CREAT|0644);
shmidc=shmget(IPC_PRIVATE,sizeof(int)*900*900,IPC_CREAT|0644);

if((shmida<0)||(shmidc<0)){
printf("get shm error!!!\n");
}

/* a=shmat(shmid1,NULL,0);
c=shmat(shmid2,NULL,0);*/

printf("Input the matrix dimension: ");
scanf("%d",&dimen);

gettimeofday(&start,0);
sum=pro1(dimen,shmida,shmidc);
gettimeofday(&end,0);
int sec=end.tv_sec-start.tv_sec;
int usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 1 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro2(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 2 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro3(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 3 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro4(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 4 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro5(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 5 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro6(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 6 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro7(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 7 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro8(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 8 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro9(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 9 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro10(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 10 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro11(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 11 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro12(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 12 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro13(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 13 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro14(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 14 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro15(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 15 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

gettimeofday(&start,0);
sum=pro16(dimen,shmida,shmidc);
gettimeofday(&end,0);
sec=end.tv_sec-start.tv_sec;
usec=end.tv_usec-start.tv_usec;

printf("Multiplying matrices using 16 process\n");
printf("elapsed %f ms, Checksum: %d\n",sec*1000+(usec/1000.0),sum);

return 0;
}

unsigned long long int pro1(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0;

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

	for(i=0;i<n*n;++i){
		a[i]=i;
	}

	for(i=0;i<n;++i)
		for(j=0;j<n;++j){
			for(k=0;k<n;++k){
				tc+=a[i*n+k]*a[k*n+j];
			}
			c[i*n+j]=tc;
			tc=0;
		}

	for(i=0;i<n*n;++i) sum+=c[i];
	shmdt(a);
	shmdt(c);
	return sum;
}

unsigned long long int pro2(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[2];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<2;++p){
each1+=n/2;
if(p==1){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/2;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro3(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[3];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p♥;++p){
each1+=n/3;
if(p==2){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/3;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro4(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[4];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<4;++p){
each1+=n/4;
if(p==3){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/4;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro5(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[5];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<5;++p){
each1+=n/5;
if(p==4){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/5;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro6(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[6];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<6;++p){
each1+=n/6;
if(p==5){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/6;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro7(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[7];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<7;++p){
each1+=n/7;
if(p==6){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/7;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro8(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[8];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<8;++p){
each1+=n/8;
if(p==7){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/8;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro9(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[9];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<9;++p){
each1+=n/9;
if(p==8){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/9;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro10(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[10];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<10;++p){
each1+=n/10;
if(p==9){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/10;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro11(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[11];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<11;++p){
each1+=n/11;
if(p==10){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/11;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro12(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[12];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<12;++p){
each1+=n/12;
if(p==11){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/12;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro13(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[13];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<13;++p){
each1+=n/13;
if(p==12){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/13;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro14(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[14];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<14;++p){
each1+=n/14;
if(p==13){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/14;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro15(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[15];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<15;++p){
each1+=n/15;
if(p==14){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/15;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}

unsigned long long int pro16(int n,int shmida,int shmidc){

int *a,*c;
unsigned long long int sum=0;
long long int i,j,k,tc=0,p,each=0,each1=0;
pid_t pid[16];

a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);

for(i=0;i<n*n;++i){
a[i]=i;
}

for(p=0;p<16;++p){
each1+=n/16;
if(p==15){
each1=n;
}
if((pid[p]=fork())<0){
printf("fork error\n");
}else if(pid[p]==0){
a=shmat(shmida,NULL,0);
c=shmat(shmidc,NULL,0);
for(i=each;i<each1;++i)
for(j=0;j<n;++j){
for(k=0;k<n;++k){
tc+=a[i*n+k]*a[k*n+j];
}
c[i*n+j]=tc;
//printf("%d %d\n",i*n+j,tc);
tc=0;
}
shmdt(a);
shmdt(c);
exit(0);
}
each+=n/16;
}

while(wait(NULL)>0);
for(i=0;i<n*n;++i) sum+=c[i];
shmdt(a);
shmdt(c);
return sum;
}
