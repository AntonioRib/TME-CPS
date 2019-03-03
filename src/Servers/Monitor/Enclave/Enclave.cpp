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
    ocall_print("ENCLAVE: Attesting");
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

int trustedAttestMinionReturn(int minionSocket, int messageLength){
    ocall_print("ENCLAVE: Attesting");
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
    int result = 0;
    if (strcmp(splittedQuote[0], MessagesSGX::QUOTE) == 0 && strcmp(splittedQuote[1], AttestationConstantsSGX::QUOTE) == 0) {
        char* approvedMessage;
        approvedMessage = concatCharVec(MessagesSGX::OK_APPROVED, "");
        ocall_socketSendBuffer(minionSocket, approvedMessage);
        if (debug){
            ocall_print("ENCLAVE: Wrote to Minion: ");
            ocall_print(approvedMessage);
        }
        result = 1;
        free(approvedMessage);
    } else {
        char* notAapprovedMessage;
        notAapprovedMessage = concatCharVec(MessagesSGX::NOT_APPROVED, "");
        ocall_socketSendBuffer(minionSocket, notAapprovedMessage);
        if (debug){
            ocall_print("ENCLAVE: Wrote to Minion: ");
            ocall_print(notAapprovedMessage);
        }
        result = 0;
        free(notAapprovedMessage);
    }
    free(attestationRequestString);
    free(splittedQuote);
    ocall_print("ENCLAVE: Attested");
    return result;
}

void trustedProcessAttestation(int clientSocket, char* result, size_t resultLength, char* nonce, int messageLength){
    ocall_print("ENCLAVE: Attesting");
    char* attestationRequestString;
    attestationRequestString = concatCharVec(MessagesSGX::QUOTE, " ");
    attestationRequestString = concatCharVec(attestationRequestString, AttestationConstantsSGX::QUOTE);
    ocall_socketSendBuffer(clientSocket, attestationRequestString);
    if (debug){
        ocall_print("ENCLAVE: Wrote to Server: ");
        ocall_print(attestationRequestString);
    }

    char buffer[messageLength];
    ocall_socketReceiveBuffer(clientSocket, buffer, messageLength);
    if (debug){
        ocall_print("ENCLAVE: Recieved from Server: ");
        ocall_print(buffer);
    }

    strlcpy(result, buffer, resultLength); 
    char** splittedQuote = splitString(buffer); 
    // char* result = "";
    if (strcmp(splittedQuote[0], MessagesSGX::NOT_APPROVED) == 0) {
        if (debug){
            ocall_print("ENCLAVE: Not approved ");
        }
        result = "";
        // resultLength = strlen(result);
    } else if(strcmp(splittedQuote[0], MessagesSGX::OK_APPROVED) == 0) {
        if (debug){
            ocall_print("ENCLAVE: Approved! ");
            ocall_print("ENCLAVE: result: ");
            ocall_print(result);
        }
    }

    free(attestationRequestString);
    free(splittedQuote);
    return;
}
