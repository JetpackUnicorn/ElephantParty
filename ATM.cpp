#include <iostream>    //cout
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <string>  //string
#include <sstream> //stringstream
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h> //hostent
#include <cstdlib> //exit
 
using namespace std;

#include "tcp_client.h"

bool loggedIn = false;
string currentUser;
tcp_client c;
stringstream sstm;

bool encryptAndSend(string msg) {
	
	// encryption
	string encrypted_msg = msg;

	return c.send_data(encrypted_msg);
}

string receiveAndDecrypt() {

	string resp = c.receive(512);

	// decryption
	string decrypted_msg = resp;

	return decrypted_msg;
}

void login(int portNum) {
	if(loggedIn) {
		cout << "Already logged in" << endl;
		return;
	}
	string username;
	cin >> username;
	if(username == "") {
		cout << "No username entered" << endl;
		return;
	}

	int pinNum;
	cout << "Enter PIN: ";
	cin >> pinNum;

	// Authentication

	c.conn("localhost", portNum);
	loggedIn = true;
	currentUser = username;
}

void balance() {
	if(!loggedIn) {
		cout << "Not logged in" << endl;
		return;
	}
	
	sstm.str("");
	sstm << currentUser << " BALANCE";
	string msg = sstm.str();
	if( !encryptAndSend(msg) ) {
		cerr << "ERROR: Could not contact server" << endl;
	}
	else {
		cout << receiveAndDecrypt() << endl;
	}
}

void withdraw() {
	if(!loggedIn) {
		cout << "Not logged in" << endl;
		return;
	}
	int amt = -1;
	cin >> amt;
	if( amt < 0 ) {
		cout << "Invalid amount" << endl;
		return;
	}

	// funds check

	sstm.str("");
	sstm << currentUser << " WITHDRAW " << amt;
	string msg = sstm.str();
	if( !encryptAndSend(msg) ) {
		cerr << "ERROR: Could not contact server" << endl;
	}
	else {
		cout << receiveAndDecrypt() << endl;
	}
}

void transfer() {
	if(!loggedIn) {
		cout << "Not logged in" << endl;
		return;
	}
	int amt = -1;
	cin >> amt;
	if( amt < 0 ) {
		cout << "Invalid amount" << endl;
		return;
	}
	string recAcct = "";
	cin >> recAcct;
	if( recAcct == "" ) {
		cout << "Receiver account not entered" << endl;
		return;
	}

	// account verification check
	// funds check

	sstm.str("");
	sstm << currentUser << " TRANSFER " << amt << " " << recAcct;
	string msg = sstm.str();
	if( !encryptAndSend(msg) ) {
		cerr << "ERROR: Could not contact server" << endl;
	}
	else {
		cout << receiveAndDecrypt() << endl;
	}
}

int main(int argc , char *argv[])
{
	if( argc != 2 ) {
		cerr << "Usage: atm <port#>" << endl;
		exit(1);
	}

	int portNum = atoi(argv[1]);
	string data, encryptedData;

	string command;
	while(true) {
		cout << "ATM $";
		cin >> command;

		if(command == "login") login(portNum);
		else if(command == "balance") balance();
		else if(command == "withdraw") withdraw();
		else if(command == "transfer") transfer();
		//else if(command == "logout") logout();
	}


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
    /*c.send_data("hello");
     
    //receive and echo reply
    cout<<"----------------------------\n\n";
    cout<<c.receive(1024);
    cout<<"\n\n----------------------------\n\n";*/
     

    /*string command;
    cout<<"Hello, how can I help you today? (balance, withdraw, transfer or logout)\n";
    cin>>command;*/

//if statement...


    return 0;
}
