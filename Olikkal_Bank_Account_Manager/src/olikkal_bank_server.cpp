#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <sys/time.h>

using namespace std;

#define VALUE 256

void *transactionHandler(void *);

char* buffer[VALUE];
char* name[VALUE];
int lines = 0;
string records[VALUE];
int id[VALUE];
double accbalance[VALUE];
string nam[VALUE];
int bnd, socketfd, newsockfd, portno;
int *new_sock;
struct sockaddr_in serveradd, clientaddr;
//struct timeval start_time, end_time; 
 
 
fstream olikkal_bankRecords;
int i = 0;
int accid, amount;
char op[2];
char recordData[VALUE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int x = 0;
double interest;
int t;
float rate = 0.05;

size_t len = 0; 
 
int main(int argc,char** argv){
	if(argc < 2){
		std::cerr<<"Kindly enter: server <port_number>"<<std::endl;
		exit(1); 
	}
	
	string ch;
	olikkal_bankRecords.open("Records.txt", ios::in | ios::out);
	if(!olikkal_bankRecords){
		std::puts("Error opening the file\n");
	}
	
	while(std::getline(olikkal_bankRecords, ch)){
		lines++;				
	}
	cout << "Number of accounts in the records : "<<lines << endl; 
	olikkal_bankRecords.clear();
	//cout << olikkal_bankRecords.tellg()<< endl;
	olikkal_bankRecords.clear();
	olikkal_bankRecords.seekg(0, ios::beg);

	
	for(i = 0; i<lines; i++){
		records[i] = "";
		std::getline(olikkal_bankRecords, records[i]);
		char *p;
		char a[VALUE];
		strcpy(a, records[i].c_str());
		p = strtok(a, " ");
		
		if(p){
			id[i] = atoi(p);		
		}
		
		p = strtok(NULL, " ");
		if(p){
			name[i] = p; 		
			nam[i] = name[i];
		}
		p = strtok(NULL, " ");
		if(p){
			accbalance[i] = atoi(p);
		}
	}
	
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0){
		std::cerr<<"Error in creating a socket"<<std:: endl;
	}
	/*int reuse;
	if(setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *)&reuse, sizeof(int)) == -1){
			std::cerr<<"Error in reusing port"<<std:: endl;
	
	}
	*/
	bzero((char *)&serveradd, sizeof(serveradd));
	
	portno = atoi(argv[1]);
	serveradd.sin_family = AF_INET;
	serveradd.sin_addr.s_addr = htonl(INADDR_ANY);
	serveradd.sin_port = (in_port_t) htons(portno);
	
	bnd = bind(socketfd, (struct sockaddr *)&serveradd, sizeof(serveradd)) ;
	if(bnd < 0){
		//cout<<bnd;
		std::cerr<<"\n"<<"Error in binding socket"<<std:: endl;
	}	
	
	listen(socketfd, 5);
	
	socklen_t clientlen = sizeof(clientaddr);
	
	pthread_mutex_init(&mutex, NULL);
	
	while(newsockfd = accept(socketfd, (struct sockaddr*)&clientaddr, &clientlen)){
		if(newsockfd < 0){
			std::cerr << "Error accepting client request"<<std:: endl;
		}
		
		else{
			cout<<"*********************WAITING FOR CONNECTING TO CLIENT*********************"<<"\n\n"<<endl;
			cout<<"Connection between the client and server is established"<<"\n"<<endl;
			
		}
				
		pthread_t tid;
		new_sock = (int *)malloc(1);
		*new_sock = newsockfd;
		
		if(pthread_create(&tid, NULL, transactionHandler, (void *) new_sock) < 0){
		cout<<"Could not create thread";
		return 1;
		}
	}
	
	close(newsockfd);
	pthread_exit(NULL);
	
	close(socketfd);
	return 0;
}

void *transactionHandler(void *socket_desc)
{

	x++;
	
	bzero(buffer, VALUE);
	int newsocket = *(int *)socket_desc;
	
	cout<<"Thread "<<"\t"<< pthread_self() <<"\n";
	bzero(recordData, VALUE);
	int n = 0;
	int trans = 1;
	while(n = read(newsocket, recordData, VALUE))
	{
		if(n < 0)
		{
			std::cerr<<"Error receiving data"<<std:: endl;
			exit(1);
		}
		
		if(strlen(recordData) == 0)
		{
			continue;
		}
		//gettimeofday(&start_time, NULL);
		cout<<"Data received from client: "<<"\t"<<recordData<<"\n";
		//cout<<"Thread number "<< pthread_self() <<"\t"<<"\n";
		cout<<"Transaction "<<trans<<endl<<"\n";
		
		char* p;
		p = strtok(recordData, " ");
		p = strtok(NULL, " ");
		//cout<<recordData<<endl;
		if(p)
		{
			accid = atoi(p);
		}
		
		//cout<<accid;	
		p = strtok(NULL, " ");
	
		if(p)
		{
			strcpy(op, p);
		}
		p = strtok(NULL, " ");
	
		if(p)
		{
			amount = atoi(p);
		}
		
		//cout<<"\t"<<amount<<"\t";
		
		int size = 0;
		int exist = 0;
		
	
		//cout<<lines<<"\n";
		for (i = 0; i < lines; i++)
		{
		
			if (i == 0)
				size = 0;
			else
				size += (floor(log10(abs(id[i-1]))) + 1) + (strlen(nam[i-1].c_str())) + (floor(log10(abs(accbalance[i-1]))) + 1) + 3;
			
			t = (abs(long(recordData) - (5*trans)));
			if(t > 0){
				interest = accbalance[i] * rate * trans;
				accbalance[i] = accbalance[i] + int(interest);
			}
			
			if (accid == id[i])
			{
				pthread_mutex_lock(&mutex);
				cout<<"The account id exists in the bank records"<<"\n\n";
				
				if(strcmp(op, "w") == 0)
				{
					cout<<"######Withdrawing amount######"<<"\n";
					
					if(accbalance[i] - amount > 0)
					{
						accbalance[i] =  accbalance[i] - amount;
						cout<<"Amount balance : "<<accbalance[i]<<endl;
						olikkal_bankRecords.clear();
						olikkal_bankRecords.seekg(size, ios::beg);
						olikkal_bankRecords<<id[i] <<" "<<nam[i]<<" "<<accbalance[i]<<flush<<endl;
						cout<<"\n"<<"Confirmation of Withdrawal sent to client"<<"\n"<<endl;
						
						cout<<"\n"<<"******************************END OF TRANSACTION****************************"<<endl;
						
						n = write(newsocket, "Amount withdrawn\n", 50);
						trans++;
						
						if(n < 0)
							std::cerr<<"Error writing to socket\n"<<std:: endl;
					}
				
					else
					{
						cout<<"Insufficient balance : "<<accbalance<<"\n";
						cout<<"Confirmation sent"<<endl;
						cout<<"\n"<<"******************************END OF TRANSACTION****************************"<<"\n"<<endl;
					
						n = write(newsocket, "Error !!! :Insufficient balance\n", 50);
						if(n < 0)
							std::cerr<<"Error writing to socket"<<std:: endl;
					
					}
				}
								
				else if(strcmp(op, "d") == 0)
				{
					olikkal_bankRecords.clear();
					olikkal_bankRecords.seekg(0, ios::beg);
					cout<<"######Depositing amount######"<<"\n"<<endl;
					accbalance[i] = accbalance[i] + amount;
					cout<<"Amount balance : " <<accbalance[i]<<"\t"<<endl;
					olikkal_bankRecords.clear();
					olikkal_bankRecords.seekg(size, ios::beg);
					olikkal_bankRecords<<id[i] <<" "<<nam[i]<<" "<<accbalance[i]<<flush<<endl;
					cout<<"\n"<<"Confirmation of Deposit sent to client"<<"\n";
					cout<<"\n"<<"******************************END OF TRANSACTION****************************"<<endl;
					
					n = write(newsocket, "Amount deposited\n", 50);
					trans++;
					
					if (n < 0)
						std::cerr<<"Error writing to socket\n"<<std:: endl;
				}
				
				else
				{
					cout<<"Please provide a vaild amount"<<"\n"<<endl;
					cout<<"\n"<<"******************************END OF TRANSACTION****************************"<<endl;
					
					n = write(newsocket, "Error !!! Invalid transaction type\n", 50);
					trans++;
					if (n < 0)
						std::cerr<<"Error writing to socket\n"<<std:: endl;
				}
				
				exist = 1;
				
				pthread_mutex_unlock(&mutex);
				break;
				trans++;
				//gettimeofday(&end_time, NULL);
			}
		}
		
		if (exist == 0)
		{
			cout<<"The account id does not exist in the bank records"<<"\n"<<endl;
			cout<<"\n"<<"******************************END OF TRANSACTION****************************"<<endl;
			n = write(newsocket, "Error!!! Invalid Account ID\n", 100);
			trans++;
			if(n < 0)
				std::cerr<<"Error writing to socket"<<std:: endl;
		}
		
		//cout<<"Time taken = "<<(end_time.tv_sec - start_time.tv_sec)<<endl;
	}
		

	
	return 0;
}
