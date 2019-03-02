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
    // char result[strlen(c1)+strlen(c2)+1];
    char* result = (char*) malloc(sizeof(char) * strlen(c1)+strlen(c2)+1); /* make space for the new string (should check the return value ...) */
    strlcpy(result, c1, strlen(c1)+1); 
    strncat(result, c2, strlen(c2)+1);
    return result;
}

char** splitString(char* str){
    ocall_print("Inside splitString");
    int count = 0;
    char* p = strtok (str," ");
    // char* pch[10];
    char** pch = (char**) malloc(sizeof(char*) * 10); 
    count++;
    while (p != NULL)
    {
        ocall_print("ENCLAVE: p: ");
        ocall_print(p);
        pch[count++] = p;
        p = strtok (NULL, " ");
    }
    ocall_print("Done");

            int i = 0;
            int size = (int)(sizeof(pch) / sizeof(pch[0]));
            for(i = 0; i < size; i++){
                ocall_print("ENCLAVE: splittedQuote: ");
                ocall_print(pch[i]);
                ocall_print("\n");
            }

    return pch;
}

#define MAX_BUF_LEN 256

bool debug = true;

void trustedAttestMonitor(int monitorSocket, int messageLength){
    ocall_print("Attesting");
    char* attestationRequestString;
    attestationRequestString = concatCharVec(MessagesSGX::ATTEST, " "); //MessagesSGX::ATTEST + " " + AttestationConstantsSGX::NONCE;
    if (debug){
        ocall_print("ENCLAVE: 1st attestationRequestString: ");
        ocall_print(attestationRequestString);
    }
    attestationRequestString = concatCharVec(attestationRequestString, AttestationConstantsSGX::NONCE);
    if (debug){
        ocall_print("ENCLAVE: 2nd attestationRequestString: ");
        ocall_print(attestationRequestString);
    }

    ocall_socketSendBuffer(monitorSocket, attestationRequestString);
    // debugStr = string("Wrote: ") + attestationRequestString + string(" to server\n")
    if (debug){
        ocall_print("ENCLAVE: Wrote to server: ");
        ocall_print(attestationRequestString);
        ocall_print("\n");
    }

    char buffer[messageLength];
    ocall_socketReceiveBuffer(monitorSocket, buffer, messageLength);
    // debugStr = string("Recieved from server: ") + buffer + string("\n");
    if (debug){
        ocall_print("ENCLAVE: Recieved from server: ");
        ocall_print(buffer);
        ocall_print("\n");
    }

    // char* quote = string(buffer);
    char** splittedQuote = splitString(buffer);
        if (debug){
            int i = 0;
            int size = (int)(sizeof(splittedQuote) / sizeof(splittedQuote[0]));
            for(i = 0; i < size; i++){
                ocall_print("ENCLAVE: splittedQuote: ");
                ocall_print(splittedQuote[i]);
                ocall_print("\n");
            }
        }
    // if (splittedQuote[0] == MessagesSGX::QUOTE && splittedQuote[1] == AttestationConstantsSGX::QUOTE && splittedQuote[2] == AttestationConstantsSGX::PCR_SHA1 && splittedQuote[3] == AttestationConstantsSGX::PCR_SHA1) {
    if (strcmp(splittedQuote[0], MessagesSGX::QUOTE) == 0 && strcmp(splittedQuote[1], AttestationConstantsSGX::QUOTE) == 0 
        && strcmp(splittedQuote[2], AttestationConstantsSGX::PCR_SHA1) == 0 && strcmp(splittedQuote[3], AttestationConstantsSGX::PCR_SHA1) == 0) {
        // char* approvedMessage = ;
        // General::stringToCharArray(approvedMessage, buffer, messageLength);
        char* approvedMessage;
        approvedMessage = concatCharVec(MessagesSGX::OK_APPROVED, "");
        ocall_socketSendBuffer(monitorSocket, approvedMessage);
        if (debug){
            ocall_print("ENCLAVE: Wrote to server: ");
            ocall_print(buffer);
            ocall_print("\n");
        }
    } else {
        // General::stringToCharArray(, buffer, messageLength);
        char* notAapprovedMessage;
        notAapprovedMessage = concatCharVec(MessagesSGX::NOT_APPROVED, "");
        ocall_socketSendBuffer(monitorSocket, notAapprovedMessage);
        if (debug){
            ocall_print("ENCLAVE: Wrote to server: ");
            ocall_print(buffer);
            ocall_print("\n");
        }
    }
    ocall_print("Attested");
}