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

using namespace std;

#define MAXFILE 500
int main(int argc, char **argv)
{

	if(argc < 4)
	{
		std::cerr<<"Kindly use the format <host address> <port number> <timestamp> <filename>"<<std::endl;
		exit(1);
	}
	
	string buffer = "";
	int x=0;
	char ch;
	char line[200];
	char *customerTransaction;
	customerTransaction = argv[4];

	//open the filename received from the command line
	fstream cusTransFile;
	cusTransFile.open(customerTransaction, ios::in | ios::out); 
	if(!cusTransFile)
	{
		cout<<"Error in opening file"<<endl;
	}                                                              

	struct hostent *server;
	int portno;
	double timestamp;
	//extract values from command line arguements
	portno = atoi(argv[2]);
	timestamp = atof(argv[3]);
	server = gethostbyname(argv[1]);
	if(server == NULL)
	{
		std::cerr<<"No such server exists"<<"\n"<<std::endl;
		exit(1);
	}

	struct sockaddr_in server_addr;

	server_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,server->h_length);
	server_addr.sin_port = htons(portno);	


	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	//cout<<socketfd<<endl;
	if(socketfd < 0)
	{
		std::cerr<<"Error in socket creation"<<std:: endl;
		exit(0);
	}

    double time;
    double initial_time;
    //connect to server socket
	if(connect(socketfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr<<"Error while connecting"<<std:: endl;
	}

	x++;
	int size=0;
	char a[MAXFILE];
    int transactions=0;
	char b[MAXFILE];   
    //get each transaction and send it one by one
    while (getline(cusTransFile, buffer))
    {
    	transactions++;
    	initial_time = initial_time + 1;
    	char *p;
			
		strcpy(a, buffer.c_str());
    	
		//cout<<a<<endl;
		for(int z = 0; z < strlen(a); z++)
			b[z] = a[z];
    	
		p=strtok(b," ");
		
    	if(p)
    	{
    		time=atoi(p);
    	}
    	
    	while(initial_time!=time)
    	{
    		if(initial_time>time)
    		{
    			sleep((initial_time-time)*timestamp);
    			initial_time = initial_time + 1;
    		}

    		else
    		{
    			sleep((time - initial_time)*timestamp);
    			initial_time = initial_time + 1;
    		}
    	
    	}

    	int n;
    	if(initial_time == time)
    	{
    		cout<<"Requesting transaction to be processed"<<"\t"<<endl;
    		
			n = write(socketfd, a, sizeof(a));
			if(n < 0)
			{
				cout<<"Error while writing to socket"<<"\n";
				exit(1);	
			}
			bzero(b, MAXFILE);
			//read the acknowledgement received from server
			n = read(socketfd, a, MAXFILE);
			cout<<"Processed : "<<a;
			cout<<"******************************END OF TRANSACTION********************************"<<"\n"<<endl;

	    }

		else{exit(0);}
	  	 	
    }	
		
	close(socketfd);
		
	cusTransFile.close();	
	 
	return 0;
}

