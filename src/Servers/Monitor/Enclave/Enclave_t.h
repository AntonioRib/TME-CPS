#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include "sgx_edger8r.h" /* for sgx_ocall etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

int generate_random_number(void);
void trustedAttestMinion(int minionSocket, int messageLength);
int trustedAttestMinionReturn(int minionSocket, int messageLength);
void trustedProcessAttestation(int clientSocket, char* result, size_t resultLength, char* nonce, int messageLength);

sgx_status_t SGX_CDECL ocall_print(const char* str);
sgx_status_t SGX_CDECL ocall_socketSendBuffer(int monitorSocket, char* buffer);
sgx_status_t SGX_CDECL ocall_socketReceiveBuffer(int monitorSocket, char* buffer, size_t bufferLength);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
