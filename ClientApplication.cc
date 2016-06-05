#include "ClientApplication.h"

#include <iostream>
#include <cstdio>
#include <string>
#include <regex>

#include "tp2_constants.h"

using namespace std;
using namespace TP2;


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
    _numTimeouts = 0;

	if (LOGGING) cout << "Starting Client Application" << endl;
    if (LOGGING) cout << "Number of arguments: " << argc << endl;
    
	connectionFlag = _mediator.setUpSocket(addr.c_str(), stoul(port));
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

		//Appends sequence number
		//If client is closing the connection, sequence number is always 0
		if (convertedTime < 0) seqNum = 0;
		insertSeqNum(seqNum, clientMessage);

		//sends converted time to server
		_mediator.sendRequest(clientMessage);

		//Keep on sending this message if a TIMEOUT occurs
		while ((serverResponse =_mediator.getResponse(TIMEOUT)) == "TIMEOUT" ) {
			if (LOGGING) {
				cout << "TIMEOUT! Resending message..." << endl;
			}
			if (++_numTimeouts == MAX_NUM_TIMEOUTS) {
				cout << "Giving up after " << MAX_NUM_TIMEOUTS << " attempts." << endl;
				exit(1);
			}
			_mediator.sendRequest(clientMessage);
		}

		displayResponse(serverResponse);

		//toggles seqNum
		seqNum = !seqNum;

	} while (convertedTime >= 0);

	_mediator.cleanUp();

	return 0;
}

void ClientApplication::displayResponse(const string& message) {

    if (stoll(message) >= 0) {
	    cout << message << endl;
    } else {
        //Server is ending connection
        if (LOGGING) {
            cout << "Exiting client" << endl;
        }
    }
}

string ClientApplication::getClientInput() {
	string clientMessage;
	if (LOGGING) {
		cout << "Please type your time" << endl;
	}
	getline(cin, clientMessage);
	return clientMessage;
}

/******************************************************************************
* A valid message is a message which starts with a digit
*******************************************************************************/
bool ClientApplication::isValidMessage(const string& message) {
	return strtoll(message.c_str(), NULL, 10);
}

/******************************************************************************
* Modifies message inserting seqNum into it.
*******************************************************************************/
void ClientApplication::insertSeqNum(int seqNum, string& message) {
	message = "[" + to_string(seqNum) + "]" + message;

	if (LOGGING) {
		cout << "Message with seqNum is: " << message << endl;
	}
}