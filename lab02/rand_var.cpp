#include <iostream>
#include <time.h>
#include <cstdlib>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
	
	int i,j;
	int num_of_wanted_var=0;
	int maxi=0,mini=4294967295,now,bound,bound_down;
	int any_negative=0;
    cout<<"enter the variable size : ";
	cin >> num_of_wanted_var ;
	cout<<endl;
	cout<<"enter the max int bound : ";
	cin >> bound ;
	cout<<endl;
	cout<<"enter the min int bound : ";
	cin >> bound_down ;
	cout<<endl;
	cout<<"do u need negative value ?  (yes = 1 ; no = 0) : ";
	cin >> any_negative ;
	cout << endl ;
	
	ofstream ofs(argv[1]);
	//ofs<<num_of_wanted_var<<endl;
	srand( (unsigned)time(NULL));
	if(any_negative==1){
		any_negative = -1;
	}
	else{
		any_negative = 1;
	}
	
    for(i=0; i < num_of_wanted_var	; i++ ){
		
			now = (rand() % (bound+1) ) * any_negative +1000;
			
			ofs<<now<<" ";
		
		ofs<<endl;
		
		/*if(now < mini){
			printf("new minimum : %d  exist \n ", now );
			mini = now ;
		}
		if(now > maxi){
			printf("new maximum : %d  exist \n ", now );
			maxi = now ;
		}*/
		
	}
    //getchar();
    
	return 0;

}
