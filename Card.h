#ifndef NAMECARD
#define NAMECARD


#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

using namespace std;  


class Namecard {
public:

  Namecard () {}

  bool readAcard(string name) {

    ifstream in_str;
    in_str.open(name.c_str());

    if (!in_str.good()) {
      std::cerr << "Can't open " << in_str << " to read.\n";
      return false;
    }
    
    string input_num;
    string input_name;

    getline (in_str,input_num);
    getline (in_str, input_name);

    //cardNum = stoi(input_num);
    cardNum = stoull(input_num.c_str());
    cardName = input_name;
    
    in_str.close();
    return true;
  }

  void setCardNum(long long int x) { cardNum = x; }
  void setCardName(string x) { cardName = x; }


  long long int getCardNum() {return cardNum;}
  string getCardName() {return cardName;}

private:
  long long int cardNum;
  string cardName; 
};

#endif
