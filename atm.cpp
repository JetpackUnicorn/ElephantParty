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
#include "Card.h"

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

bool login() {
	if(loggedIn) {
		cout << "Already logged in" << endl;
		return false;
	}
	string username;
	cin >> username;
	if(username == "") {
		cout << "No username entered" << endl;
		return false;
	}

	int pinNum;
	cout << "Enter PIN: ";
	cin >> pinNum;

	// Authentication
	
	loggedIn = true;
	currentUser = username;
	return true;
}

void balance() {
	if(!loggedIn) {
		cout << "Not logged in" << endl;
		return;
	}
	
	sstm.str("");
	sstm << "balance " << currentUser;
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
	sstm << "withdraw " << currentUser << " " << amt;
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
	sstm << "transfer " << currentUser << " " << amt << " " << recAcct;
	string msg = sstm.str();
	if( !encryptAndSend(msg) ) {
		cerr << "ERROR: Could not contact server" << endl;
	}
	else {
		cout << receiveAndDecrypt() << endl;
	}
}

void logout() {
	if(!loggedIn) {
		cout << "Not logged in" << endl;
		return;
	}
	if(!c.disconn()) {
		cerr << "ERROR: Could not disconnect" << endl;
		return;
	}
	
	cout << "Disconnected. Thank you" << endl;
	currentUser = "";
	loggedIn = false;
}


void mainmenu(){
  
  cout<<"\nCommand Options:\n"<<"* login <username>\n"<<"* balance\n"<<"* withdraw <amount>\n"<<"* transfer <amount> <recipient>\n"<<"* logout\n\r";
  
}



int main(int argc , char *argv[])
{
	if( argc != 2 ) {
		cerr << "Usage: atm <port#>" << endl;
		exit(1);
	}

	int portNum = atoi(argv[1]);
	string data, encryptedData;

	Namecard alice;
    	alice.readAcard("Alice2.card");
	cout<<"test - alice card number : "<<alice.getCardNum()<<endl;
    	cout<<"test - alice card name : "<<alice.getCardName()<<endl;

    cout << "Connecting to bank..." << endl;
    if( !c.conn("localhost", portNum) ) {
		cout << "Could not establish connection to bank. Terminating session." << endl;
		return 0;
	} else {
		cout << "Connected. Welcome" << endl;
	}

	string command;
	while(true) {
    
    	mainmenu();
		cout << "\nATM $";
		cin >> command;
    
		if(command == "login") {
			if( !login() ) {
				cout << "Login failed. Terminating session." << endl;
				return 0;
			}
		}
		else if(command == "balance") balance();
		else if(command == "withdraw") withdraw();
		else if(command == "transfer") transfer();
	    else if(command == "logout")
	    {
	      logout();
	      cout<<"lose connection to ATM\n\r";
	      break;
	    }
	    //else mainmenu();
      
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
