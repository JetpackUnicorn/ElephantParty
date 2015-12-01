#include <map>
#include <string>
#include "bank.h"

using namespace std;

Bank::Bank(){
    accounts["Alice"] = 100.0;
    accounts["Bob"] = 50.0;
    accounts["Eve"] = 0.0;
}

bool Bank::accountExists(string username)
{
    if (accounts.find(username) == accounts.end()) {
      return false;
    }
    return true;
}

bool Bank::deposit(string username, float amount)
{
    if (accountExists(username))
    {
        if (amount >= 0)
        {
            accounts[username] = accounts[username] + amount;
            return true;
        }
    }
    return false;
}

bool Bank::withdraw(string username, float amount)
{
    if (accountExists(username))
    {
        if (amount <= accounts[username] && amount >= 0)
        {
            accounts[username] = accounts[username] - amount;
            return true;
        }
    }
    return false;
}

bool Bank::transfer(string usernameSrc, float amount, string usernameDest)
{
    if (accountExists(usernameSrc) && accountExists(usernameDest))
    {
        if (amount <= accounts[usernameSrc] && amount >= 0)
        {
            accounts[usernameSrc] = accounts[usernameSrc] - amount;
            accounts[usernameDest] = accounts[usernameDest] + amount;
            return true;
        }
    }
    return false;
}

float Bank::checkBalance(string username)
{
    if (accountExists(username)) {
        return accounts[username];
    }
    return -1;
}