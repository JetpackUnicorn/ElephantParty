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
#include <cstring>
#include <sstream>
#include <vector>

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
#include <cryptopp/aes.h>
#include <cryptopp/hex.h>


typedef std::vector<char> Packet;
#define PACKET_SIZE 256



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
