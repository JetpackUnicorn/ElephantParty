#include <cryptopp/base64.h>
#include <cryptopp/modes.h>
#include <cryptopp/integer.h>
#include <cryptopp/files.h>
using CryptoPP::Integer;
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
#include <cryptopp/filters.h>
using CryptoPP::SignerFilter;
using CryptoPP::SignatureVerificationFilter;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
#include <cryptopp/cryptlib.h>
using CryptoPP::Exception;
#include <cryptopp/sha.h>
using CryptoPP::SHA1;
#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::endl;
#include <fstream>
using CryptoPP::RSAES_OAEP_SHA_Encryptor;
using CryptoPP::RSAES_OAEP_SHA_Decryptor;
using CryptoPP::PK_EncryptorFilter;
using CryptoPP::PK_DecryptorFilter;
using CryptoPP::DecodingResult;
using CryptoPP::SHA1;
#include <cryptopp/SecBlock.h>
using CryptoPP::SecByteBlock;
using std::cerr;
#include <exception>
using std::exception;
#include <assert.h>

class crypto {
private:
public:
    cryto() {}
    void SaveKey(const RSA::PublicKey& PublicKey, const string& filename);
    void LoadKey(const string& filename, RSA::PublicKey& PublicKey);
    void PrintPrivateKey(const RSA::PrivateKey& key);
    void PrintPublicKey(const RSA::PublicKey& key);
    void signature_sign(const string & pbkey_file, const string & account_num, string & signature);
    void signature_verify(const string & pbkey_file, const string & account_num, string & signature);
    void rsa_decrypt();
    void rsa_encrypt();
};