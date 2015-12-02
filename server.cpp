#include <iostream>    //cout
#include <stdio.h> //printf
#include <stdlib.h>
#include <string.h>    //strlen
#include <string>  //string
#include <sys/socket.h>    //socket
#include <sys/types.h>  
#include <arpa/inet.h> //inet_addr
#include <netinet/in.h>
#include <netdb.h> //hostent
#include <unistd.h> //close
#include <cstdlib> //exit
#include <pthread.h>
#include <sstream>
#include <vector>
#include <iomanip>

#include "bank.h"

using namespace std;
static const int BUFSIZE = 512;

Bank bank;

vector <string> splitStringByWhitespace(string inString, int limit)
{
    string buffer;
    stringstream ss(inString);
    vector <string> tokens;
    while (ss >> buffer) {
        if (tokens.size() > limit) { break; }
        tokens.push_back(buffer);
    }
    return tokens;
}

string formatAmount(float amt)
{
    stringstream ss;
    ss << fixed << setprecision(2) << amt;
    return ss.str();
}

void * cmdShellThreadRoutine(void * arg)
{
    cout << "Welcome to the bank. Available commands:\n"
            << "deposit [username] [amount]\n"
            << "balance [username]\n";
    while(1)
    {
        cout << "BANK $";
        string input;
        getline(cin, input);
        vector <string> tokens = splitStringByWhitespace(input, 3);
        if (tokens.size() == 0) { continue; }
        if (tokens[0] == "deposit")
        {
            bool success;
            try { success = bank.deposit(tokens[1], stof(tokens[2])); }
            catch(const invalid_argument& ia) { success = false; }
            if (success == false) { cerr << "Error on deposit.\n"; }
        }
        else if (tokens[0] == "balance")
        {
            float amount = bank.checkBalance(tokens[1]);
            if (amount >= 0) { cout << "$" << formatAmount(amount) << "\n"; }
            else if (amount < 0) { cerr << "Error on check balance.\n"; }
        }
        else { cout << "Error: invalid command.\n"; }
    }
}

void * cliThreadRoutine(void * arg)
{
    ssize_t numbytes;
    char buffer[BUFSIZE];
    while(1)
    {
        int * socket = (int *) arg; 
        string resp;
        numbytes = recv(*socket, buffer, BUFSIZE, 0);
        if (numbytes == 0) { break; }
        string msg = buffer;
        bzero(buffer, BUFSIZE);
        vector <string> tokens = splitStringByWhitespace(msg, 3);
        if (tokens.size() == 0) { continue; }
        resp = "FAILURE";
        if (tokens[0] == "balance")
        {
            float success = bank.checkBalance(tokens[1]);
            if (success >= 0.0) { resp = "Your balance is $"+formatAmount(success)+"."; }
        }
        else if (tokens[0] == "withdraw")
        {
            float success = bank.withdraw(tokens[1], stof(tokens[2]));
            if (success == true) { resp = "$"+formatAmount(stof(tokens[2]))+" withdrawn."; }
        }
        else if (tokens[0] == "transfer")
        {
            float success = bank.transfer(tokens[1], stof(tokens[2]), tokens[3]);
            if (success == true) { resp = "$"+formatAmount(stof(tokens[2]))+" transferred to "+tokens[3]+"."; }
        }
        else { resp = "FAILURE"; }
        numbytes = send(*socket, resp.c_str(), strlen(resp.c_str())+1, 0);
        if (numbytes == -1) { break; }
    }
}

int main(int argc, char * argv[])
{
    if (argc != 2) 
    { 
        cerr << "Usage: bank <port>\n";
        exit(1);
    }
    int sockfd, newsockfd, portno;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in addr_l;
    struct sockaddr_in serv_addr;
    if (sockfd < 0)
    {
        cerr << "Error creating socket\n";
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {    
        cerr << "Error on binding\n";
        exit(1);
    }
    listen(sockfd,5);
    cout << "Bank initialized, listening on port " << portno << endl;

    //Create a thread for the command shell
    pthread_t cmdShellThread;
    pthread_create(&cmdShellThread, NULL, cmdShellThreadRoutine, NULL);

    int numThreads;
    while(1)
    {
        sockaddr_in cliAddr;
        socklen_t cliLen = sizeof(cliAddr);
        int cliSock = accept(sockfd, (struct  sockaddr *) &cliAddr, &cliLen);
        if (cliSock < 0) { continue; }
        if (numThreads < 10)
        {
            numThreads++;
            pthread_t cliThread;
            pthread_create(&cliThread, NULL, cliThreadRoutine, (void*)(&cliSock));
        }
        else
        {
            cerr << "bank declined client: " << to_string(cliSock) << " too many connections\n";
        }            
    }
    return 0;
}




