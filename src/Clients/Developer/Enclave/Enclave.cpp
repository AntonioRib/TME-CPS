#include "Enclave_t.h"
// #include <netdb.h>
// #include <netinet/in.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <sys/wait.h>  // for wait()
// #include <unistd.h>  // for fork()
// #include <iostream>
// #include <iterator>
// #include <sstream>
// #include <string>
// #include <vector>
// #include "../../../Utilities/AttestationConstants.h"
// #include "../../../Utilities/General.h"
// #include "../../../Utilities/Messages.h"
// #include "../../../Utilities/Ports.h"
// #include "../../../Utilities/TPM.h"

int generate_random_number() {
    ocall_print("Processing random number generation...");
    return 42;
}

void attestMonitor(int monitorSocket){
        ocall_print("Attested");
    // char buffer[SocketUtils::MESSAGE_BYTES];
    // bzero(buffer, SocketUtils::MESSAGE_BYTES);
    // string attestationRequestString = Messages::ATTEST + " " + AttestationConstants::NONCE;
    // General::stringToCharArray(attestationRequestString, buffer, SocketUtils::MESSAGE_BYTES);
    // SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
    // // if (DebugFlags::debugDeveloper)
    // //     ocal_print("Wrote" + buffer + "to server\n");
    //     // cout << "Wrote: " << buffer << " to server\n";

    // bzero(buffer, SocketUtils::MESSAGE_BYTES);
    // SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    // // if (DebugFlags::debugDeveloper)
    // //     ocal_print("Recieved from server: " + buffer + "\n");
    //     // cout << "Recieved from server: " << buffer << "\n";

    // string quote(buffer);
    // vector<string> splittedQuote = General::splitString(quote);

    // if (splittedQuote[0] == Messages::QUOTE && splittedQuote[1] == AttestationConstants::QUOTE && splittedQuote[2] == AttestationConstants::PCR_SHA1 && splittedQuote[3] == AttestationConstants::PCR_SHA1) {
    //     string approvedMessage = Messages::OK_APPROVED;
    //     General::stringToCharArray(approvedMessage, buffer, SocketUtils::MESSAGE_BYTES);
    //     SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
    //     // if (DebugFlags::debugDeveloper)
    //     //     ocal_print("Wrote: " + buffer + " to server\n");
    //         // cout << "Wrote: " << buffer << " to server\n";
    // } else {
    //     General::stringToCharArray(Messages::NOT_APPROVED, buffer, SocketUtils::MESSAGE_BYTES);
    //     SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
    //     // if (DebugFlags::debugDeveloper)
    //     //     ocal_print("Wrote: " + buffer + " to server\n");
    // }
}