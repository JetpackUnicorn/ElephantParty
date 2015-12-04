#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include "card.h"
//#include "protocol.h"
#include "crypto.h"


typedef std::vector<char> Packet;

using namespace std;

Crypto crypto;

bool loggedIn = false;
string currentUser;
stringstream sstm;
int sock;
bool sock_alive;

bool login();
void balance(); 
void withdraw();
void transfer();
void logout();
bool encryptAndSend(const string & msg);
string receiveAndDecrypt();
string signature_verify(const string & signature);

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("Usage: atm proxy-port\n");
        return -1;
    }
  
    //string keyfile = crypto.key_filenameGen();
    //string opposite_keyfile = keysharing(keyfile);
    crypto.SharedKeyInit("pubkey_atm.txt");
    crypto.LoadOppositeKey("pubkey_bank.txt");
    unsigned short proxport = atoi(argv[1]);
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(!sock)
    {
        printf("fail to create socket\n");
        return -1;
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(proxport);
    unsigned char* ipaddr = reinterpret_cast<unsigned char*>(&addr.sin_addr);
    ipaddr[0] = 127;
    ipaddr[1] = 0;
    ipaddr[2] = 0;
    ipaddr[3] = 1;
    if(0 != connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)))
    {
      printf("fail to connect to proxy\n");
      return -1;
    }
    sock_alive = true;
    string command;
    while(1)
    {
      
        std::cout << "\nCommand Options:\n"<<"* login <username>\n"<<"* balance\n"<<"* withdraw <amount>\n"<<"* transfer <amount> <recipient>\n"<<"* logout\n\r";
        std::cout << "\nATM $";
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
    }
    close(sock);
    return 0;
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
    cout << "Disconnected. Thank you" << endl;
    currentUser = "";
    loggedIn = false;
}

bool encryptAndSend(const string & msg) 
{
  Packet packet;
  string encrypted_msg = crypto.RSAEncryption(msg);
  cout<<"error_check1"<<endl;
  string signed_msg = crypto.SignatureSign(encrypted_msg);
  cout<<"error_check2"<<endl;
  for (int i = 0; i<signed_msg.length(); i++) {
    packet.push_back(signed_msg[i]);
  }
  cout<<"error_check3"<<endl;
  if (PACKET_SIZE != send(sock, &packet[0], PACKET_SIZE, 0))
  {
    printf("failed to send packet\n");
    return false;
  }
  cout<<"error_check4"<<endl;
  return true;
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

string receiveAndDecrypt() {
  Packet packet;
  packet.resize(256);
  if(PACKET_SIZE != recv(sock, &packet[0], PACKET_SIZE, 0))
  {
      printf("fail to read packet\n");
      return "";
  }
  std::string resp;
  resp.append(packet.begin(),packet.end());
  string verified_msg = crypto.SignatureVerify(resp);
  string decrypted_msg = crypto.RSADecryption(verified_msg);
  return decrypted_msg;
}

