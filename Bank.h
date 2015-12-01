#include <map>
#include <string>

using namespace std;

class Bank {

private:
    map <string, float> accounts;
    bool accountExists(string username);

public:
    Bank();
    bool deposit(string username, float amount);
    bool withdraw(string username, float amount);
    bool transfer(string usernameSrc, float amount, string usernameDest);
    float checkBalance(string username);
    
};