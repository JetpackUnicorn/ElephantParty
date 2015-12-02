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
  //Authentication
  std::string msg("elephantparty");

  std::cout<<msg<<std::endl;
  
  // Pseudo Random Number Generator
  CryptoPP::AutoSeededRandomPool rng;

  // Key Generation (PRNG)
  CryptoPP::InvertibleRSAFunction keys;
  keys.GenerateRandomWithKeySize( rng, 384 );

/*
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
  
*/
  //********************************************************************************
  
  
  /*
  //********************************************************************************
  //Encryption
  
  CryptoPP::Integer n("0xbeaadb3d839f3b5f"), e("0x11"), d("0x21a5ae37b9959db9");
  
  CryptoPP::RSA::PublicKey pubKey;
  pubKey.Initialize(n, e);
  
  CryptoPP::Integer m, c;
  std::string message = "secret";
  
  std::cout << "message: " << message << std::endl;
  
  // Treat the message as a big endian byte array
  m = CryptoPP::Integer((const byte *)message.data(), message.size());
  std::cout << "m: " << std::hex << m << std::endl;
  
  // Encrypt
  c = pubKey.ApplyFunction(m);
  std::cout << "c: " << std::hex << c << std::endl;
  
  //********************************************************************************
  
  //********************************************************************************
  //Decryption
  
  //CryptoPP::Integer n("0xbeaadb3d839f3b5f"), e("0x11"), d("0x21a5ae37b9959db9");
  CryptoPP::AutoSeededRandomPool prng;
  
  CryptoPP::RSA::PrivateKey privKey;
  privKey.Initialize(n, e, d);
  
  //c = 0x3f47c32e8e17e291;
  //CryptoPP::Integer c(0x3f47c32e8e17e291), r;
  CryptoPP::Integer r;
  std::string recovered;
  
  // Decrypt
  r = privKey.CalculateInverse(prng, c);
  std::cout << "r: " << std::hex << r << std::endl;
  
  // Round trip the message
  size_t req = r.MinEncodedSize();
  recovered.resize(req);
  r.Encode((byte *)recovered.data(), recovered.size());
  
  std::cout << "recovered: " << recovered << std::endl;
  
  //********************************************************************************
  */
  
  //rsa_ende();
  

  return 0;
  
}



void rsa_ende()
{
  
  CryptoPP::AutoSeededRandomPool rng;
  
  // Create a private RSA key and write it to a file using DER.
  CryptoPP::RSAES_OAEP_SHA_Decryptor priv( rng, 4096 );
  CryptoPP::TransparentFilter privFile( new CryptoPP::FileSink("rsakey.der") );
  priv.DEREncode( privFile );
  privFile.MessageEnd();
  
  // Create a private RSA key and write it to a string using DER (also write to a file to check it with OpenSSL).
  std::string the_key;
  CryptoPP::RSAES_OAEP_SHA_Decryptor pri( rng, 2048 );
  CryptoPP::TransparentFilter privSink( new CryptoPP::StringSink(the_key) );
  pri.DEREncode( privSink );
  privSink.MessageEnd();
  
  std::ofstream file ( "key.der", std::ios::out | std::ios::binary );
  file.write( the_key.data(), the_key.size() );
  file.close();
  
  // Example Encryption & Decryption
  CryptoPP::InvertibleRSAFunction params;
  params.GenerateRandomWithKeySize( rng, 1536 );
  
  std::string plain = "RSA Encryption", cipher, decrypted_data;
  
  CryptoPP::RSA::PrivateKey privateKey( params );
  CryptoPP::RSA::PublicKey publicKey( params );
  
  CryptoPP::RSAES_OAEP_SHA_Encryptor e( publicKey );
  CryptoPP::StringSource( plain, true, new CryptoPP::PK_EncryptorFilter( rng, e, new CryptoPP::StringSink( cipher )));
  
  CryptoPP::RSAES_OAEP_SHA_Decryptor d( privateKey );
  CryptoPP::StringSource( cipher, true, new CryptoPP::PK_DecryptorFilter( rng, d, new CryptoPP::StringSink( decrypted_data )));
  
  assert( plain == decrypted_data );
  if (plain == decrypted_data) {
    std::cout<<"plain == decrypted_data"<<std::endl;
  }
}













