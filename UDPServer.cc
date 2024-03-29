#include "UDPServer.h"

#include <iostream>

#include "ServerApplication.h"
#include "tp2_constants.h"

using namespace std;
using namespace TP2;


void UDPServer::setUp(const string& port) {

    //helper variable to check for errors
    int returnCode = -1; 
    if (LOGGING) {
        cout << "Binding server..." << endl;
    }
    
    _serverSocketId = socket(AF_INET6, SOCK_DGRAM, 0);
    
    if (_serverSocketId < 0) {
        cout << "ERROR setting up socket" << endl;
        exit(1);
    }
    
    //Initializing socket structure
    //in6addr_any allows connection from both ipv6 and ipv4
    bzero(&_serverSocket, sizeof(_serverSocket));
    _serverSocket.sin6_family = AF_INET6;
    _serverSocket.sin6_port = htons(stoi(port));
    _serverSocket.sin6_addr = in6addr_any;

    _clientAddressLength = sizeof(_clientAddressStorage);

    returnCode = bind(_serverSocketId, (sockaddr*) &_serverSocket, sizeof(_serverSocket));
    if (returnCode < 0 ) {
        cout << "ERROR binding server socket." << endl;
        cout << "Maybe try a different port?" << endl;
        exit(1);
    }

    //No need to listen to anything because ... THIS IS SPARTAAAAAA (I mean UDP...)
    //listen(_serverSocketId, MAX_NUM_CONNECTIONS );
}

void UDPServer::closeConnection() {
    if (LOGGING) {
        cout << "Closing connection to client..." << endl;
    }

    //TODO - remove client from _clientList (in the future)
    
    sendMessageToClient("-1");

    if (LOGGING) {
        cout << "Client is out" << endl;
    }
}


string UDPServer::getMessageFromClient() {
    if (LOGGING) {
        cout << "Waiting for client message" << endl;
    }

    char msg[MAX_BUF];
    
    recvfrom(_serverSocketId,msg,MAX_BUF, 0, (sockaddr*)&_clientAddressStorage, &_clientAddressLength);

    if (LOGGING) {
        cout << "Got client message: " << msg << endl;
    }
   
    return string(msg);

}


void UDPServer::sendMessageToClient(const string& message) {
    if (LOGGING) {
        cout << "Sending message to client: " << message << "." << endl;
    }

   if (sendto(_serverSocketId, message.c_str(), message.size()+1, 0,(sockaddr*)&_clientAddressStorage,_clientAddressLength) < 0) {
        cout << "I can't send message back to client. I give up!" << endl;
        exit(1);
   } 
}
