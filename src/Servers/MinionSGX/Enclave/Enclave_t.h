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
int minionMonitorRequestTrustedProcessAttestation(int monitorSocket, int messageLength);
int minionTrustedProcessAttestation(int monitorSocket, int messageLength);
int minionAuditorRequestTrustedProcessAttestation(int monitorSocket, int messageLength);

sgx_status_t SGX_CDECL ocall_print(const char* str);
sgx_status_t SGX_CDECL ocall_socketSendBuffer(int monitorSocket, char* buffer);
sgx_status_t SGX_CDECL ocall_socketReceiveBuffer(int monitorSocket, char* buffer, size_t bufferLength);
sgx_status_t SGX_CDECL ocall_socketReadTPM(char* tpmOut, size_t tpmOutLength);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
