#include <iostream>
#include <map>
#include <string>
<<<<<<< HEAD
=======

>>>>>>> master
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

<<<<<<< HEAD
using namespace std;

map <string, float> ACCOUNTS;
=======

using namespace std;

map <string, float> ACCOUNTS;
static const int BUFSIZE = 512;
>>>>>>> master

void initializeAccounts()
{
    ACCOUNTS["Alice"] = 100.0;
    ACCOUNTS["Bob"] = 50.0;
    ACCOUNTS["Eve"] = 0.0;
<<<<<<< HEAD
} 
=======
}

bool accountExists(string username)
{
    return ( ACCOUNTS.find(username) == ACCOUNTS.end() );
}
>>>>>>> master

void deposit(string username, float amount)
{
    ACCOUNTS[username] = ACCOUNTS[username] + amount;
}

void balance(string username)
{
    cout << username << ": " << ACCOUNTS[username] << "\n";
}

void printError(string errMsg)
{
    std::cerr << errMsg;
    exit(1);
}

int main(int argc, char * argv[]) 
{
    if (argc != 2) { printError("Usage: bank <port>\n"); }

    initializeAccounts();
    string terminal = "1. Deposit\n2. Check Balance\n3. Quit\n";
    
     int sockfd, newsockfd, portno;
     socklen_t clilen;
<<<<<<< HEAD
     char buffer[256];
=======
     char buffer[BUFSIZE];
>>>>>>> master
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        printError("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              printError("ERROR on binding");
     listen(sockfd,5);
<<<<<<< HEAD
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          printError("ERROR on accept");
     bzero(buffer,256);
     
    while(1)
    {
        n = write(newsockfd,"1. Deposit\n2. Check Balance\n3. Quit\n",36);
        if (n < 0) printError("ERROR writing to socket");   
        n = read(newsockfd, buffer, 1);
        if (n < 0) printError("ERROR reading from socket");
        if (strcmp(buffer, "1")) 
=======
     cout << "Bank initialized, listening on port " << portno << endl;

     clilen = sizeof(cli_addr);
     cout << "Looking for proxy..." << endl;
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          printError("ERROR on accept");
     else {
          cout << "Proxy found, connected" << endl;
     }
     bzero(buffer,BUFSIZE);
     
    ssize_t numbytes;
    while(1)
    {
        numbytes = recv(newsockfd, buffer, BUFSIZE, 0);
        string msg = buffer;
        cout << msg << endl;
        bzero(buffer, BUFSIZE);

        string resp = "ACK";
        numbytes = send(newsockfd, resp.c_str(), strlen(resp.c_str())+1, 0);

        /*n = write(newsockfd,"1. Deposit\n2. Check Balance\n3. Quit\n",terminal.size());
        if (n < 0) printError("ERROR writing to socket");   
        n = read(newsockfd, buffer, 1);
        if (n < 0) printError("ERROR reading from socket");
        if (buffer == "1") 
>>>>>>> master
        {
            n = write(newsockfd,"username:",9);
            if (n < 0) printError("ERROR writing to socket");  
            n = read(newsockfd,buffer,255); 
            if (n < 0) printError("ERROR reading from socket");
            string username = buffer;
            n = write(newsockfd,"amount:",9);
            if (n < 0) printError("ERROR writing to socket");  
            n = read(newsockfd,buffer,255); 
            if (n < 0) printError("ERROR reading from socket");
            int amount = atoi(buffer);
            deposit(username, amount);
        }
<<<<<<< HEAD
        if (strcmp(buffer,"2"))
=======
        if (buffer == "2")
>>>>>>> master
        {
            n = write(newsockfd,"username:",9);
            if (n < 0) printError("ERROR writing to socket");  
            n = read(newsockfd,buffer,255); 
            if (n < 0) printError("ERROR reading from socket");
            string username = buffer;
            balance(username);
        }

<<<<<<< HEAD
        if (strcmp(buffer,"3"))
=======
        if (buffer == "3")
>>>>>>> master
        {
            close(newsockfd);
            close(sockfd);
            break;
<<<<<<< HEAD
        }
=======
        }*/
>>>>>>> master
    }
    return 0; 
}