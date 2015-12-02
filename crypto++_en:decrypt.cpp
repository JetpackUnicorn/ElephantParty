#include <cryptopp/base64.h>
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

void RSA_Encryption(SecByteBlock & sbbCipherText){
  //Encryption
  
  //Load public key
  RSAES_OAEP_SHA_Encryptor pubkey(FileSource("pubkey.txt", true, new Base64Decoder)));
  
  
  AutoSeededRandomPool rng;
  pubkey.Encrypt(
                 rng,
                 (byte const*) strShortString.data(),
                 strShortString.size(),
                 sbbCipherText.Begin());
  
  //FileSink("encrypted.dat").Put(sbbCipherText.Begin(), sbbCipherText.Size());

}

//********************************************************************************

void RSA_Decryption(const SecByteBlock & sbbCipherText){
  //Decryption
  
  //Load private key
  RSAES_OAEP_SHA_Decryptor privkey(FileSource("privkey.txt", true, new Base64Decoder)));
  
  std::string plaintext;
  

  AutoSeededRandomPool rng;
  /*
  privkey.Decrypt(
                 rng,
                 (byte const*) sbbCipherText.data(),
                 sbbCipherText.size(),
                 sbbCipherText.Begin());
   */
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
 
  
  string strShortString =
  "Alice: Withdraw balance 50";
  // Cannot use std::string for buffer;
  // its internal storage might not be contiguous
  SecByteBlock sbbCipherText(pubkey.CipherTextLength(strShortString.size()));
  
}




