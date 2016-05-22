#include "ServerApplication.h"

#include <iostream>

#include "Server.h"
#include "Ranking.h"
#include "ClientInterface.h"

using namespace std;
using namespace TP1;

Server* Server::_serverInstance = NULL;

int ServerApplication::runApp(int argc, char** argv) {

    //Local and helper variables
    string clientMessage = "";
    string position = "";
    string port = argv[1];
    bool isValid = false, isClosingMessage = false;

    //Flags for STOP-AND-WAIT
    bool curSeqNum = 0;
    _expectedSeqNum = 0; 
    
    if (LOGGING) {
        cout << "Starting server!" << endl;
        cout << "Number of arguments: " << argc << endl;
        cout << "Press ctrl + c at anytime to exit" << endl;
    }

    Server* server = Server::getInstance();

    //Sets up server configuration and structures
    server->setUp(port);

    //initializes structure to handle race times
    _timeRanking = new Ranking();

    //Server's main loop
    while (true) {
        //TODO - Remove commented code below
        //Below calls are blocking, will wait until
        //Client connects and send their time in the race
        //curClient = server->acceptClient(); // NO NEED ANYMORE BECAUSE IT'S UDP

        do {
            clientMessage = server->getMessageFromClient();
            curSeqNum = extractSeqNum(clientMessage);
            isValid = isValidMessage(clientMessage);

            //First, we'll check if client sends a valid message
            if (!isValid) {
                server->sendMessageToClient("0");
                isClosingMessage = false; //We'll keep taking request from user
                continue;
            }

            //TODO - we check the sequence number

            //Checks if client wants to close connection
            isClosingMessage = isClosingSignal(clientMessage);
            if (isClosingMessage) {
                _timeRanking->clear();
                server->closeConnection();
             } else {

                //Will handle user request, sending his/her
                //position back
                 position = _timeRanking->insert(clientMessage);
                 cout << position << endl;
                 server->sendMessageToClient(position);
             }

        } while (!isClosingMessage);
    
    }
    
    //Will never reach below lines in single-threaded version
    server->stop();

    return 0;

}

/** 
* A closing signal is a negative number which the client sends 
**/
bool ServerApplication::isClosingSignal(string message) {
    int signal = stoi(message);

    if (signal < 0) 
        return true;
    else
        return false;
}

/**
* A valid message is a message which starts with a digit
**/
bool ServerApplication::isValidMessage(string message) {
    return strtoll(message.c_str(), NULL, 10);
}

/******************************************************************************
* Extracts sequence number from message. If no sequence number can be found,
* returns expectedSeqNum (what an optimistic server! =) )
******************************************************************************/
bool ServerApplication::extractSeqNum(string& message) {
    int firstBracket = message.find('[');
    int secondBracket = message.find(']');

    //If we don't find a square bracket in the message, this was sent
    //without a sequence number, so we just return the expected one.
    if (firstBracket == -1 || secondBracket == -1) {
        return _expectedSeqNum;
    }

    string seqNumStr = message.substr(firstBracket + 1,secondBracket - firstBracket);
    int seqNum = stoi(seqNumStr);

    //Removes sequence number tokens from message and returns sequence number;
    message = message.substr(secondBracket + 1, -1);
    if (LOGGING) {
        cout << "Message without sequence number: " << message << endl;
        cout << "Sequence number: " << seqNum << endl;
    }
    return seqNum;
}

ServerApplication::~ServerApplication() {
    Server::deleteInstance();
    if (!_timeRanking) delete(_timeRanking);
    
}

