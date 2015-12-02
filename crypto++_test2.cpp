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

void signature_sign(const string & account_num, const string & pvkey_file, string & signature){
  
  // Setup
  string message = account_num;
  RSA::PrivateKey privateKey;
  AutoSeededRandomPool rng;
  
  // Load key
  LoadKey(pvkey_file, privateKey);
  
  
  // Sign and Encode
  RSASS<PSSR, SHA1>::Signer signer(privateKey);
  
  // StringSource
  StringSource(message, true,
               new SignerFilter(rng, signer,
                                new StringSink(signature),
                                true // putMessage
                                ) // SignerFilter
               );
  
  
  // Save Public Key
  //string pbkey_file = "Eve_pbkey.DER";
  //SaveKey( publicKey, pbkey_file);
  
  // Save Private Key
  //string pvkey_file = "Eve_pvkey.DER";
  //SaveKey(privateKey, pvkey_file);
  
  /*
  // Save signature
  std::ofstream atmbank_sign;
  atmbank_sign.open ("Signature.sign");
  atmbank_sign << signature;
  
  cout<< "signature send :" << signature<<endl;
  atmbank_sign.close();
   
  */
}



void signature_verify(const string & account_num, const string & pbkey_file, const string & signature){
  
  RSA::PublicKey publicKey;
  string recovered, message;
  // Load key
  LoadKey(pbkey_file, publicKey);
  
  /*
  // Load signature
  std::ifstream atmbank_sign ("Signature.sign");
  atmbank_sign >> signature;
  atmbank_sign.close();
  
  cout<< "signature receive :" << signature<<endl;
   */
  
  
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
  
  string PrivateKey_DER;
  string PublicKey_DER;
  
  if( argc != 5) {
    std::cerr << "Usage1: <bank> <card> <public key> <verify>" << std::endl;
    std::cerr << "Usage2: <atm> <card> <private key> <sign>" << std::endl;
    exit(1);
  }
  if(argv[4] == "verify") {
    
    PublicKey_DER = argv[2];
    
  }
  else if (argv[4] == "sign"){
    PrivateKey_DER = argv[2];
  }
  
  string Card_file = argv[1], account_num;
  
  
  std::ifstream mycard;
  mycard.open (Card_file);
  mycard >> account_num;
  mycard.close();
  
  std::cout<<"account_num: "<<account_num<<std::endl;
  
  string signature;   // need to be transfered through proxy
  
  signature_sign(account_num, PrivateKey_DER, signature);
  signature_verify(account_num, PublicKey_DER, signature);
  //********************************************************************************
  
}
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
