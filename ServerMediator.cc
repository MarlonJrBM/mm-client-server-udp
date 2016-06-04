#include "ServerMediator.h"

#include <iostream>
#include <cstdio>

using namespace std;
using namespace TP1;

const unsigned int MAX_BUF = 101;

/*******************************************************
* Opens socket to server based on
* addr and port. This is a UDP connection.
* Returns the _serverSocketId that was setup.
*******************************************************/
int ServerMediator::setUpSocket(string addr, unsigned port) {
    if (_logging) {
        cout << "Setting binding to server up at: " << addr << ":" << port << endl;
    }
    //Variables used to determine address (IPv4 vs IPv6)
    int returnCode = -1; 
    in6_addr serverAddr;
    addrinfo hints;

    //Hints structure will hold information about address
    bzero(&hints, sizeof(hints));
    hints.ai_flags    = AI_NUMERICSERV; //Indicates that port is a numeric number
    hints.ai_family   = AF_UNSPEC; //Family will be determined below
    hints.ai_socktype = SOCK_DGRAM;  //UDP

    //Now let's try to figure out if it is IPv4 or IPv6
    //If below code doesn't get executed, this means we have
    //a name instead of a number (localhost, google.com, etc...)
    //The AI_NUMERICHOST flag prevents the name from being resolved,
    //in case we have our address is a number
    returnCode = inet_pton(AF_INET, addr.c_str(), &serverAddr);
    if (returnCode == 1) {  /* this is a valid IPv4 (number format) */
       hints.ai_family = AF_INET;
       hints.ai_flags |= AI_NUMERICHOST; 
    } else {
       returnCode = inet_pton(AF_INET6, addr.c_str(), &serverAddr);
       if (returnCode == 1) { /* this is a valid IPv6 (number format) */
          hints.ai_family = AF_INET6;
          hints.ai_flags |= AI_NUMERICHOST;
       }
    }

    returnCode = getaddrinfo(addr.c_str(), to_string(port).c_str() ,&hints, &_result);

    if (returnCode < 0 ) {
        cout << "ERROR when resolving server address" << endl;
        exit(1);
    }

    _serverSocketId = socket(_result->ai_family, _result->ai_socktype,_result->ai_protocol);

    return _serverSocketId;
}

/************************************
* Closes connection to server
* 
* 
************************************/
void ServerMediator::closeConnection() {
	if (_logging) {
		cout << "Closing connection (UDP-wise)" << endl;
	}

    if (_result != NULL) {
        freeaddrinfo(_result);
    }

    if (_serverSocketId != -1) {
        close(_serverSocketId);
    }
}


/************************************
* Gets position back from server 
* (as string). Wait at most timeout 
* seconds. Defaults to 0 (no timeout).
* If a timeout has occurred, will return
* the string "TIMEOUT"
************************************/
string ServerMediator::getResponse(unsigned timeout) {
	if (_logging) {
		cout << "Waiting response from server..." << endl;
	}

    //Sets timeout
    struct timeval tv;
    tv.tv_sec = timeout; tv.tv_usec = 0;
    if (setsockopt(_serverSocketId, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv,sizeof(struct timeval)) < 0) {
        cout << "WARNING: Could not set timeout!" << endl;
    }

    char msg[MAX_BUF];

    if (recvfrom(_serverSocketId, msg, MAX_BUF, 0, NULL, NULL) < 1) {
        return string("TIMEOUT");
    }

	return string(msg);
}

/************************************
* Sends client's time to server
* (as long long int)
*
************************************/
void ServerMediator::sendRequest(string clientTime) {
	if (_logging) {
		cout << "Sending request to server: " << clientTime << endl;
	}
   
    if (sendto(_serverSocketId,clientTime.c_str(), clientTime.size()+1,0,_result->ai_addr, _result->ai_addrlen) < 0) {
        cout << "Error sending request. I give up!" << endl;
        exit(1);
    };
}

