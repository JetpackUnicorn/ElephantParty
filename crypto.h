#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <string>
#include <cryptopp/base64.h>
#include <cryptopp/modes.h>
#include <cryptopp/integer.h>
#include <cryptopp/files.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pssr.h>
#include <cryptopp/rsa.h>
#include <cryptopp/filters.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>
#include<cryptopp/aes.h>
#include<cryptopp/hex.h>


#define PACKET_SIZE 256

typedef std::vector<char> Packet;



using namespace CryptoPP;
using std::string;

class Crypto {
private:
    RSA::PrivateKey PrivateKey;
    RSA::PublicKey PublicKey;
    RSA::PublicKey OppositePublicKey;   
public:
    Crypto(){};
    RSA::PrivateKey GetPrivateKey() { return PrivateKey; }
    RSA::PublicKey GetPublicKey() { return PublicKey; }
    RSA::PublicKey GetOppositePublicKey() { return OppositePublicKey; }
    void SharedKeyInit(const string & publicKeyFile);
    void LoadOppositeKey(const std::string & oppositePublicKeyFile);
    string RSAEncryption(const string & plainTxt);
    string RSADecryption(const string & cipherTxt);
    string SignatureSign(const string & msg);
    string SignatureVerify(const string & signature);
    string key_filenameGen();
};