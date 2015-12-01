#include <iostream>
#include <fstream>
#include <cryptopp/integer.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <cryptopp/sha.h>
#include <cryptopp/modes.h>

void rsa_ende();

//using namespace CryptoPP;



void authentication(){
  
  //********************************************************************************
  //Authentication
  std::string msg("elephantparty");
  
  std::cout<<msg<<std::endl;
  
  // Pseudo Random Number Generator
  CryptoPP::AutoSeededRandomPool rng;
  
  // Key Generation (PRNG)
  CryptoPP::InvertibleRSAFunction keys;
  keys.GenerateRandomWithKeySize( rng, 384 );
  
  // Signature
  CryptoPP::RSASS< CryptoPP::PKCS1v15, CryptoPP::SHA >::Signer signer( keys );
  
  // Set up for Signmessage()
  byte* signature = new byte[ signer.MaxSignatureLength() ];
  if( NULL == signature ) { return -1; }
  
  // Sign...
  size_t length = signer.SignMessage( rng, (const byte*) msg.c_str(),
                                     msg.length(), signature );
  
  // Verification
  CryptoPP::RSASS< CryptoPP::PKCS1v15, CryptoPP::SHA >::Verifier verifier( signer );
  
  bool result = verifier.VerifyMessage( (const byte*)msg.c_str(),
                                       msg.length(), signature, length );
  
  // Result
  if( true == result )  std::cout << "msg Verified" << std::endl;
  else  std::cout << "msg Verification Failed" << std::endl;
  
  if( NULL != signature ) { delete[] signature; }

  
}


int main( int argc , char *argv[]) {
  
  if( argc != 2 ) {
    std::cerr << "Usage: atm <card>" << std::endl;
    exit(1);
  }
  
  std::string Card_name = argv[1];
  int account_num;
  
  std::ifstream mycard;
  mycard.open (Card_name);
  mycard >> account_num;
  mycard.close();
  
  std::cout<<"account_num: "<<account_num<<std::endl;
  
  

  //********************************************************************************
