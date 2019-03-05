#ifndef ENCLAVE_U_H__
#define ENCLAVE_U_H__

#include <stdint.h>
#include <wchar.h>
#include <stddef.h>
#include <string.h>
#include "sgx_edger8r.h" /* for sgx_satus_t etc. */


#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef OCALL_PRINT_DEFINED__
#define OCALL_PRINT_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_print, (const char* str));
#endif
#ifndef OCALL_SOCKETSENDBUFFER_DEFINED__
#define OCALL_SOCKETSENDBUFFER_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_socketSendBuffer, (int monitorSocket, char* buffer));
#endif
#ifndef OCALL_SOCKETRECEIVEBUFFER_DEFINED__
#define OCALL_SOCKETRECEIVEBUFFER_DEFINED__
void SGX_UBRIDGE(SGX_NOCONVENTION, ocall_socketReceiveBuffer, (int monitorSocket, char* buffer, size_t bufferLength));
#endif

sgx_status_t generate_random_number(sgx_enclave_id_t eid, int* retval);
sgx_status_t trustedAttestMinion(sgx_enclave_id_t eid, int minionSocket, int messageLength);
sgx_status_t trustedAttestMinionReturn(sgx_enclave_id_t eid, int* retval, int minionSocket, int messageLength);
sgx_status_t trustedProcessAttestation(sgx_enclave_id_t eid, int clientSocket, char* result, size_t resultLength, char* nonce, int messageLength);
sgx_status_t developerTrustedProcessAttestation(sgx_enclave_id_t eid, int* retval, int developerSocket, char* approvedSHA1, char* approvedConfiguration, int messageLength);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
