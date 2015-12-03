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




string RSA_Encryption(const string & plain){
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
  
  //assert (plain == recovered);
  std::cout << "decrypted plain: " << recovered << std::endl;
  return recovered;
}

//signature
string signature_sign(const string & msg){
  
  // Setup
  string message = msg;
  cout << "unsigned message: "<< msg <<endl;
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
  
  
  cout << "Verified Message: " << "'" << recovered << "'" << endl;
  return recovered;
  
}

void SharedKey_Init(){
  
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
  
}


string encryptAndSend(string msg) {
  
  // encryption
  string encrypted_msg = RSA_Encryption(msg);
  // sign
  string signed_msg = signature_sign(encrypted_msg);
  
  return signed_msg;
}

string receiveAndDecrypt(string resp) {

  // verify
  string verified_msg = signature_verify(resp);
  // decryption
  string decrypted_msg = RSA_Decryption(verified_msg);
  
  return decrypted_msg;
}



 //********************************************************************************

int main( int argc , char *argv[]) {

  SharedKey_Init();
  
  string plain = "hello world_ alice";
  string resp = encryptAndSend(plain);
  string msg_recovered = receiveAndDecrypt(resp);
  
  
}




