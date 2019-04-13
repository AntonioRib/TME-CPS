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

void trustedProcessAttestation(int clientSocket, char* result, size_t resultLength, char* nonce, int messageLength){
    ocall_print("ENCLAVE: Attesting");
            char* tpmOut;
        ocall_socketReadTPM(tpmOut, 0);
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


int sysAdminTrustedProcessAttestation(int adminSocket, char* approvedSHA1, char* approvedConfiguration, int messageLength){
    ocall_print("ENCLAVE: Attesting");
    char buffer[messageLength];
    ocall_socketReceiveBuffer(adminSocket, buffer, messageLength);
    if (debug){
        ocall_print("ENCLAVE: Recieved from SysAdmin: ");
        ocall_print(buffer);
    }

    // strlcpy(result, buffer, resultLength); 
    char** splittedRequest = splitString(buffer); 
    // char* result = "";
    if (strcmp(splittedRequest[0], MessagesSGX::ATTEST) == 0) {
        char* configurationString;
        configurationString = concatCharVec(MessagesSGX::QUOTE, " ");
        configurationString = concatCharVec(configurationString, AttestationConstantsSGX::QUOTE);
        configurationString = concatCharVec(configurationString, " ");
        configurationString = concatCharVec(configurationString, approvedSHA1);
        configurationString = concatCharVec(configurationString, " ");
        configurationString = concatCharVec(configurationString, approvedConfiguration);
    
        ocall_socketSendBuffer(adminSocket, configurationString);
        if (debug){
            ocall_print("ENCLAVE: Wrote to SysAdmin: ");
            ocall_print(configurationString);
        }
    } 
   
    // char recieveBuffer[messageLength];
    // ocall_socketReceiveBuffer(adminSocket, recieveBuffer, messageLength);
    // if (debug){
    //     ocall_print("ENCLAVE: Recieved from SysAdmin: ");
    //     ocall_print(recieveBuffer);
    // }

    // strlcpy(result, recieveBuffer, resultLength); 
    // char* result = "";
    int result = 1;
    // char** splittedResponse = splitString(recieveBuffer); 
    // if (strcmp(splittedResponse[0], MessagesSGX::NOT_APPROVED) == 0) {
    //     if (debug){
    //         ocall_print("ENCLAVE: Not approved ");
    //     }
    //     result = 0;
    // } else if(strcmp(splittedResponse[0], MessagesSGX::OK_APPROVED) == 0) {
    //     if (debug){
    //         ocall_print("ENCLAVE: Approved! ");
    //     }
    //     result = 1;
    // }
    // free(splittedResponse);
    return result;
}
