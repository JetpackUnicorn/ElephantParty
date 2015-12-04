#include "crypto.h"

using namespace CryptoPP;
using std::string;

void Crypto::SharedKeyInit(const std::string & publicKeyFile)
{
  AutoSeededRandomPool rng;
  InvertibleRSAFunction privkey;
  privkey.Initialize(rng, 1024);
  PrivateKey = privkey;
  
  // Suppose we want to store the public key separately,
  // possibly because we will be sending the public key to a third party.
  RSAFunction pubkey(privkey);
  const char * publicKeyFileCString = publicKeyFile.c_str();
  Base64Encoder pubkeysink(new FileSink(publicKeyFileCString));
  pubkey.DEREncode(pubkeysink);
  pubkeysink.MessageEnd(); 
}

void Crypto::LoadOppositeKey(const std::string & oppositePublicKeyFile)
{
    ByteQueue bytes;
    const char * publicKeyFileCString = oppositePublicKeyFile.c_str();
    FileSource file(publicKeyFileCString, true, new Base64Decoder);
    file.TransferTo(bytes);
    bytes.MessageEnd();
    OppositePublicKey.Load(bytes);
}

string Crypto::RSAEncryption(const string & plainTxt)
{
    AutoSeededRandomPool rng;
    RSAES_OAEP_SHA_Encryptor e(OppositePublicKey);
    string cipherTxt;
    StringSource ss1(plainTxt, true, new PK_EncryptorFilter(rng, e, new StringSink(cipherTxt)));
    return cipherTxt;
}

string Crypto::RSADecryption(const string & cipherTxt)
{
    AutoSeededRandomPool rng;
    std::string plainTxt;
    RSAES_OAEP_SHA_Decryptor d(PrivateKey);
    StringSource ss2(cipherTxt, true, new PK_DecryptorFilter(rng, d, new StringSink(plainTxt)));
    return plainTxt;
}

std::string Crypto::SignatureSign(const string & msg)
{
    string message = msg;
    AutoSeededRandomPool rng;
    RSASS<PSSR, SHA1>::Signer signer(PrivateKey);
    std::string signature;
    StringSource(message, true, new SignerFilter(rng, signer, new StringSink(signature), true));
    return signature;
}

string Crypto::SignatureVerify(const string & signature){
    string recovered, message;
    //ByteQueue bytes;
    //OppositePublicKey.Load(bytes);
    RSASS<PSSR, SHA1>::Verifier verifier(OppositePublicKey);
    StringSource(signature, true,
               new SignatureVerificationFilter(
                                               verifier,
                                               new StringSink(recovered),
                                               SignatureVerificationFilter::THROW_EXCEPTION |
                                               SignatureVerificationFilter::PUT_MESSAGE
                                               ) // SignatureVerificationFilter
               ); // StringSource
    return recovered;
}

string Crypto::key_filenameGen(){
  
  CryptoPP::SecByteBlock seed(32 + 16);
  CryptoPP::OS_GenerateRandomBlock(false, seed, seed.size());
  CryptoPP::OFB_Mode<AES>::Encryption prng;
  prng.SetKeyWithIV(seed, 32, seed + 32, 16);
  CryptoPP::SecByteBlock t(16);
  prng.GenerateBlock(t, t.size());
  string s;
  CryptoPP::HexEncoder hex(new StringSink(s));
  hex.Put(t, t.size());
  hex.MessageEnd();
  return s;
}










