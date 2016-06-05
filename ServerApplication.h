#ifndef SERVER_APPLICATION_H
#define SERVER_APPLICATION_H

#include "Ranking.h"

namespace TP2 {

/**
* Encapsulates server's business logic
* This class is responsible for connecting the server-side concerns with the athtletes ranking concern.
*/
class ServerApplication {

public:
    /**
	* Entry point of the application.
	* This is the entry point of the application and contains its main loop.
	* Implements stop-and-wait protocol in order to communicate with client.
	* @param argc number of arguments.
	* @param argv command line arguments.
	* @return return code of the entire program
	*/
    int runApplication(int argc, char** argv);

    /**
    * Cleans up server singleton.
    */
    ~ServerApplication();

protected:
    Ranking* _timeRanking = NULL; ///< keeps storage of times sent by client (athletes).

    bool _expectedSeqNum; ///< currently expected sequence number. Can either be 0 or 1, since this follows a stop-and-wait mechanism.

    std::string _lastSentMessage;


    /**
    * Checks whether message is a closing signal.
    * A closing signal is a negative number which the client sends.
    * @param message client message.
    * @return true if it's closing signal.
    */
    bool isClosingSignal(const std::string& message);

    /**
    * Checks whether message is a valid one.
    * A valid message is a message which starts with a digit.
    * @param message client message.
    * @return true if it's valid message
    */
    bool isValidMessage(const std::string& message);

    /**
    * Extracts sequence number from message (modifying it) and returns the sequence number.
    * Sequence number is either 0 or 1, as this follows a stop-and-wait mechanism.
    * If message has no sequence number, it will return internal member _expectedSeqNum.
    * @param message message whose sequence number will be extracted.
    * @return extracted sequence number.
    */
    bool extractSeqNum(std::string& message);

	
};


} //namespace TP2

#endif
