#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <vector>
using namespace std;


 


int main(int argc, char** argv)
{
	vector<int> temp1,temp2;
	int index=0,temp;
	ifstream in1(argv[1]);
	while(!in1.eof()){
		in1>>temp;
		temp1.push_back(temp);
	
	}
	in1.close();
	ifstream in2(argv[2]);
	while(!in2.eof()){
		in2>>temp;
		temp2.push_back(temp);
	
	}
	cout<<temp1.size()<<" "<<temp2.size()<<endl;
	for(index = 0 ; index < temp1.size();index++){
		
		
		if(temp1[index] != temp2[index])
		{
			cerr << "error !  index : "<<index<<endl;
			exit(1);
		}
		
	}
	cout<<"success"<<endl;
	
}

 

