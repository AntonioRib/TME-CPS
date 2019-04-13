#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_generate_random_number_t {
	int ms_retval;
} ms_generate_random_number_t;

typedef struct ms_trustedAttestMinion_t {
	int ms_minionSocket;
	int ms_messageLength;
} ms_trustedAttestMinion_t;

typedef struct ms_trustedAttestMinionReturn_t {
	int ms_retval;
	int ms_minionSocket;
	int ms_messageLength;
} ms_trustedAttestMinionReturn_t;

typedef struct ms_trustedProcessAttestation_t {
	int ms_clientSocket;
	char* ms_result;
	size_t ms_resultLength;
	char* ms_nonce;
	size_t ms_nonce_len;
	int ms_messageLength;
} ms_trustedProcessAttestation_t;

typedef struct ms_developerTrustedProcessAttestation_t {
	int ms_retval;
	int ms_developerSocket;
	char* ms_approvedSHA1;
	size_t ms_approvedSHA1_len;
	char* ms_approvedConfiguration;
	size_t ms_approvedConfiguration_len;
	int ms_messageLength;
} ms_developerTrustedProcessAttestation_t;

typedef struct ms_ocall_print_t {
	const char* ms_str;
} ms_ocall_print_t;

typedef struct ms_ocall_socketSendBuffer_t {
	int ms_monitorSocket;
	char* ms_buffer;
} ms_ocall_socketSendBuffer_t;

typedef struct ms_ocall_socketReceiveBuffer_t {
	int ms_monitorSocket;
	char* ms_buffer;
	size_t ms_bufferLength;
} ms_ocall_socketReceiveBuffer_t;

typedef struct ms_ocall_socketReadTPM_t {
	char* ms_tpmOut;
	size_t ms_tpmOutLength;
} ms_ocall_socketReadTPM_t;

static sgx_status_t SGX_CDECL Enclave_ocall_print(void* pms)
{
	ms_ocall_print_t* ms = SGX_CAST(ms_ocall_print_t*, pms);
	ocall_print(ms->ms_str);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_socketSendBuffer(void* pms)
{
	ms_ocall_socketSendBuffer_t* ms = SGX_CAST(ms_ocall_socketSendBuffer_t*, pms);
	ocall_socketSendBuffer(ms->ms_monitorSocket, ms->ms_buffer);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_socketReceiveBuffer(void* pms)
{
	ms_ocall_socketReceiveBuffer_t* ms = SGX_CAST(ms_ocall_socketReceiveBuffer_t*, pms);
	ocall_socketReceiveBuffer(ms->ms_monitorSocket, ms->ms_buffer, ms->ms_bufferLength);

	return SGX_SUCCESS;
}

static sgx_status_t SGX_CDECL Enclave_ocall_socketReadTPM(void* pms)
{
	ms_ocall_socketReadTPM_t* ms = SGX_CAST(ms_ocall_socketReadTPM_t*, pms);
	ocall_socketReadTPM(ms->ms_tpmOut, ms->ms_tpmOutLength);

	return SGX_SUCCESS;
}

static const struct {
	size_t nr_ocall;
	void * table[4];
} ocall_table_Enclave = {
	4,
	{
		(void*)Enclave_ocall_print,
		(void*)Enclave_ocall_socketSendBuffer,
		(void*)Enclave_ocall_socketReceiveBuffer,
		(void*)Enclave_ocall_socketReadTPM,
	}
};
sgx_status_t generate_random_number(sgx_enclave_id_t eid, int* retval)
{
	sgx_status_t status;
	ms_generate_random_number_t ms;
	status = sgx_ecall(eid, 0, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t trustedAttestMinion(sgx_enclave_id_t eid, int minionSocket, int messageLength)
{
	sgx_status_t status;
	ms_trustedAttestMinion_t ms;
	ms.ms_minionSocket = minionSocket;
	ms.ms_messageLength = messageLength;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t trustedAttestMinionReturn(sgx_enclave_id_t eid, int* retval, int minionSocket, int messageLength)
{
	sgx_status_t status;
	ms_trustedAttestMinionReturn_t ms;
	ms.ms_minionSocket = minionSocket;
	ms.ms_messageLength = messageLength;
	status = sgx_ecall(eid, 2, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t trustedProcessAttestation(sgx_enclave_id_t eid, int clientSocket, char* result, size_t resultLength, char* nonce, int messageLength)
{
	sgx_status_t status;
	ms_trustedProcessAttestation_t ms;
	ms.ms_clientSocket = clientSocket;
	ms.ms_result = result;
	ms.ms_resultLength = resultLength;
	ms.ms_nonce = nonce;
	ms.ms_nonce_len = nonce ? strlen(nonce) + 1 : 0;
	ms.ms_messageLength = messageLength;
	status = sgx_ecall(eid, 3, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t developerTrustedProcessAttestation(sgx_enclave_id_t eid, int* retval, int developerSocket, char* approvedSHA1, char* approvedConfiguration, int messageLength)
{
	sgx_status_t status;
	ms_developerTrustedProcessAttestation_t ms;
	ms.ms_developerSocket = developerSocket;
	ms.ms_approvedSHA1 = approvedSHA1;
	ms.ms_approvedSHA1_len = approvedSHA1 ? strlen(approvedSHA1) + 1 : 0;
	ms.ms_approvedConfiguration = approvedConfiguration;
	ms.ms_approvedConfiguration_len = approvedConfiguration ? strlen(approvedConfiguration) + 1 : 0;
	ms.ms_messageLength = messageLength;
	status = sgx_ecall(eid, 4, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

