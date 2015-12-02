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

 //********************************************************************************

void rsa_encrypt(){
  
  // Generate keys
  AutoSeededRandomPool rng;
  
  InvertibleRSAFunction parameters;
  parameters.GenerateRandomWithKeySize( rng, 1024 );
  
  RSA::PrivateKey privateKey( parameters );
  RSA::PublicKey publicKey( parameters );
  
  string plain="RSA Encryption", cipher, recovered;
 
  // Encryption
  RSAES_OAEP_SHA_Encryptor e( publicKey );
  
  StringSource( plain, true,
               new PK_EncryptorFilter( rng, e,
                                      new StringSink( cipher )
                                      ) // PK_EncryptorFilter
               );
  
  
}



void rsa_decrypt(){
  
  // Decryption
  RSAES_OAEP_SHA_Decryptor d( privateKey );
  
  StringSource( cipher, true,
               new PK_DecryptorFilter( rng, d,
                                      new StringSink( recovered )
                                      ) // PK_EncryptorFilter
               ); // StringSource
  
  assert( plain == recovered );
}







 //********************************************************************************

int main( int argc , char *argv[]) {
  
  if( argc != 2 ) {
    std::cerr << "Usage: atm <card>" << std::endl;
    exit(1);
  }
  
  std::string Card_name = argv[1];
  string account_num, account_name;

  std::ifstream mycard;
  mycard.open (Card_name);
  mycard >> account_num;
  mycard >> account_name;
  mycard.close();
  
  std::cout<<"account_num: "<<account_num<<std::endl;
  std::cout<<"account_name: "<<account_name<<std::endl;
  
  
  string signature;
  string pbkey_DER = account_name + "_pbkey.DER";
  
  signature_sign(pbkey_DER, account_num, signature);
  signature_verify(pbkey_DER, account_num, signature);
 
  
}




