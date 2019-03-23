#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <cstring>
#include <queue>
#include <unistd.h>
using namespace std;

char* POINTER_SELF = "." ;
char* POINTER_LAST = ".." ;	
queue<string> directory_list;
struct stat file_info;
struct option_code
{
	bool opcode[4] = {} ;
	//for name = 0 , inode = 1 , min = 2 , max = 3
	string name;
	double size_min,size_max;
	long unsigned int inode;
};

void do_option(DIR* dir, dirent* drnt,const char* path,option_code& op)
{
	char temp_char[256] = "";
	strcat(temp_char,path);
	double FILE_SIZE;
	
	if(op.opcode[1]==true)
	{
		if(drnt->d_ino == op.inode && (strcmp(POINTER_SELF,drnt->d_name) != 0) &&(strcmp(POINTER_LAST,drnt->d_name) != 0) )
		{
			strcat(temp_char,drnt->d_name) ;
			stat(temp_char,&file_info);
			
			if((op.opcode[2]==true) && (op.opcode[3]==true))
			{
				if((file_info.st_size >= op.size_min) && (file_info.st_size <= op.size_max))
				{
					FILE_SIZE = file_info.st_size/1048576.0;
					cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
				}
			}
			else if(op.opcode[2]==true)
			{
				if(file_info.st_size >= op.size_min)
				{
					FILE_SIZE = file_info.st_size/1048576.0;
					cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
				}
			}
			else if(op.opcode[3]==true)
			{
				if(file_info.st_size <= op.size_max)
				{
					FILE_SIZE = file_info.st_size/1048576.0;
					cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
				}
			}
			else
			{
				FILE_SIZE = file_info.st_size/1048576.0;
				cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
			}
		}
	}	
	
	else if(op.opcode[0]==true)
	{
		if( strcmp(drnt->d_name,op.name.c_str()) == 0)
		{
			strcat(temp_char,drnt->d_name) ;
			stat(temp_char,&file_info);
			
			if((op.opcode[2]==true) && (op.opcode[3]==true))
			{
				if((file_info.st_size >= op.size_min) && (file_info.st_size <= op.size_max))
				{
					FILE_SIZE = file_info.st_size/1048576.0;
					cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
				}
			}
			else if(op.opcode[2]==true)
			{
				if(file_info.st_size >= op.size_min)
				{
					cout<<file_info.st_size<<" "<<op.size_min<<endl;
					FILE_SIZE = file_info.st_size/1048576.0;
					cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
				}
			}
			else if(op.opcode[3]==true)
			{
				if(file_info.st_size <= op.size_max)
				{
					FILE_SIZE = file_info.st_size/1048576.0;
					cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
				}
			}
			else
			{
				FILE_SIZE = file_info.st_size/1048576.0;
				cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
			}
		}
	}
	else
	{
		if((strcmp(POINTER_SELF,drnt->d_name) != 0) &&(strcmp(POINTER_LAST,drnt->d_name) != 0))
		{
			strcat(temp_char,drnt->d_name) ;
			stat(temp_char,&file_info);
			
			if((op.opcode[2]==true) && (op.opcode[3]==true))
			{
				if((file_info.st_size >= op.size_min) && (file_info.st_size <= op.size_max))
				{
					FILE_SIZE = file_info.st_size/1048576.0;
					cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
				}
			}
			else if(op.opcode[2]==true)
			{
				if(file_info.st_size >= op.size_min)
				{
					FILE_SIZE = file_info.st_size/1048576.0;
					cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
				}
			}
			else if(op.opcode[3]==true)
			{
				if(file_info.st_size <= op.size_max)
				{
					FILE_SIZE = file_info.st_size/1048576.0;
					cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
				}
			}
			else
			{
				FILE_SIZE = file_info.st_size/1048576.0;
				cout<<temp_char<<" "<<drnt->d_ino<<" "<<FILE_SIZE<<" MB"<<endl;
			}
		}
	}

	
	
}

void NULL_CHECK(char** argv, int i)
{
	if(argv[i] == NULL)
	{
		cerr<<"parameter format error , please check !"<<endl;
		exit(1);
	}	
}

void recusive_seedir(DIR* dir, dirent* drnt,char* path,option_code& op)
{
	//cout<<"test func star"<<endl;
	//cout<<path<<"   test len = "<<strlen(path)<<endl;
	
	if( path[strlen(path)-1] != '/' )strcat(path , "/");
	const char* const_path = path ;
	if(! (dir = opendir(const_path)) ) cerr<<"opendir NULL"<<endl;
	//else cout<<"access success !"<<endl;
	
	struct dirent *temp_dirent;
	string temp_string;
	
	while( (temp_dirent = readdir(dir)) != NULL)
	{
		
		//cout<<"open DIR* dir = "<<path<<" , now reading file = "<<temp_dirent->d_ino<<endl;
		do_option(dir,temp_dirent,const_path,op);
		//INODE在這裡找
		//NAME也在這裡找
		//MAX MIN 也在這找
		if( (temp_dirent->d_type == DT_DIR) && ( (strcmp(temp_dirent->d_name,POINTER_SELF) != 0) && (strcmp(temp_dirent->d_name,POINTER_LAST) != 0) ) )
		{
			//cout<<(temp_dirent->d_name)<<"|"<< ((temp_dirent->d_name) != POINTER_SELF )<<"|"<<strcmp((temp_dirent->d_name),POINTER_SELF)<<endl;
			char* now_path = strdup(const_path) ;
			const char* sub_path = strcat( now_path ,temp_dirent->d_name);
			temp_string.assign(sub_path);
			directory_list.push(temp_string);
			//cout<<"enter the branch and the sub path added to the queue = "<<temp_string<<endl;
			
			//recusive_seedir(dir,drnt,sub_path);
		}
		
	}
	
	closedir(dir);
	while(!directory_list.empty())
	{
		temp_string = directory_list.front();
		directory_list.pop();
		char* next_path = strdup( temp_string.c_str() );
		recusive_seedir(dir,drnt,next_path,op);
	}
	
	
}

int main(int argc, char** argv)
{
	string argument;
	string name ;
	double mini_num = -1 , maxi_num = -1 ;
	long unsigned int inode_num = 0 ;
	option_code opcode;
	string path_name = argv[1];
	for(int i = 2 ; i < 99 ; i++)
	{
		if(argv[i] == NULL ) break;
		argument = argv[i] ;
		
		if(argument == "-inode")
		{	
			NULL_CHECK(argv,i+1);
			argument = argv[i+1];
			inode_num = stoi(argument);
			opcode.inode = inode_num ;
			i++;
		}		
		else if(argument == "-name")
		{
			NULL_CHECK(argv,i+1);
			name = argv[i+1];
			opcode.name = name ;
			i++;
		}		
		else if(argument == "-size_min")
		{
			NULL_CHECK(argv,i+1);
			argument = argv[i+1];
			mini_num = stod(argument);
			opcode.size_min = mini_num * 1048576.0;
			i++;
		}
		else if(argument == "-size_max" )
		{
			NULL_CHECK(argv,i+1);
			argument = argv[i+1];
			maxi_num = stod(argument);
			opcode.size_max = maxi_num * 1048576.0;
			i++;
		}
		else
		{
			cerr<<"argument error in the "<<i<<" argument : "<<argument<<" , please check !"<<endl;
			exit(1);
		}
		
	}
	
	//cout<<"read parameter success ! , pathname : "<<path_name;
	if(!name.empty())	opcode.opcode[0] = true ;//cout<<" , name : "<<name;
	else				opcode.opcode[0] = false ;
	if(inode_num != 0)	opcode.opcode[1] = true ;//cout<<" , inode number :"<<inode_num;
	else				opcode.opcode[1] = false ;
	if(mini_num != -1 )	opcode.opcode[2] = true ;//cout<<" , size_min : "<<mini_num<<" MB";
	else				opcode.opcode[2] = false ;
	if(maxi_num != -1 )	opcode.opcode[3] = true ;//cout<<" , size_max : "<<maxi_num<<" MB";
	else				opcode.opcode[3] = false ;
	cout<<endl;
	
	bool check;
	DIR* directory;
	struct dirent* dir_data;
	char* path = strdup( path_name.c_str() );
	
	
	recusive_seedir(directory,dir_data,path,opcode);



	
	return 0;
}
