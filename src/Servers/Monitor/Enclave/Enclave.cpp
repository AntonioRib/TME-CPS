#include "Enclave_t.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../../../Utilities/SGX_Utils/AttestationConstantsSGX.h"
#include "../../../Utilities/SGX_Utils/MessagesSGX.h"
#include "../../../Utilities/SGX_Utils/PortsSGX.h"

int generate_random_number() {
    ocall_print("Processing random number generation...");
    return 42;
}

char* concatCharVec(const char* c1, const char* c2){
    char* result = (char*) malloc(sizeof(char) * strlen(c1)+strlen(c2)+1); /* make space for the new string (should check the return value ...) */
    strlcpy(result, c1, strlen(c1)+1); 
    strncat(result, c2, strlen(c2)+1);
    return result;
}

char** splitString(char* str){
    char* p = strtok(str," ");
    char** pch = (char**) malloc(sizeof(char*) * 10); 
    int count = 0;
    while (p != NULL)
    {
        pch[count++] = strndup(p, strlen(p)+1);
        p = strtok (NULL, " ");
    }
    return pch;
}

#define MAX_BUF_LEN 256

bool debug = true;

void trustedAttestMinion(int minionSocket, int messageLength){
    ocall_print("Attesting");
    char* attestationRequestString;
    attestationRequestString = concatCharVec(MessagesSGX::ATTEST, " ");
    attestationRequestString = concatCharVec(attestationRequestString, AttestationConstantsSGX::NONCE);
    ocall_socketSendBuffer(minionSocket, attestationRequestString);
    if (debug){
        ocall_print("ENCLAVE: Wrote to Minion: ");
        ocall_print(attestationRequestString);
    }

    char buffer[messageLength];
    ocall_socketReceiveBuffer(minionSocket, buffer, messageLength);
    if (debug){
        ocall_print("ENCLAVE: Recieved from Minion: ");
        ocall_print(buffer);
    }

    char** splittedQuote = splitString(buffer); 
    if (strcmp(splittedQuote[0], MessagesSGX::QUOTE) == 0 && strcmp(splittedQuote[1], AttestationConstantsSGX::QUOTE) == 0) {
        char* approvedMessage;
        approvedMessage = concatCharVec(MessagesSGX::OK_APPROVED, "");
        ocall_socketSendBuffer(minionSocket, approvedMessage);
        if (debug){
            ocall_print("ENCLAVE: Wrote to Minion: ");
            ocall_print(approvedMessage);
        }
        free(approvedMessage);
    } else {
        char* notAapprovedMessage;
        notAapprovedMessage = concatCharVec(MessagesSGX::NOT_APPROVED, "");
        ocall_socketSendBuffer(minionSocket, notAapprovedMessage);
        if (debug){
            ocall_print("ENCLAVE: Wrote to Minion: ");
            ocall_print(notAapprovedMessage);
        }
        free(notAapprovedMessage);
    }
    free(attestationRequestString);
    free(splittedQuote);
    ocall_print("ENCLAVE: Attested");
}

// void MinionRequestHandler::attestMinion(int minionSocket) {
//     char buffer[SocketUtils::MESSAGE_BYTES];
//     std::string request = Messages::ATTEST + " " + AttestationConstants::NONCE;
//     General::stringToCharArray(request, buffer, SocketUtils::MESSAGE_BYTES);
//     SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
//     if (DebugFlags::debugMonitor)
//         cout << "Wrote: " << buffer << " to Minion\n";

//     bzero(buffer, SocketUtils::MESSAGE_BYTES);
//     SocketUtils::receiveBuffer(minionSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
//     if (DebugFlags::debugMonitor)
//         cout << "Recieved: " << buffer << "\n";

//     string response(buffer);
//     vector<string> responseSplit = General::splitString(response);
//     if (responseSplit[0] == Messages::QUOTE && responseSplit[1] == AttestationConstants::QUOTE) {
//         std::string confirmation = Messages::OK;
//         General::stringToCharArray(confirmation, buffer, SocketUtils::MESSAGE_BYTES);
//         SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
//         if (DebugFlags::debugMonitor)
//             cout << "Wrote: " << buffer << " to Minion\n";
//     } else {
//         std::string confirmation = Messages::NOT_OK;
//         General::stringToCharArray(confirmation, buffer, SocketUtils::MESSAGE_BYTES);
//         SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
//         if (DebugFlags::debugMonitor)
//             cout << "Wrote: " << buffer << " to Minion\n";
//         cout << "Not approved!\n";
//     }
// }