#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <queue>
#include <iomanip>
#include <sys/time.h>
#include <deque>
using namespace std;

class LinkedList_Node;
class Doubly_Linked_List;

struct DataBlock{
	short int MemoryKey;		//四位數字
	short int DataLocation;		//資料存在MemoryBlock的位置
	LinkedList_Node* LRU_location;
};

class LinkedList_Node{
	short int MemoryKey;
	LinkedList_Node* prev;
	LinkedList_Node* next;
	
public:
	LinkedList_Node* get_location(short int data){return this;}
	friend class Doubly_Linked_List;
};

class Doubly_Linked_List{
	
	unsigned int size_count;
public:
	Doubly_Linked_List();
	~Doubly_Linked_List(){;}
	LinkedList_Node* head;
	LinkedList_Node* back;
	void retach(LinkedList_Node* delete_node);
	LinkedList_Node* push_back(short int data);
	LinkedList_Node* swap_victim(short int data);
	short int see_victim(){
		//cout<<head<<endl;
		return (head->MemoryKey) ;
		}
	bool is_full(unsigned int TableSize){return ((TableSize == size_count) ? true : false) ;}
	
};

Doubly_Linked_List::Doubly_Linked_List():head(NULL),back(NULL),size_count(0){
	
}

LinkedList_Node* Doubly_Linked_List::push_back(short int data){
	
	
	size_count++;
	//cout<<size_count<<endl;
	LinkedList_Node* temp = new(LinkedList_Node);
	temp->MemoryKey = data;
	temp->next = NULL;
	if(head == NULL)
	{
		//temp->MemoryKey = data;
		//temp->next = NULL;
		temp->prev = NULL;
		head = temp ; 
		back = temp ;
	}
	else
	{
		
		//temp->MemoryKey = data;
		//temp->next = NULL;
		temp->prev = back;
		
		back->next = temp;
		back = temp;
	}
	
	
	return temp;
}

LinkedList_Node* Doubly_Linked_List::swap_victim(short int data){
												//cout<<"0"<<endl;
	LinkedList_Node* temp = head; 				//cout<<"1"<<endl; 				
	LinkedList_Node* temp_next = head->next ;	//cout<<"2"<<endl;
	temp_next->prev = NULL;						//cout<<"3"<<endl;
	temp->next = NULL ;							//cout<<"4"<<endl;
	head = temp_next;							//cout<<"5"<<endl;
	
	temp->prev = back ;							//cout<<"6"<<endl;
	back->next = temp ;							//cout<<"7"<<endl;
	back = temp ;								//cout<<"8"<<endl;
	
	back->MemoryKey = data;						//cout<<"9"<<endl;
	
	if(head == NULL){
		cerr<<"head broken victim"<<endl;
		exit(1);
	}
	return temp ;
	
}

void Doubly_Linked_List::retach(LinkedList_Node* delete_node){
	//LinkedList_Node* temp = back		;										//cout<<"0"<<endl;
	LinkedList_Node* temp_next = delete_node->next;								//cout<<"1"<<endl;
	LinkedList_Node* temp_prev = delete_node->prev;								//cout<<"2"<<endl;
	
	if((delete_node->next != NULL) && (delete_node->prev != NULL))
	{
		temp_next->prev = delete_node->prev ;			//cout<<"3"<<endl;
		temp_prev->next = delete_node->next ;	
		delete_node->prev = back;
		delete_node->next = NULL;
		back->next = delete_node ;
		back = delete_node ;
	}
	else if((delete_node->next != NULL) && (delete_node->prev == NULL))
	{//cout<<"123"<<endl;
		temp_next->prev = delete_node->prev ;
		head = delete_node->next;
		delete_node->prev = back;
		delete_node->next = NULL;
		back->next = delete_node ;
		back = delete_node ;//cout<<"789"<<endl;
	}
	
	else if((delete_node->next == NULL)&& (delete_node->prev != NULL))
	{//cout<<"100"<<endl;
		temp_prev->next = delete_node;	
		delete_node->next = NULL;
		//back->next = delete_node ;
		back = delete_node ;
	}
	else ;
	//cout<<"222"<<endl;
	//else back = delete_node->prev ;	
	//如果delete的node沒有下一個的化  delete 本身就是最後一個
	/*if(delete_node->prev != NULL )temp_prev->next = delete_node->next ;			//cout<<"4"<<endl; //fault 
	else if(delete_node->next != NULL )head = delete_node->next ;
	*/
	//if(delete_node == head && delete_node->next != NULL)head = delete_node->next ;
	
	//delete_node->next = NULL;				//cout<<"5"<<endl;
	//delete_node->prev = back;													//cout<<"6"<<endl;
	//back->next = delete_node ;													//cout<<"7"<<endl;
	//back = delete_node ;														//cout<<"8"<<endl;
	
	if(head == NULL){
		cerr<<"head broken retach , delete_node :"<<delete_node<<" prev :"<<delete_node->prev <<" next :"<<delete_node->next <<endl;
		exit(1);
	}
	//cout<<"456"<<endl;
}



class PageTable{
public:
	PageTable();
	void LoadData(int TableSize, vector<short int> data);
	void Algorithm_FIFO();
	void Algorithm_LRU();
	int Get_Hit_Count(){	return Hit_Count ;	};
	int Get_MisCount(){		return MisCount	;	};
	double Get_Page_Fault_Ratio(){return Page_Fault_Ratio =(  static_cast<double>(MisCount) / static_cast<double>(MisCount + Hit_Count)) ; };
private:
	vector<short int> DupMemoryData;
	vector<short int> MemoryBlock;
	//queue<short int> FIFO_Table;
	
	int Hit_Count,MisCount;
	double Page_Fault_Ratio;
	//short int *HashTable;
	vector<DataBlock> HashTable;
	//list<DataBlock> LRU_Table;
};

PageTable::PageTable()
{
	
	DataBlock temp;
	temp.MemoryKey = -1;
	temp.DataLocation = -1;
	//(HashTable.assign(9000,-1)).DataLocation;
	for(int i=0;i<9000;i++)
	{
		HashTable.push_back(temp);
		//HashTable[i].MemoryKey = -1 ;
		//HashTable[i].DataLocation = -1 ;
	}
	//HashTable = new (short int)[9000];
}

void PageTable::LoadData(int TableSize, vector<short int> data)
{
	DupMemoryData = data ;					//複製資料
	MemoryBlock.clear();
	MemoryBlock.assign( TableSize , 0 );	//產生MEMORY BLOCK
	
	Hit_Count = 0;
	MisCount = 0;
	DataBlock temp;
	temp.MemoryKey = -1;
	temp.DataLocation = -1;
	temp.LRU_location = NULL;
	//(HashTable.assign(9000,-1)).DataLocation;
	for(int i=0;i<9000;i++)
	{
		//HashTable.push_back(temp);
		HashTable[i].MemoryKey = -1 ;
		HashTable[i].DataLocation = -1 ;
	}
	
}

void PageTable::Algorithm_FIFO()
{
	/// know trace(vector : DupMemoryData) , page size (vector : MemoryBlock) ///
	short int TableSize = static_cast<short int> (MemoryBlock.size());
	//MemoryBlock.size();
	short int replaced = 0 ;
	short int param_memoryblock=0;
	
	unsigned int index = 0 ;
	bool DATA_HIT = false;
	queue<short int> FIFO_Table;
	//DupMemoryData進去
	for( ; index < DupMemoryData.size() ; index++ )
	{
		//找MemoryBlock有沒有要得PAGE(雜湊)
		replaced = DupMemoryData[index];
		if(replaced != HashTable[replaced - 1000].MemoryKey  ) //如果對應的TABLE無效 FALSE
		{	
			DATA_HIT = false ;
			MisCount++;	
		}
		else
		{ 
			DATA_HIT = true ;
			Hit_Count++;
		}
		//沒有 : MIS +1 , 資料丟進去
		if(DATA_HIT == false)
		{
			
			if(MemoryBlock.size() == FIFO_Table.size() ) //當queue的size和memoryblock一樣大時
			{
				
				//MemoryBlock滿載 ,找FIFO_Table第一個資料 拿去MemoryBlock找出來 踢出去
				//FIFO_Table pop . FIFO丟進去的資料 push進去
				replaced = FIFO_Table.front();
				FIFO_Table.pop();
				
				//丟replaced進去 找數字
				
				if(replaced != HashTable[replaced-1000].MemoryKey){
					cerr<<"error , replace != key  , replaced = "<<replaced<<" and HashTable = "<<HashTable[replaced-1000].MemoryKey<<endl;
					cerr<<"MemoryBlock size :"<<MemoryBlock.size()<<"  FIFO_Table.size = "<<FIFO_Table.size()<<"index = "<<index<<endl;
					exit(1);
				}
				
				HashTable[replaced-1000].MemoryKey = -1 ;//標記失效
				replaced = HashTable[replaced-1000].DataLocation; //得到tablesize的位置
				
				MemoryBlock[replaced] = DupMemoryData[index]; //victim 被複寫 GG
				FIFO_Table.push( MemoryBlock[replaced] ); //丟KEY進去
				HashTable[ MemoryBlock[replaced]-1000 ] . DataLocation = replaced;
				HashTable[ MemoryBlock[replaced]-1000 ] . MemoryKey = MemoryBlock[replaced];
				
			}
			else
			{
				//MemoryBlock沒滿，直接丟資料進去
				
				replaced = DupMemoryData[index];
				FIFO_Table.push(replaced);
				//只有前面index < MemoryBlock的時候才會沒滿
				MemoryBlock[param_memoryblock] = replaced ;
				HashTable[replaced-1000].DataLocation = param_memoryblock ;
				HashTable[replaced-1000].MemoryKey = replaced ;
				param_memoryblock++;
				if(param_memoryblock > TableSize)
				{
					cerr<<"param > table size , error !"<<endl;
				}
				//cout <<"trace :"<<DupMemoryData[index]<<"  Hash KEY : "<<HashTable[replaced-1000].MemoryKey 
				//<<" Hash Location :"<<HashTable[replaced-1000].DataLocation<<endl;			
				//FIFO_table直接push			
				
			}				
		}	
		//有 : HIT +1 , TIFO_Table 不用動				
	}
}


void PageTable::Algorithm_LRU()
{
	/// know trace(vector : DupMemoryData) , page size (vector : MemoryBlock) ///
	short int TableSize = static_cast<short int> (MemoryBlock.size());
	//MemoryBlock.size();
	short int replaced = 0 ;
	short int param_memoryblock=0;
	unsigned int index = 0 ;
	Doubly_Linked_List LRU_Table;
	bool DATA_HIT = false;
	//BST LRU_Table;
	//int xxx=0;
	//DupMemoryData進去
	for( ; index < DupMemoryData.size() ; index++ )
	{ //cout<<xxx<<endl;xxx++;
		//找MemoryBlock有沒有要得PAGE(雜湊)
		//cout<<"index "<<index<<endl;
		//LRU_Table.push_back(DupMemoryData[index]);
		/*
		if(index == 7  ){
			cout<<"  address : "<<&LRU_Table.back()<<endl;
			LRU_Table.erase(LRU_Table.begin()+3);
			cout<<"fot test index = "<<index <<"  address of 5: "<<&LRU_Table[5]<<"   data  : " << LRU_Table[5]<<endl;
		}
		cout<<"fot test index = "<<index <<"  address : "<<&LRU_Table[index]<<" data  : " << LRU_Table[index]<<endl;
		*/
		replaced = DupMemoryData[index];
		if(replaced != HashTable[replaced - 1000].MemoryKey  )
		{	
			DATA_HIT = false ;
			MisCount++;
		}
		else
		{ 
			DATA_HIT = true ;
			Hit_Count++;
		}
		//沒有 : MIS +1 , 資料丟進去
		//test 
		
		///////////////////////////////////////////////
		if(DATA_HIT == false)
		{
			//cout<<MemoryBlock.size()<<endl;
			if( LRU_Table.is_full(MemoryBlock.size()) ) //當queue的size和memoryblock一樣大時
			{
				//MemoryBlock滿載 ,找FIFO_Table第一個資料 拿去MemoryBlock找出來 踢出去
				//FIFO_Table pop . FIFO丟進去的資料 push進去
				//cout<<"find victim "<<endl;
				replaced = LRU_Table.see_victim();
				
				//cout<<"find victim 2"<<endl;
				//丟replaced進去 找數字
				/*
				if(replaced != HashTable[replaced-1000].MemoryKey){
					cerr<<"error , replace != key  , replaced = "<<replaced<<" and HashTable = "<<HashTable[replaced-1000].MemoryKey<<endl;
					cerr<<"MemoryBlock size :"<<MemoryBlock.size()<<"  LRU_Table.size = "<<LRU_Table.size()<<"index = "<<index<<endl;
					exit(1);
				}*/
				
				HashTable[replaced-1000].MemoryKey = -1 ;//標記失效
				HashTable[replaced-1000].LRU_location = NULL ; //標記失效
				replaced = HashTable[replaced-1000].DataLocation; //得到tablesize的位置
				//cout<<"find victim 3"<<endl;
				MemoryBlock[replaced] = DupMemoryData[index]; //victim 被複寫 GG
				//LRU_Table.push_back( MemoryBlock[replaced] ); //丟KEY進去
				HashTable[ MemoryBlock[replaced]-1000 ] . DataLocation = replaced;
				HashTable[ MemoryBlock[replaced]-1000 ] . MemoryKey = MemoryBlock[replaced];
				//cout<<"find victim 4"<<endl;
				HashTable[ MemoryBlock[replaced]-1000 ] . LRU_location = LRU_Table.swap_victim( DupMemoryData[index] );
				//cout<<"find victim 5"<<endl;
			}
			else
			{
				//MemoryBlock沒滿，直接丟資料進去
				//cout<<"others "<<endl;
				replaced = DupMemoryData[index];
				
				//只有前面index < MemoryBlock的時候才會沒滿
				MemoryBlock[param_memoryblock] = replaced ;
				HashTable[replaced-1000].LRU_location = LRU_Table.push_back(replaced);
				HashTable[replaced-1000].DataLocation = param_memoryblock ;
				HashTable[replaced-1000].MemoryKey = replaced ;
				//HashTable[replaced-1000].recently_used = 0 ;
				param_memoryblock++;
				//cout <<"not full  , trace :"<<DupMemoryData[index]<<"  Hash KEY : "<<HashTable[replaced-1000].MemoryKey 
				//<<" Hash Location :"<<HashTable[replaced-1000].DataLocation<<"  LRU location :"<<HashTable[replaced-1000].LRU_location<<endl;
				if(param_memoryblock > TableSize)
				{
					cerr<<"param > table size , error ! param = "<<param_memoryblock<<"   TableSize = "<<TableSize<<endl;
					exit(1);
				}
				
				//FIFO_table直接push
				
				
			}
			
			
		}
		else
		{
			//有 : HIT +1 ,LRU_Table 挑出數字直接往後面丟
			//cout<<"hit !! "<<HashTable[replaced-1000].LRU_location<<"  replaced :" <<replaced<<endl;
			LRU_Table.retach( HashTable[replaced-1000].LRU_location );
			
			
			
		}	
		
	}
	
}
int main(int argc, char** argv)
{
	ifstream ifs;
	struct timeval start,end;
	int sec ;// = end.tv_sec - start.tv_sec ;
	int usec ;//= end.tv_usec - start.tv_usec ;
	string name;
	cout<<"Please enter the file name : ";
	cin>>name;
	cout<<endl;
	ifs.open(name);
	//ifs.open("trace.txt");
	if(!ifs.is_open())
	{
		cerr<<"Can not open the file , please check . "<<endl;
		exit(1);
	}
	
	vector<short int> MemoryData;
	short int StreamTemp;
	
	while(ifs>>StreamTemp)
	{
		MemoryData.push_back(StreamTemp);
	}
	
	//cout<<"test end size = "<<MemoryData.size()<<"  size : "<<MemoryData.max_size()<<endl;
	//cout<<sizeof(short int)<<" "<<sizeof(int)<<endl;
	
	/////////		readfile end 		//////////////
	cout<<"FIFO-------------------------------------------------"<<endl;
	cout<<setw(5)<<"Size"<<setw(10)<<"miss"<<setw(10)<<"hit"<<setw(22)<<"page fault ratio"<<endl;
	gettimeofday(&start,0);
	
	
	PageTable FIFO, LRU;
	
	for(int TableSize = 128 ; TableSize <= 1024 ; TableSize *= 2 )
	{
		
		FIFO.LoadData(TableSize,MemoryData)	;
		FIFO.Algorithm_FIFO();
		cout<<showpoint<<setw(5)<<TableSize<<setw(11)<<FIFO.Get_MisCount()<<setw(11)
			<<FIFO.Get_Hit_Count()<<setw(17)<<setprecision(9)<<fixed<<FIFO.Get_Page_Fault_Ratio()<<endl;
		
	
		
	}
	cout<<"LRU--------------------------------------------------"<<endl;
	cout<<setw(5)<<"Size"<<setw(10)<<"miss"<<setw(10)<<"hit"<<setw(22)<<"page fault ratio"<<endl;
	
	for(int TableSize = 128 ; TableSize <= 1024 ; TableSize *= 2 )
	{
		//PageTable.assign(TableSize,0);
		LRU.LoadData(TableSize,MemoryData)	;
		LRU.Algorithm_LRU();
		cout<<showpoint<<setw(5)<<TableSize<<setw(11)<<LRU.Get_MisCount()<<setw(11)
			<<LRU.Get_Hit_Count()<<setw(17)<<setprecision(9)<<fixed<<LRU.Get_Page_Fault_Ratio()<<endl;
	}
	
	
	
	
	gettimeofday(&end,0);
	sec  = end.tv_sec - start.tv_sec ;
	usec = end.tv_usec - start.tv_usec ;
	cout<<"Total Elapsed Time : "<<setw(10)<<sec*1+(usec/1000000.0)<<" second   "<<endl;



	return 0 ;
}
