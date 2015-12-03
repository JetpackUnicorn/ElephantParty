#include <iostream>    
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>  
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <pthread.h>
#include <sstream>
#include <vector>
#include <iomanip>
#include <fstream>
#include <time.h>
#include <map>

#include <cryptopp/aes.h>
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

#include "bank.h"

using namespace std;

static const int BUFSIZE = 512;
int NUMTHREADS;
Bank BANK;
map< long long int, int > acctpins;
InvertibleRSAFunction privkey;


//int generatePin(long long int acctnum);

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

//signature
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

void SharedKey_Init(){
  
  // InvertibleRSAFunction is used directly only because the private key
  // won't actually be used to perform any cryptographic operation;
  // otherwise, an appropriate typedef'ed type from rsa.h would have been used.
  AutoSeededRandomPool rng;
  privkey.Initialize(rng, 1024);
  
  // Suppose we want to store the public key separately,
  // possibly because we will be sending the public key to a third party.
  RSAFunction pubkey(privkey);
  Base64Encoder pubkeysink(new FileSink("pubkey_bank.txt"));
  pubkey.DEREncode(pubkeysink);
  pubkeysink.MessageEnd();
  
}

void * cmdShellThreadRoutine(void * arg)
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


void * cliThreadRoutine(void * arg)
{
    ssize_t numbytes;
    char buffer[BUFSIZE];
    while(1)
    {
        int * socket = (int *) arg; 
        string resp;
        numbytes = recv(*socket, buffer, BUFSIZE, 0);     // msg received
        if (numbytes == 0) { break; }
        string received_msg = buffer;
        //decrypt message
        // verify
        string verified_msg = signature_verify(received_msg);
        // decryption
        string decrypted_msg = RSA_Decryption(verified_msg);
    
        bzero(buffer, BUFSIZE);
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
      
        // encrypt resp
        string encrypted_msg = RSA_Encryption(resp);
        // sign
        string signed_msg = signature_sign(encrypted_msg);
      
        numbytes = send(*socket, signed_msg.c_str(), strlen(signed_msg.c_str())+1, 0);
        if (numbytes == -1) { break; }
    }
    NUMTHREADS--;
}

/*int generatePin(long long int acctnum) {
    
    string plaintext = to_string(acctnum);
    string ciphertext;

    CryptoPP::StreamTransformationFilter stfEncryptor(pinAlg, new CryptoPP::StringSink( ciphertext ) );
    stfEncryptor.Put( reinterpret_cast<const unsigned char*>( plaintext.c_str() ), plaintext.length() + 1 );
    stfEncryptor.MessageEnd();

    //Convert ciphertext to digits
    char temp[7];
    for(int i=0; i<6; ++i) {
        temp[i] = abs((int)ciphertext[i]) % 10 + 48;
    }
    temp[6]='\0';

    string tempstr = temp;
    int temppin = atoi(tempstr.c_str());

    return temppin;
}*/

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

int main(int argc, char * argv[])
{
    if (argc != 2) 
    { 
        cerr << "Usage: bank <port>\n";
        exit(1);
    }
  
    SharedKey_Init();
    srand( time(NULL) );

    if(!readPins())
        initializeCards();

    int sockfd, newsockfd, portno;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in addr_l;
    struct sockaddr_in serv_addr;
    if (sockfd < 0)
    {
        cerr << "Error creating socket\n";
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {    
        cerr << "Error on binding\n";
        exit(1);
    }
    listen(sockfd,5);
    cout << "Bank initialized, listening on port " << portno << endl;

    //Create a thread for the command shell
    pthread_t cmdShellThread;
    pthread_create(&cmdShellThread, NULL, cmdShellThreadRoutine, NULL);

    while(1)
    {
        sockaddr_in cliAddr;
        socklen_t cliLen = sizeof(cliAddr);
        int cliSock = accept(sockfd, (struct  sockaddr *) &cliAddr, &cliLen);
        if (cliSock < 0) { continue; }
        if (NUMTHREADS < 1)
        {
            NUMTHREADS++;
            pthread_t cliThread;
            pthread_create(&cliThread, NULL, cliThreadRoutine, (void*)(&cliSock));
        }
        else
        {
            cerr << "bank declined client: " << to_string(cliSock) << " too many connections\n";
        }            
    }
    return 0;
}




