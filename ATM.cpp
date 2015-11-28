#include <iostream>    //cout
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <string>  //string
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h> //hostent
#include <cstdlib> //exit
 
using namespace std;

#include "tcp_client.h"
 
int main(int argc , char *argv[])
{
	if( argc != 2 ) {
		cerr << "Usage: atm <port#>" << endl;
		exit(1);
	}

	int portNum = atoi(argv[1]);

	//connect to host
    tcp_client c;
    c.conn("localhost", portNum);

	/* Enter user */
    string userName;
    cout<<"\nEnter your user name: ";
    cin>>userName;

    // Check if user exists

    int pinNum;
    cout<<"Enter your PIN number: ";
    cin>>pinNum;


//namecard??

//check authentication, if yes, build the connection


    
    //string host;
     
    /*cout<<"Enter hostname: ";
    cin>>host;*/

    /*int portNum;
    cout<<"Enter port number: (test 80 for HTTP)";
    cin>>portNum;*/



     
    
     
    /*send some data, c.send_data return true if 
      send(sock , data.c_str() , strlen( data.c_str() ) , 0) >= 0
      
      send_data (Operator):
      http://www.halcon.com/download/reference/send_data.html
    */
    c.send_data("hello");
     
    //receive and echo reply
    cout<<"----------------------------\n\n";
    cout<<c.receive(1024);
    cout<<"\n\n----------------------------\n\n";
     

    string command;
    cout<<"Hello, how can I help you today? (balance, withdraw, transfer or logout)\n";
    cin>>command;

//if statement...


    return 0;
}
