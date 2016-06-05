#include "ServerApplication.h"

#include <iostream>

#include "UDPServer.h"
#include "Ranking.h"
#include "tp2_constants.h"

using namespace std;
using namespace TP2;


UDPServer* UDPServer::_serverInstance = NULL;

int ServerApplication::runApplication(int argc, char** argv) {

    //Local and helper variables
    string clientMessage = "";
    string position = "";
    string port = argv[1];
    bool isClosingMessage = false;

    //Flags for STOP-AND-WAIT
    bool curSeqNum = 0;
    _expectedSeqNum = 0;
    _lastSentMessage = "";
    
    if (LOGGING) {
        cout << "Starting server!" << endl;
        cout << "Number of arguments: " << argc << endl;
        cout << "Press ctrl + c at anytime to exit" << endl;
    }

    UDPServer* server = UDPServer::getInstance();

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
            isClosingMessage = isClosingSignal(clientMessage);

            //Accepts closing message, regardless of sequence number
            if (isClosingMessage) {
                _timeRanking->clear();
                server->closeConnection();
                _lastSentMessage = "-1";

                //here we reset the sequence number because we might be getting a new client after this one
                _expectedSeqNum = 0; 
                continue;
            }

            //STOP-N-WAIT Protocol:
            //Let us check the sequence number!
            if (curSeqNum != _expectedSeqNum) {

                if (LOGGING) {
                    cout << "Sequence number did NOT match!" << endl;
                    cout << "Resending last message: " << _lastSentMessage << endl;
                }

                //Sequence number is not the expected one
                //Resends last message
                if (_lastSentMessage != "") {
                    server->sendMessageToClient(_lastSentMessage);
                }

            } else { /* sequence number is the one expected, proceed as usual*/

                if (LOGGING) {
                    cout << "Sequence number MATCH!" << endl;
                }

                //First, we'll check if client sends a valid message
                //This is just a basic server-side input validation
                if (!isValidMessage(clientMessage)) {
                    server->sendMessageToClient("0");
                    _lastSentMessage = "0";
                    isClosingMessage = false; //We'll keep taking requests from user

                    //toggling sequence Number
                    _expectedSeqNum = !_expectedSeqNum;
                    continue;
                }
                
                //Will handle user request, sending his/her
                //position back
                position = _timeRanking->insert(clientMessage);
                cout << position << endl;
                server->sendMessageToClient(position);
                _lastSentMessage = position;

                //toggling sequence Number
                _expectedSeqNum = !_expectedSeqNum;
               
           }


       } while (!isClosingMessage);

   }

    //Will never reach below lines in single-threaded version
   server->stop();

   return 0;

}

bool ServerApplication::isClosingSignal(const string& message) {
    int signal = stoi(message);

    if (signal < 0) 
        return true;
    else
        return false;
}

bool ServerApplication::isValidMessage(const string& message) {
    return strtoll(message.c_str(), NULL, 10);
}

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
        UDPServer::deleteInstance();
        if (!_timeRanking) delete(_timeRanking);

}

