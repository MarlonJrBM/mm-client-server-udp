#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "ClientInterface.h"


namespace TP1{

enum IP_TYPE { IPV4, IPV6};

typedef std::vector<Client> ClientList;

/*************************
 * Singleton Server class
 *************************/
class Server {
    
    public:
       
        //There can only be one server, which will be initialized on demand
        static Server* getInstance() {
            if (_serverInstance == NULL) {
                _serverInstance = new Server();
            }
            return _serverInstance;
        }

        static void deleteInstance() {
            if (_serverInstance->_serverSocketId != -1) {
                close(_serverInstance->_serverSocketId);
            }
            
            if (_serverInstance != NULL) {
                delete _serverInstance;
            }
        }

        void setUp(std::string port);

        void run(); //NOT IMPLEMENTED

        void stop(); //NOT IMPLEMENTED
        
        //Accepts a client, adds it to _clientList (TODO), and returns a reference to it
        int acceptClient();

        void sendMessageToClient(std::string);

        std::string getMessageFromClient();

        //Closes connection to client stored in _clientAddressStorage
        void closeConnection();

    
    private:
        static Server* _serverInstance;
        Server() {};
        ClientList _clientList;
        sockaddr_in6 _serverSocket;
        sockaddr_storage _clientAddressStorage;
        socklen_t _clientAddressLength;
        int _serverSocketId;
        unsigned _addrLen;



};

} //namespace

#endif

