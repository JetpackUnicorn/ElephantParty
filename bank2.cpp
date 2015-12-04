#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <iomanip>

#include <sstream>

#include <sys/types.h>  
#include "crypto.h"
#include "bank.h"

void* client_thread(void* arg);
void* console_thread(void* arg);
InvertibleRSAFunction privkey;

Bank BANK;
map< long long int, int > acctpins;


vector <string> splitStringByWhitespace(string inString, int limit)
{
    string buffer;
    stringstream ss(inString);
    vector <string> tokens;
    while (ss >> buffer) {
        if (tokens.size() > limit) { break; }
        tokens.push_back(buffer);
    }
    return tokens;
}

string formatAmount(float amt)
{
    stringstream ss;
    ss << fixed << setprecision(2) << amt;
    return ss.str();
}

bool authenticate( long long int acctnum, int pin ) {

    if( acctpins.find(acctnum) == acctpins.end() ) return false;    // account does not exist

    return (acctpins[acctnum] == pin);
}
//********************************************************************************
string RSA_Encryption(const string & plain){
  //Encryption
  AutoSeededRandomPool rng;
  
  //Load public key
  CryptoPP::RSA::PublicKey pubKey;
  CryptoPP::ByteQueue bytes;
  FileSource file("pubkey_atm.txt", true, new Base64Decoder);
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
  string recovered;
  
  RSAES_OAEP_SHA_Decryptor d(privkey);
  
  StringSource ss2(cipher, true,
                   new PK_DecryptorFilter(rng, d,
                                          new StringSink(recovered)
                                          ) // PK_DecryptorFilter
                   ); // StringSource
  
  //assert (plain == recovered);
  std::cout << "decrypted plain: " << recovered << std::endl;
  return recovered;
}
//********************************************************************************
string signature_sign(const string & msg){
  
  // Setup
  string message = msg;
  cout << "unsigned message: "<< msg <<endl;
  AutoSeededRandomPool rng;
  
  // Sign and Encode
  RSASS<PSSR, SHA1>::Signer signer(privkey);
  
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
//********************************************************************************
string signature_verify(const string & signature){
  
  RSA::PublicKey publicKey;
  string recovered, message;
  
  // Load public key
  CryptoPP::ByteQueue bytes;
  FileSource file("pubkey_atm.txt", true, new Base64Decoder);
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
void encryptAndSend(Packet & packet, const string & msg) {
  
  // encryption
  string encrypted_msg = RSA_Encryption(msg);
  // sign
  string signed_msg = signature_sign(encrypted_msg);
  cout<<"signed msg length: "<<signed_msg.length()<<endl;
  
  for (int i = 0; i<signed_msg.length(); i++) {
    packet.push_back(signed_msg[i]);
  }
  string tmp;
  tmp.append(packet.begin(),packet.end());
  assert(tmp == signed_msg);
  cout<<"message check passed\n"<<endl;
}
void receiveAndDecrypt(const Packet & packet, string & msg) {
  
  std::string resp;
  resp.append(packet.cbegin(),packet.cend());
  // verify
  string verified_msg = signature_verify(resp);
  // decryption
  string decrypted_msg = RSA_Decryption(verified_msg);
  cout<<"the message sucessfully decrypted!"<<endl;
  msg = decrypted_msg;
  cout<<"decrypted_msg: "<<msg<<endl;
}
//********************************************************************************
void SharedKey_Init(){
  
  AutoSeededRandomPool rng;
  privkey.Initialize(rng, 1024);
  
  RSAFunction pubkey(privkey);
  Base64Encoder pubkeysink(new FileSink("pubkey_bank.txt"));
  pubkey.DEREncode(pubkeysink);
  pubkeysink.MessageEnd();
  
}

bool readPins() {
    ifstream pinstr;
    pinstr.open("pins");
    if(pinstr.good()) {
        string line;
        for(int i=0; i<3; ++i) {
            getline(pinstr, line);
            vector<string> tokens = splitStringByWhitespace(line, 2);
            acctpins[stoull(tokens[0])] = stoi(tokens[1]);
        }
    } else { return false; }
    return true;
}

void initializeCards() {

    string names[3] = {"Alice", "Bob", "Eve"};

    ofstream pinstr;
    pinstr.open("pins");

    ifstream instr;
    for(int i=0; i<3; ++i) {
        instr.open(names[i]+".card");
        if(!instr.good()) {
            ofstream ostr;
            ostr.open(names[i]+".card");
            int numL = rand() % 90000000 + 10000000;
            int numR = rand() % 90000000 + 10000000;
            ostr << numL << numR << "\n\r" << names[i];
            ostr.close();

            long long int acctnum = stoull(to_string(numL)+to_string(numR));
            int randpin = rand() % 900000 + 100000;
            acctpins[acctnum] = randpin;
            pinstr << acctnum << " " << randpin << endl;
        }
        instr.close();
    }
    pinstr.close();
}



int main(int argc, char* argv[])
{
  if(argc != 2)
  {
    printf("Usage: bank listen-port\n");
    return -1;
  }
  
  unsigned short ourport = atoi(argv[1]);
  
  //crypto setup
  SharedKey_Init();
  

  //cards setup
  if(!readPins())
        initializeCards();

  //mutex setup
  //pthread_mutex_init(&EVIL_GLOBAL_STATE_MUTEX, NULL);
  
  //socket setup
  int lsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  int opt = 1;
  setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
  if(!lsock)
  {
    printf("fail to create socket\n");
    return -1;
  }
  
  //listening address
  sockaddr_in addr_l;
  addr_l.sin_family = AF_INET;
  addr_l.sin_port = htons(ourport);
  unsigned char* ipaddr = reinterpret_cast<unsigned char*>(&addr_l.sin_addr);
  ipaddr[0] = 127;
  ipaddr[1] = 0;
  ipaddr[2] = 0;
  ipaddr[3] = 1;
  if(0 != bind(lsock, reinterpret_cast<sockaddr*>(&addr_l), sizeof(addr_l)))
  {
    printf("failed to bind socket\n");
    return -1;
  }
  if(0 != listen(lsock, SOMAXCONN))
  {
    printf("failed to listen on socket\n");
    return -1;
  }
  
  pthread_t cthread;
  pthread_create(&cthread, NULL, console_thread, NULL);
  
  //loop forever accepting new connections
  while(1)
  {
    sockaddr_in unused;
    socklen_t size = sizeof(unused);
    int csock = accept(lsock, reinterpret_cast<sockaddr*>(&unused), &size);
    if(csock < 0)	//bad client, skip it
      continue;
    
    pthread_t thread;
    pthread_create(&thread, NULL, client_thread, (void*)csock);
  }
}

void* console_thread(void* arg)
{
    cout << "Welcome to the bank. Available commands:\n"
            << "deposit [username] [amount]\n"
            << "balance [username]\n";
    while(1)
    {
        cout << "BANK $";
        string input;
        getline(cin, input);
        vector <string> tokens = splitStringByWhitespace(input, 3);
        if (tokens.size() == 0) { continue; }
        if (tokens[0] == "deposit")
        {
            bool success;
            try { success = BANK.deposit(tokens[1], stof(tokens[2])); }
            catch(const invalid_argument& ia) { success = false; }
            if (success == false) { cerr << "Error on deposit.\n"; }
        }
        else if (tokens[0] == "balance")
        {
            float amount = BANK.checkBalance(tokens[1]);
            if (amount >= 0) { cout << "$" << formatAmount(amount) << "\n"; }
            else if (amount < 0) { cerr << "Error on check balance.\n"; }
        }
        else { cout << "Error: invalid command.\n"; }
    }
}


void* client_thread(void* arg)
{
  int csock = (int)(size_t)arg;
  string decrypted_msg, encrypted_msg;
  
  printf("[bank] client ID #%d connected\n", csock);
  
  //input loop
  Packet packet;
  packet.resize(PACKET_SIZE);
  //std::vector<char> *packet_begin = & packet[0];
  
  while(1)
  {
    //read the packet from the ATM
    if(PACKET_SIZE != recv(csock, & packet[0], PACKET_SIZE, 0))
    {
      printf("[bank] fail to read packet\n");
      break;
    }
    //decrypt message here
    receiveAndDecrypt(packet, decrypted_msg);

    string resp;
    
    vector <string> tokens = splitStringByWhitespace(decrypted_msg, 3);
    if (tokens.size() == 0) { continue; }
    resp = "FAILURE";
    if (tokens[0] == "balance")
    {
        float success = BANK.checkBalance(tokens[1]);
        if (success >= 0.0) { resp = "Your balance is $"+formatAmount(success)+"."; }
    }
    else if (tokens[0] == "withdraw")
    {
        float success = BANK.withdraw(tokens[1], stof(tokens[2]));
        if (success == true) { resp = "$"+formatAmount(stof(tokens[2]))+" withdrawn."; }
    }
    else if (tokens[0] == "transfer")
    {
        float success = BANK.transfer(tokens[1], stof(tokens[2]), tokens[3]);
        if (success == true) { resp = "$"+formatAmount(stof(tokens[2]))+" transferred to "+tokens[3]+"."; }
    }
    else if (tokens[0] == "authenticate")
    {
        bool success = authenticate(stoull(tokens[1]), stoi(tokens[2]));
        if (success == true) { resp = "Authenticated."; }
    }
    else { resp = "FAILURE"; }
    




    //std::string msg = "logged in\n";
    
    Packet response;
    //response.resize(PACKET_SIZE);
    //std::vector<char> *response_begin = & response[0];
    
    encryptAndSend(response, resp);
    
    //send the new packet back to the client
    if(PACKET_SIZE != send(csock, & response[0], PACKET_SIZE, 0))
    {
      printf("[bank] fail to send packet\n");
      break;
    }
    
  }
  
  printf("[bank] client ID #%d disconnected\n", csock);
  
  close(csock);
  return NULL;
}
