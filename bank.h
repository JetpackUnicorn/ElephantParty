#include <map>
#include <string>

using namespace std;

class Bank {

private:
    map <string, float> accounts;
    bool accountExists(string username){
    	if (accounts.find(username) == accounts.end()) {
      	return false;
    	}
    	return true;
		}	

public:
    Bank(){
    	accounts["Alice"] = 100.0;
    	accounts["Bob"] = 50.0;
    	accounts["Eve"] = 0.0;
		}
    bool deposit(string username, float amount)
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
    bool withdraw(string username, float amount)
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
    bool transfer(string usernameSrc, float amount, string usernameDest)
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
    float checkBalance(string username)
		{
	    if (accountExists(username)) {
	        return accounts[username];
	    }
	    return -1;
		}
    
};
