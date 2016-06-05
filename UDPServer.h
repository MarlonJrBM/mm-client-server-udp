#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>



namespace TP2{

/**
* UDP Server counterpart to the Olympic Protocol problem. 
* This server sends back the position performed by athletes (client)
* in a UDP manner. It implements a stop-and-wait sliding window to maintain
* communication reliability. Accepts ONE client at a time.
*/
class UDPServer {
    
    public:
       
        /**
        * Gets server's singleton instance.
        * Instantiates server in a lazy manner if needed.
        * @return pointer to server instance.
        */
        static UDPServer* getInstance() {
            if (_serverInstance == NULL) {
                _serverInstance = new UDPServer();
            }
            return _serverInstance;
        }

        /**
        * Deletes server's singleton and cleans its internal structures if needed.
        */ 
        static void deleteInstance() {
            if (_serverInstance->_serverSocketId != -1) {
                close(_serverInstance->_serverSocketId);
            }
            
            if (_serverInstance != NULL) {
                delete _serverInstance;
            }
        }

        /**
        * Binds UDPServer to specified port.
        * @param port
        */
        void setUp(const std::string& port);

        void run(); ///< UNIMPLEMENTED

        void stop(); ///< UNIMPLEMENTED
        
        int acceptClient(); ///< UNIMPLEMENTED

        /**
        * Sends message to client.
        * Client is automatically stored in _clientAddressStorage once a new message is received.
        * @param message
        */
        void sendMessageToClient(const std::string& message);

        /**
        * Obtains string from a UDP packet in the network.
        * Accepts a UDP packet from anyone and stores the sender's address in _clientAddressStorage.
        * @return client message.
        */ 
        std::string getMessageFromClient();

        /**
        * Sends closing signal to client (-1).
        */
        void closeConnection();

    
    protected:
        static UDPServer* _serverInstance; ///< server's singleton instance.
        UDPServer() {}; ///< private constructor since this is a singleton class.
        sockaddr_in6 _serverSocket; ///< strucutre which holds server socket data.
        sockaddr_storage _clientAddressStorage; ///< holds address of last sender.
        socklen_t _clientAddressLength; ///< helper variable for client address.
        int _serverSocketId; ///< ID of the socket used to communicate with the client.       

};

} //namespace

#endif

