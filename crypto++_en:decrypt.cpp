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






/*
void SavePublicKey(const string& filename, const RSA::PublicKey& key)
{
  ByteQueue queue;
  key.Save(queue);
  
  Save(filename, queue);
}

void SavePrivateKey(const string& filename, const RSA::PrivateKey& key)
{
  ByteQueue queue;
  key.Save(queue);
  
  Save(filename, queue);
}


void LoadPublicKey(const string& filename, RSA::PublicKey& key)
{
  ByteQueue queue;
  Load(filename, queue);
  
  key.Load(queue);
}

void LoadPrivateKey(const string& filename, RSA::PrivateKey& key)
{
  ByteQueue queue;
  Load(filename, queue);
  
  key.Load(queue);
}
*/
 
/*

void PrintPrivateKey(const RSA::PrivateKey& key)
{
  cout << "n: " << key.GetModulus() << endl;
  
  cout << "d: " << key.GetPrivateExponent() << endl;
  cout << "e: " << key.GetPublicExponent() << endl;
  
  cout << "p: " << key.GetPrime1() << endl;
  cout << "q: " << key.GetPrime2() << endl;
}

void PrintPublicKey(const RSA::PublicKey& key)
{
  cout << "n: " << key.GetModulus() << endl;
  cout << "e: " << key.GetPublicExponent() << endl;
}
*/

//********************************************************************************

void RSA_Encryption(CryptoPP::Integer & m, CryptoPP::Integer & c){
  //Encryption
  
  //Load public key
  CryptoPP::RSA::PublicKey pubKey;
  CryptoPP::ByteQueue bytes;
  FileSource file("pubkey.txt", true, new Base64Decoder);
  file.TransferTo(bytes);
  bytes.MessageEnd();
  pubKey.Load(bytes);
  

  //AutoSeededRandomPool rng;
  //Encryption
  
  //CryptoPP::Integer n("0xbeaadb3d839f3b5f"), e("0x11"), d("0x21a5ae37b9959db9");
  
  
  //pubKey.Initialize(n, e);
  
  //CryptoPP::Integer m, c;
  std::string message = "Withdraw 50";
  
  std::cout << "message: " << message << std::endl;
  
  // Treat the message as a big endian byte array
  m = CryptoPP::Integer((const byte *)message.data(), message.size());
  std::cout << "m: " << std::hex << m << std::endl;
  
  // Encrypt
  c = pubKey.ApplyFunction(m);
  std::cout << "c: " << std::hex << c << std::endl;
  //FileSink("encrypted.dat").Put(sbbCipherText.Begin(), sbbCipherText.Size());

}

//********************************************************************************

void RSA_Decryption(CryptoPP::Integer & c){
  //Decryption
  
  //Load private key
  CryptoPP::RSA::PrivateKey privKey;
  // Load private key
  CryptoPP::ByteQueue bytes;
  FileSource file("privkey.txt", true, new Base64Decoder);
  file.TransferTo(bytes);
  bytes.MessageEnd();
  privKey.Load(bytes);
  
  //CryptoPP::Integer n("0xbeaadb3d839f3b5f"), e("0x11"), d("0x21a5ae37b9959db9");
  //c = 0x3f47c32e8e17e291;
  //CryptoPP::Integer c(0x3f47c32e8e17e291), r;
  
  CryptoPP::AutoSeededRandomPool prng;
  CryptoPP::Integer r;
  std::string recovered;
  
  // Decrypt
  r = privKey.CalculateInverse(prng, c);
  std::cout << "r: " << std::hex << r << std::endl;
  
  // Round trip the message
  size_t req = r.MinEncodedSize();
  recovered.resize(req);
  r.Encode((byte *)recovered.data(), recovered.size());
  
  std::cout << "recovered: " << recovered << std::endl;}



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
  
  RSA_Encryption(m, c);
  RSA_Decryption(c);
  
  
}




