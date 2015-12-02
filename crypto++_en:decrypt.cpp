#include <cryptopp/base64.h>
using CryptoPP::Base64Decoder;
using CryptoPP::Base64Encoder;
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
using CryptoPP::RSA;
using CryptoPP::InvertibleRSAFunction;
using CryptoPP::RSAES_OAEP_SHA_Encryptor;
using CryptoPP::RSAES_OAEP_SHA_Decryptor;
using CryptoPP::RSAFunction;
#include <cryptopp/filters.h>
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::PK_EncryptorFilter;
using CryptoPP::PK_DecryptorFilter;
#include <cryptopp/cryptlib.h>
using CryptoPP::Exception;
using CryptoPP::DecodingResult;
#include <cryptopp/sha.h>
using CryptoPP::SHA1;
#include <cryptopp/SecBlock.h>
using CryptoPP::SecByteBlock;
#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::endl;
using std::cerr;
#include <fstream>
#include <exception>
using std::exception;
#include <assert.h>
#include <cryptopp/queue.h>
using CryptoPP::ByteQueue;
#include <sstream>



string ToString (const CryptoPP::Integer & n){
  std::ostringstream os;
  os<< n;
  return os.str();
}



//********************************************************************************

void RSA_Encryption(const string & plain, string & cipher){
  //Encryption
  AutoSeededRandomPool rng;
  
  //Load public key
  CryptoPP::RSA::PublicKey pubKey;
  CryptoPP::ByteQueue bytes;
  FileSource file("pubkey.txt", true, new Base64Decoder);
  file.TransferTo(bytes);
  bytes.MessageEnd();
  pubKey.Load(bytes);
  
 
  RSAES_OAEP_SHA_Encryptor e(pubKey);
  
  StringSource ss1(plain, true,
                   new PK_EncryptorFilter(rng, e,
                                          new StringSink(cipher)
                                          ) // PK_EncryptorFilter
                   ); // StringSource
}

//********************************************************************************

void RSA_Decryption(const string & plain, string cipher){
  //Decryption
  AutoSeededRandomPool rng;
  //Load private key
  CryptoPP::RSA::PrivateKey privKey;
  // Load private key
  CryptoPP::ByteQueue bytes;
  FileSource file("privkey.txt", true, new Base64Decoder);
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
  
  assert (plain == recovered);
  std::cout << "recovered: " << recovered << std::endl;
}



 //********************************************************************************

int main( int argc , char *argv[]) {

  /*
  Integer n("0xbeaadb3d839f3b5f"), e("0x11"), d("0x21a5ae37b9959db9");
  
  RSA::PublicKey pubKey;
  pubKey.Initialize(n, e);
  */
   
  
  // Generate the keypair every time the server starts
  AutoSeededRandomPool rng;
  InvertibleRSAFunction privkey;
  privkey.Initialize(rng, 1024);
  
  // With the current version of Crypto++, MessageEnd() needs to be called
  // explicitly because Base64Encoder doesn't flush its buffer on destruction.
  Base64Encoder privkeysink(new FileSink("privkey.txt"));
  privkey.DEREncode(privkeysink);
  privkeysink.MessageEnd();
  
  // Suppose we want to store the public key separately,
  // possibly because we will be sending the public key to a third party.
  RSAFunction pubkey(privkey);
  
  Base64Encoder pubkeysink(new FileSink("pubkey.txt"));
  pubkey.DEREncode(pubkeysink);
  pubkeysink.MessageEnd();
  
  CryptoPP::Integer m, c;
  
  string plain = "hello world_ alice", cipher;
  RSA_Encryption(plain, cipher);
  RSA_Decryption(plain, cipher);
  
  
}




