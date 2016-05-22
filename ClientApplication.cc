#include "ClientApplication.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <regex>

#include "StringManip.h"

using namespace std;
using namespace TP1;

/******************************************************************************
 * Client's application main loop. Messages are verified in a stop-and-wait
 * fashion.
 *******************************************************************************/
int ClientApplication::runApplication(int argc, char** argv) {
	string clientMessage = "";
	string serverResponse = "";
	long long convertedTime = 0;
    string addr = argv[1];
    string port = argv[2];
    int connectionFlag = -1;
    bool seqNum = 0;

	if (_logging) cout << "Starting Client Application" << endl;
    if (_logging) cout << "Number of arguments: " << argc << endl;
    
	connectionFlag = _mediator.setUpServerSocket(addr.c_str(), stoul(port));
    if (connectionFlag < 0 ) {
        cout << "ERROR! Could not open socket " << addr <<  ":" << port << endl;
        exit(1);
    }

	do {
		clientMessage = getClientInput();

		//This does basic client-side input verification
		//If user doesn't send a valid time, it won't be sent
		//to server
		if (isValidMessage(clientMessage)) { 
			convertedTime = stoll(clientMessage);
		} else {
			cout << "Invalid message!" << endl; 
			convertedTime = 0;
			continue;
		}

		//appends sequence number
		insertSeqNum(seqNum, clientMessage);

		//sends converted time to server
		_mediator.sendRequest(clientMessage);

		//gets answer from server and displays it
		serverResponse = _mediator.getResponse();
		displayResponse(serverResponse);

		//toggles seqNum
		seqNum = !seqNum;

	} while (convertedTime >= 0);

	_mediator.closeConnection();

	return 0;
}

void ClientApplication::displayResponse(string position) {

    if (stoll(position) >= 0) {
	    cout << position << endl;
    } else {
        //Server is ending connection
        if (_logging) {
            cout << "Exiting client" << endl;
        }
    }
}

string ClientApplication::getClientInput() {
	string clientMessage;
	if (_logging) {
		cout << "Please type your time" << endl;
	}
	getline(cin, clientMessage);
	return clientMessage;
}

/******************************************************************************
* A valid message is a message which starts with a digit
*******************************************************************************/
bool ClientApplication::isValidMessage(string message) {
	return strtoll(message.c_str(), NULL, 10);
}

/******************************************************************************
* Modifies message inserting seqNum into it.
*******************************************************************************/
void ClientApplication::insertSeqNum(int seqNum, string& message) {
	message = "[" + to_string(seqNum) + "]" + message;

	if (_logging) {
		cout << "Message with seqNum is: " << message << endl;
	}
}