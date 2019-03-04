#include "Enclave_u.h"
#include <errno.h>

typedef struct ms_generate_random_number_t {
	int ms_retval;
} ms_generate_random_number_t;

typedef struct ms_trustedAttestMonitor_t {
	int ms_monitorSocket;
	int ms_messageLength;
} ms_trustedAttestMonitor_t;

typedef struct ms_seal_t {
	sgx_status_t ms_retval;
	uint8_t* ms_plaintext;
	size_t ms_plaintext_len;
	sgx_sealed_data_t* ms_sealed_data;
	size_t ms_sealed_size;
} ms_seal_t;

typedef struct ms_unseal_t {
	sgx_status_t ms_retval;
	sgx_sealed_data_t* ms_sealed_data;
	size_t ms_sealed_size;
	uint8_t* ms_plaintext;
	uint32_t ms_plaintext_len;
} ms_unseal_t;

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

sgx_status_t trustedAttestMonitor(sgx_enclave_id_t eid, int monitorSocket, int messageLength)
{
	sgx_status_t status;
	ms_trustedAttestMonitor_t ms;
	ms.ms_monitorSocket = monitorSocket;
	ms.ms_messageLength = messageLength;
	status = sgx_ecall(eid, 1, &ocall_table_Enclave, &ms);
	return status;
}

sgx_status_t seal(sgx_enclave_id_t eid, sgx_status_t* retval, uint8_t* plaintext, size_t plaintext_len, sgx_sealed_data_t* sealed_data, size_t sealed_size)
{
	sgx_status_t status;
	ms_seal_t ms;
	ms.ms_plaintext = plaintext;
	ms.ms_plaintext_len = plaintext_len;
	ms.ms_sealed_data = sealed_data;
	ms.ms_sealed_size = sealed_size;
	status = sgx_ecall(eid, 2, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

sgx_status_t unseal(sgx_enclave_id_t eid, sgx_status_t* retval, sgx_sealed_data_t* sealed_data, size_t sealed_size, uint8_t* plaintext, uint32_t plaintext_len)
{
	sgx_status_t status;
	ms_unseal_t ms;
	ms.ms_sealed_data = sealed_data;
	ms.ms_sealed_size = sealed_size;
	ms.ms_plaintext = plaintext;
	ms.ms_plaintext_len = plaintext_len;
	status = sgx_ecall(eid, 3, &ocall_table_Enclave, &ms);
	if (status == SGX_SUCCESS && retval) *retval = ms.ms_retval;
	return status;
}

