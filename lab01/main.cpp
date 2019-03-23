#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>

using namespace std;

int main()
{
    pid_t pid;
	
	string exe,program_name,arg1,arg2;
    
	//int myPid = getpid();
    //int parentPid = getppid();
	
    
    while(true){
	
		cout<<"> ";
		getline(cin,exe);
		//exe = "wwwww";
		pid = fork();
		if(exe == "exit")exit(0);
		//else if(exe.empty())cout<<"> ";
		if(pid < 0){
			cout<<"Fork Failed !"<<endl;
        
			exit(-1);
		}
		else if(pid == 0){
			
			//printf("Child:SelfID=%d ParentID=%d \n", myPid, parentPid);
			//cout<<"PID = 0 completed"<<endl;
			//cout<<exe<<endl;
			program_name.assign( exe , 0 , exe.find(' ',0) );
			const char *execute_p = program_name.c_str();
			exe.erase(0,program_name.size());
			
			//cout<<" program name : "<<program_name<<endl;
			//cout<<exe<<endl;
			//cout<<" is string empty ? :"<<exe.empty()<<endl;
			//cout<<"test arg1 : "<<arg1.empty()<<"   test arg2 : "<<arg2.empty()<<endl;
			
			while(1){
			
				if(exe[0] == ' ')exe.erase(0,1);
				else{
					break;
				}
			}
			
			arg1.assign( exe , 0 , exe.find(' ',0) );
			const char *execute_arg1 = arg1.c_str();
			exe.erase(0,arg1.size());
			//cout<<"test arg1 : "<<arg1<<"    ||||    arg2 emp : "<<arg2.empty()<<endl;
			
			while(1){
			
				if(exe[0] == ' ')exe.erase(0,1);
				else{
					break;
				}
			}
			
			arg2.assign( exe , 0 , exe.find(' ',0) );
			const char *execute_arg2 = arg2.c_str();
			exe.erase(0,arg2.size());
			//cout<<"test arg2 : "<<arg2<<"    ||||    arg2 empty ??: "<<arg2.empty()<<endl;
			
			if(!arg2.empty()){
			
				if(arg2 == "&"){
					pid_t pid_grandson = fork();
					
					if(pid_grandson == 0){
						//cout<<"callme"<<endl;
						execlp(execute_p,execute_p,execute_arg1,NULL);
						cout<<"failed to call execlp from child process "<<endl;
						exit(0);
					
					}
					else{
						exit(0);
					
					}
					//cout<<"failed to call execlp from child process "<<endl;
					
				}
				else{
					execlp(execute_p,execute_p,execute_arg1,execute_arg2,NULL);
					cout<<"failed to call execlp from child process "<<endl;
					exit(0);
				
				}
			}	
			else if(!arg1.empty()){
				execlp(execute_p,execute_p,execute_arg1,NULL);
				cout<<"failed to call execlp from child process "<<endl;
				exit(0);
			}
			else{
				
				execlp(execute_p,execute_p,NULL);
				cout<<"failed to call execlp from child process "<<endl;
				exit(0);
			}
			
			//execlp("which","which",execute_p,NULL);
			//printf("Child:flag=%d %p \n", flag, &flag);
			cout<<"failed to call execlp from child process "<<endl;
			exit(0);
		
			/*int myPid = getpid();
			int parentPid = getppid();
			printf("Child:SelfID=%d ParentID=%d \n", myPid, parentPid);
			flag = 123;
			printf("Child:flag=%d %p \n", flag, &flag);*/
		}
		else{
			wait(NULL);
			//cout<<"==========Child Complete=========="<<endl;
			//exit(0);
		}
		//cout << "Hello world!" << endl;
	}
    
}
