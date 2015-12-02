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

void SaveKey(const RSA::PublicKey& PublicKey, const string& filename)
{
  // DER Encode Key - X.509 key format
  PublicKey.Save(
                 FileSink(filename.c_str(), true /*binary*/).Ref()
                 );
}

void SaveKey(const RSA::PrivateKey& PrivateKey, const string& filename)
{
  // DER Encode Key - PKCS #8 key format
  PrivateKey.Save(
                  FileSink(filename.c_str(), true /*binary*/).Ref()
                  );
}

void LoadKey(const string& filename, RSA::PublicKey& PublicKey)
{
  // DER Encode Key - X.509 key format
  PublicKey.Load(
                 FileSource(filename.c_str(), true, NULL, true /*binary*/).Ref()
                 );
}

void LoadKey(const string& filename, RSA::PrivateKey& PrivateKey)
{
  // DER Encode Key - PKCS #8 key format
  PrivateKey.Load(
                  FileSource(filename.c_str(), true, NULL, true /*binary*/).Ref()
                  );
}

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

void signature_sign(const string & account_num, string & signature){
  
  // Setup
  string message = account_num;
  RSA::PrivateKey privateKey;
  AutoSeededRandomPool rng;
  
  // Load private key
  CryptoPP::ByteQueue bytes;
  FileSource file("privkey.txt", true, new Base64Decoder);
  file.TransferTo(bytes);
  bytes.MessageEnd();
  privateKey.Load(bytes);
  
  // Sign and Encode
  RSASS<PSSR, SHA1>::Signer signer(privateKey);
  
  // StringSource
  StringSource(message, true,
               new SignerFilter(rng, signer,
                                new StringSink(signature),
                                true // putMessage
                                ) // SignerFilter
               );
  
  /*
  string strContents = "A message to be signed";
  //FileSource("tobesigned.dat", true, new StringSink(strContents));
  
  AutoSeededRandomPool rng;
  
  //Read private key
  CryptoPP::ByteQueue bytes;
  FileSource file("privkey.txt", true, new Base64Decoder);
  file.TransferTo(bytes);
  bytes.MessageEnd();
  RSA::PrivateKey privateKey;
  privateKey.Load(bytes);
  
  //Sign message
  RSASSA_PKCS1v15_SHA_Signer privkey(privateKey);
  SecByteBlock sbbSignature(privkey.SignatureLength());
  privkey.SignMessage(
                      rng,
                      (byte const*) strContents.data(),
                      strContents.size(),
                      sbbSignature);
  
  //Save result
  FileSink sink("signed.dat");
  sink.Put((byte const*) strContents.data(), strContents.size());
  FileSink sinksig("sig.dat");
  sinksig.Put(sbbSignature, sbbSignature.size());
   
  */
}



void signature_verify( const string & account_num, string & signature){
  
  RSA::PublicKey publicKey;
  string recovered, message;
  
  // Load public key
  CryptoPP::ByteQueue bytes;
  FileSource file("pubkey.txt", true, new Base64Decoder);
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
  
  assert(account_num == recovered);
  cout << "Verified signature on message" << endl;
  cout << "Message: " << "'" << recovered << "'" << endl;
  
}




int main( int argc , char *argv[]) {
  
  if( argc != 2 ) {
    std::cerr << "Usage: atm <card>" << std::endl;
    exit(1);
  }
  
  std::string Card_name = argv[1];
  string account_num, account_name;

  std::ifstream mycard;
  mycard.open (Card_name.c_str());
  mycard >> account_num;
  mycard >> account_name;
  mycard.close();
  
  std::cout<<"account_num: "<<account_num<<std::endl;
  std::cout<<"account_name: "<<account_name<<std::endl;
  
  //generate initial key pair
  
  // InvertibleRSAFunction is used directly only because the private key
  // won't actually be used to perform any cryptographic operation;
  // otherwise, an appropriate typedef'ed type from rsa.h would have been used.
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
  
  
  string signature;
  // sign
  signature_sign(account_num, signature);
  // verify
  signature_verify(account_num, signature);
  //********************************************************************************
  
}




