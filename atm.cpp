#include <iostream>    //cout
using std::cout;
using std::endl;
#include <stdio.h> //printf
#include <string.h>    //strlen
#include <string>  //string
using std::string;
#include <sstream> //stringstream
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <netdb.h> //hostent
#include <cstdlib> //exit
#include <fstream>

#include <cryptopp/base64.h>
using CryptoPP::Base64Encoder;
using CryptoPP::Base64Decoder;
#include <cryptopp/modes.h>
#include <cryptopp/integer.h>
using CryptoPP::Integer;
#include <cryptopp/files.h>
using CryptoPP::FileSink;
using CryptoPP::FileSource;
#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;
#include <cryptopp/pssr.h>
using CryptoPP::PSSR;
#include <cryptopp/rsa.h>
using CryptoPP::InvertibleRSAFunction;
using CryptoPP::RSASS;
using CryptoPP::RSA;
using CryptoPP::RSAFunction;
using CryptoPP::RSAES_OAEP_SHA_Encryptor;
using CryptoPP::RSAES_OAEP_SHA_Decryptor;
using CryptoPP::PK_EncryptorFilter;
using CryptoPP::PK_DecryptorFilter;
#include <cryptopp/filters.h>
using CryptoPP::SignerFilter;
using CryptoPP::SignatureVerificationFilter;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
#include <cryptopp/cryptlib.h>
using CryptoPP::Exception;
#include <cryptopp/sha.h>
using CryptoPP::SHA1;

 
using namespace std;

#include "tcp_client.h"
#include "Card.h"

bool loggedIn = false;
string currentUser;
tcp_client c;
stringstream sstm;



string RSA_Encryption(const string & plain){
  //Encryption
  AutoSeededRandomPool rng;
  
  //Load public key
  CryptoPP::RSA::PublicKey pubKey;
  CryptoPP::ByteQueue bytes;
  FileSource file("pubkey_bank.txt", true, new Base64Decoder);
  file.TransferTo(bytes);
  bytes.MessageEnd();
  pubKey.Load(bytes);
  
  
  RSAES_OAEP_SHA_Encryptor e(pubKey);
  
  string cipher;
  StringSource ss1(plain, true,
                   new PK_EncryptorFilter(rng, e,
                                          new StringSink(cipher)
                                          ) // PK_EncryptorFilter
                   ); // StringSource
  return cipher;
}

//********************************************************************************

string RSA_Decryption(const string & cipher){
  //Decryption
  AutoSeededRandomPool rng;
  //Load private key
  CryptoPP::RSA::PrivateKey privKey;
  // Load private key
  CryptoPP::ByteQueue bytes;
  FileSource file("privkey_atm.txt", true, new Base64Decoder);
  file.TransferTo(bytes);
  bytes.MessageEnd();
  privKey.Load(bytes);
  
  string recovered;
  
  RSAES_OAEP_SHA_Decryptor d(privKey);
  
  StringSource ss2(cipher, true,
                   new PK_DecryptorFilter(rng, d,
                                          new StringSink(recovered)
                                          ) // PK_DecryptorFilter
                   ); // StringSource
  
  //assert (plain == recovered);
  std::cout << "decrypted plain: " << recovered << std::endl;
  return recovered;
}

//signature
string signature_sign(const string & msg){
  
  // Setup
  string message = msg;
  cout << "unsigned message: "<< msg <<endl;
  RSA::PrivateKey privateKey;
  AutoSeededRandomPool rng;
  
  // Load private key
  CryptoPP::ByteQueue bytes;
  FileSource file("privkey_atm.txt", true, new Base64Decoder);
  file.TransferTo(bytes);
  bytes.MessageEnd();
  privateKey.Load(bytes);
  
  // Sign and Encode
  RSASS<PSSR, SHA1>::Signer signer(privateKey);
  
  string signature;
  // StringSource
  StringSource(message, true,
               new SignerFilter(rng, signer,
                                new StringSink(signature),
                                true // putMessage
                                ) // SignerFilter
               );
  return signature;
}

string signature_verify(const string & signature){
  
  RSA::PublicKey publicKey;
  string recovered, message;
  
  // Load public key
  CryptoPP::ByteQueue bytes;
  FileSource file("pubkey_bank.txt", true, new Base64Decoder);
  file.TransferTo(bytes);
  bytes.MessageEnd();
  publicKey.Load(bytes);
  
  // Verify and Recover
  RSASS<PSSR, SHA1>::Verifier verifier(publicKey);
  
  StringSource(signature, true,
               new SignatureVerificationFilter(
                                               verifier,
                                               new StringSink(recovered),
                                               SignatureVerificationFilter::THROW_EXCEPTION |
                                               SignatureVerificationFilter::PUT_MESSAGE
                                               ) // SignatureVerificationFilter
               ); // StringSource
  

  cout << "Verified Message: " << "'" << recovered << "'" << endl;
  return recovered;
  
}

void SharedKey_Init(){
  
  // InvertibleRSAFunction is used directly only because the private key
  // won't actually be used to perform any cryptographic operation;
  // otherwise, an appropriate typedef'ed type from rsa.h would have been used.
  AutoSeededRandomPool rng;
  InvertibleRSAFunction privkey;
  privkey.Initialize(rng, 1024);
  
  // With the current version of Crypto++, MessageEnd() needs to be called
  // explicitly because Base64Encoder doesn't flush its buffer on destruction.
  Base64Encoder privkeysink(new FileSink("privkey_atm.txt"));
  privkey.DEREncode(privkeysink);
  privkeysink.MessageEnd();
  
  // Suppose we want to store the public key separately,
  // possibly because we will be sending the public key to a third party.
  RSAFunction pubkey(privkey);
  Base64Encoder pubkeysink(new FileSink("pubkey_atm.txt"));
  pubkey.DEREncode(pubkeysink);
  pubkeysink.MessageEnd();
  
}


bool encryptAndSend(string msg) {
	
  // encryption
  string encrypted_msg = RSA_Encryption(msg);
  // sign
  string signed_msg = signature_sign(encrypted_msg);
  
	return c.send_data(signed_msg);
}

string receiveAndDecrypt() {

	string resp = c.receive(512);
  // verify
  string verified_msg = signature_verify(resp);
	// decryption
  string decrypted_msg = RSA_Decryption(verified_msg);
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
	Namecard card;
	if(!card.readAcard(username+".card")) {
		cout << "Card not found." << endl;
		return false;
	}

	sstm.str("");
	sstm << "authenticate " << card.getCardNum() << " " << pinNum;
	string msg = sstm.str();
	if( !encryptAndSend(msg) ) {
		cerr << "ERROR: Could not contact server" << endl;
	}
	else {
		string resp = receiveAndDecrypt();
		if(resp != "Authenticated.") {
			cout << "Authentication failed." << endl;
			return false;
		}
		else {
			cout << "Welcome" << endl;
		}
	}
	
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

  //generate initial key pair
  SharedKey_Init();
  
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
		cout << "Connected." << endl;
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
