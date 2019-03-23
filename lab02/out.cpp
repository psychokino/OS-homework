#include <iostream>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fstream>
#include <vector>
#include <sys/time.h>
#include <iomanip>
#include <cstdlib>
using namespace std;

inline int fork_pcs(pid_t* pcs,int index){
	index--;
	
	if(index > 0 ){
		
		
		pcs[index] = fork();
		if( pcs[index]>0 ){
			return fork_pcs(pcs,index);
		}
		else if( pcs[index] == 0){
			
			return 0;
		}
		else return 0 ;
		
		
	} else if(index == 0 ){
		
		pcs[0] = fork();
		if(pcs[index]==0){
		
		return 0;
		}
		return 0;
	} else return 0;
			
}

unsigned int MatrixComputation( unsigned int *Matrix_A, unsigned int *Matrix_B, unsigned int *Matrix_C, int Matrix_Size, int TheNumOfPcs, int MaxPcs){
	
	unsigned int matrix_check=0,sum=0;
	int UpperBound_DivideMatrixSize_;
	int LowerBound_DivideMatrixSize_;
	if(TheNumOfPcs == (MaxPcs-1) ){
		UpperBound_DivideMatrixSize_ = Matrix_Size ;
		LowerBound_DivideMatrixSize_ = (Matrix_Size/MaxPcs)*(TheNumOfPcs);
	}else {
		UpperBound_DivideMatrixSize_ = (Matrix_Size/MaxPcs)*(TheNumOfPcs+1);
		LowerBound_DivideMatrixSize_ = (Matrix_Size/MaxPcs)*(TheNumOfPcs);
		
	}
	//cout<<"PID : "<<getpid()<<"   MS : "<<Matrix_Size<<"  MP : "<<MaxPcs<<"   TNOP : "<<TheNumOfPcs<<"   upper bound : "<<UpperBound_DivideMatrixSize_<<"   lower bound : "<<LowerBound_DivideMatrixSize_<<endl;
	for(int x = LowerBound_DivideMatrixSize_ ; x < UpperBound_DivideMatrixSize_ ; x++){
		for(int y=0 ; y < Matrix_Size ; y++){
			for(int z=0 ; z < Matrix_Size ; z++){
				//cout<<"A : "<<Matrix_A[ x*Matrix_Size + z ]<<"    B : "<<Matrix_B[ z*Matrix_Size + y ]<<endl;
				sum += Matrix_A[ x*Matrix_Size + z ] * Matrix_B[ z*Matrix_Size + y ];
				//cout<< sum<<" ";
			}
			Matrix_C[ x*Matrix_Size + y ] = sum ;
			//cout<<sum<<" ";
			matrix_check += sum ;
			sum = 0 ;
		}
				
				
	}
	return matrix_check;
}


int main(int argc ,char**  argv){
	//for(int checker=1 ; checker < 300 ; checker++){
	//cout<<"start id = "<<getpid()<<endl;
	int rootpid = getpid();
	ifstream ifs(argv[1]);
	ofstream ofs(argv[2]);
	pid_t child_pcs[16];
	struct timeval start,end;
	
	
	
	unsigned int *Matrix_A;
	unsigned int *Matrix_B;
	unsigned int *Matrix_C;
	unsigned int *checksum;
	//unsigned int sum = 0 ;
	//unsigned int* Matrix;
	int Matrix_Size;
	
	
	//Matrix_Size =checker ;
	cout<<"Input The Matrix Dimension : ";
	cin >> Matrix_Size;
	cout<<endl;
	int shmid_A,shmid_B,shmid_C,shmid_checksum;
	shmid_A = shmget(IPC_PRIVATE, (sizeof(unsigned int)*(Matrix_Size)*(Matrix_Size)), IPC_CREAT | 0644 ) ;
	shmid_B = shmget(IPC_PRIVATE, (sizeof(unsigned int)*(Matrix_Size)*(Matrix_Size)), IPC_CREAT | 0644 ) ;
	shmid_C = shmget(IPC_PRIVATE, (sizeof(unsigned int)*(Matrix_Size)*(Matrix_Size)), IPC_CREAT | 0644 ) ;
	shmid_checksum = shmget(IPC_PRIVATE, (sizeof(unsigned int)), IPC_CREAT | 0644 ) ;
	if(shmid_A == -1){cout<<"shm_A failed"<<endl;return -1 ;}
	if(shmid_B == -1){cout<<"shm_B failed"<<endl;return -1 ;}
	if(shmid_C == -1){cout<<"shm_C failed"<<endl;return -1 ;}
	if(shmid_checksum == -1){cout<<"shm_checksum failed"<<endl;return -1 ;}
	Matrix_A=(unsigned int*)shmat(shmid_A,NULL,0);
	Matrix_B=(unsigned int*)shmat(shmid_B,NULL,0);
	Matrix_C=(unsigned int*)shmat(shmid_C,NULL,0);
	checksum = (unsigned int*)shmat(shmid_checksum,NULL,0);
	
	
	
	
	for(int process_num = 1	; process_num <= 16 ; process_num++){
		for(int i=0;i<16;i++)child_pcs[i] = 0;
		for(int i=0;i<Matrix_Size*Matrix_Size ;i++)Matrix_A[i]=i;
		for(int i=0;i<Matrix_Size*Matrix_Size ;i++)Matrix_B[i]=i;
		for(int i=0;i<Matrix_Size*Matrix_Size ;i++)Matrix_C[i]=0;
		checksum[0] = 0;
		cout<<"Multiplying matrices using  "<<process_num<<" process"<<endl;
		
		fork_pcs(child_pcs,process_num);
		
		for(int j = 0 ; j < process_num ; j++){
			if(child_pcs[j]<0){
				cerr<<"fork child process "<<j<<" failed , program terminated"<<endl;
				return -1 ;
			}
		}
		if(getpid()==rootpid)gettimeofday(&start,0);
		
		if(child_pcs[15]==0 && process_num >15){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,15,process_num);
			exit(0);
		}
		if(child_pcs[14]==0 && process_num >14){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,14,process_num);
			exit(0);
		}
		if(child_pcs[13]==0 && process_num >13){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,13,process_num);
			exit(0);
		}
		if(child_pcs[12]==0 && process_num >12){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,12,process_num);
			exit(0);
		}
		if(child_pcs[11]==0 && process_num >11){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,11,process_num);
			exit(0);
		}
		if(child_pcs[10]==0 && process_num >10){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,10,process_num);
			exit(0);
		}
		if(child_pcs[9]==0 && process_num >9){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,9,process_num);
			exit(0);
		}
		if(child_pcs[8]==0 && process_num >8){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,8,process_num);
			exit(0);
		}
		if(child_pcs[7]==0 && process_num >7){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,7,process_num);
			exit(0);
		}
		if(child_pcs[6]==0 && process_num >6){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,6,process_num);
			exit(0);
		}
		if(child_pcs[5]==0 && process_num >5){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,5,process_num);
			exit(0);
		}
		if(child_pcs[4]==0 && process_num >4){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,4,process_num);
			exit(0);
		}
		if(child_pcs[3]==0 && process_num >3){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,3,process_num);
			exit(0);
		}
		if(child_pcs[2]==0 && process_num >2){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,2,process_num);
			exit(0);
		}
		if(child_pcs[1]==0 && process_num >1){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,1,process_num);
			exit(0);
			
		}
		if(child_pcs[0] == 0){
			
			checksum[0] += MatrixComputation(Matrix_A, Matrix_B, Matrix_C, Matrix_Size,0,process_num);

			exit(0);	
		}

		
		
		while(wait(NULL)>0);
		while(wait(NULL)>0);
		while(wait(NULL)>0);
		gettimeofday(&end,0);
		int sec = end.tv_sec - start.tv_sec ;
		int usec = end.tv_usec - start.tv_usec ;
			
		cout<<"elapsed time : "<<setw(10)<<sec*1+(usec/1000000.0)<<" second  , checksum : "<<(checksum[0])<<"  "<<endl;
		
		
		
		
	}
	
	shmdt(Matrix_A);
	shmdt(Matrix_B);
	shmdt(Matrix_C);
	shmdt(checksum);
	shmctl(shmid_A , IPC_RMID , NULL);
	shmctl(shmid_B , IPC_RMID , NULL);
	shmctl(shmid_C , IPC_RMID , NULL);
	
	
	
	return 0;
	
}

