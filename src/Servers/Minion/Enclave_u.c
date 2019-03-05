#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_generate_random_number_t {
	int ms_retval;
} ms_generate_random_number_t;

typedef struct ms_minionMonitorRequestTrustedProcessAttestation_t {
	int ms_retval;
	int ms_monitorSocket;
	int ms_messageLength;
} ms_minionMonitorRequestTrustedProcessAttestation_t;

typedef struct ms_minionTrustedProcessAttestation_t {
	int ms_retval;
	int ms_monitorSocket;
	int ms_messageLength;
} ms_minionTrustedProcessAttestation_t;

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

static const struct {
	size_t nr_ocall;
	void * table[3];
} ocall_table_Enclave = {
	3,
	{
		(void*)Enclave_ocall_print,
		(void*)Enclave_ocall_socketSendBuffer,
		(void*)Enclave_ocall_socketReceiveBuffer,
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

sgx_status_t minionMonitorRequestTrustedProcessAttestation(sgx_enclave_id_t eid, int* retval, int monitorSocket, int messageLength)
{
	sgx_status_t status;
	ms_minionMonitorRequestTrustedProcessAttestation_t ms;
	ms.ms_monitorSocket = monitorSocket;
	ms.ms_messageLength = messageLength;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t minionTrustedProcessAttestation(sgx_enclave_id_t eid, int* retval, int monitorSocket, int messageLength)
{
	sgx_status_t status;
	ms_minionTrustedProcessAttestation_t ms;
	ms.ms_monitorSocket = monitorSocket;
	ms.ms_messageLength = messageLength;
	status = sgx_ecall(eid, 2, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

