#include "Enclave_t.h"

#include "sgx_trts.h" /* for sgx_ocalloc, sgx_is_outside_enclave */
#include "sgx_lfence.h" /* for sgx_lfence */

#include <errno.h>
#include <mbusafecrt.h> /* for memcpy_s etc */
#include <stdlib.h> /* for malloc/free etc */

#define CHECK_REF_POINTER(ptr, siz) do {	\
	if (!(ptr) || ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_UNIQUE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_outside_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)

#define CHECK_ENCLAVE_POINTER(ptr, siz) do {	\
	if ((ptr) && ! sgx_is_within_enclave((ptr), (siz)))	\
		return SGX_ERROR_INVALID_PARAMETER;\
} while (0)


typedef struct ms_generate_random_number_t {
	int ms_retval;
} ms_generate_random_number_t;

typedef struct ms_sysAdminTrustedProcessAttestation_t {
	int ms_retval;
	int ms_adminSocket;
	char* ms_approvedSHA1;
	size_t ms_approvedSHA1_len;
	char* ms_approvedConfiguration;
	size_t ms_approvedConfiguration_len;
	int ms_messageLength;
} ms_sysAdminTrustedProcessAttestation_t;

typedef struct ms_trustedProcessAttestation_t {
	int ms_clientSocket;
	char* ms_result;
	size_t ms_resultLength;
	char* ms_nonce;
	size_t ms_nonce_len;
	int ms_messageLength;
} ms_trustedProcessAttestation_t;

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

static sgx_status_t SGX_CDECL sgx_generate_random_number(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_generate_random_number_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_generate_random_number_t* ms = SGX_CAST(ms_generate_random_number_t*, pms);
	sgx_status_t status = SGX_SUCCESS;



	ms->ms_retval = generate_random_number();


	return status;
}

static sgx_status_t SGX_CDECL sgx_sysAdminTrustedProcessAttestation(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_sysAdminTrustedProcessAttestation_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_sysAdminTrustedProcessAttestation_t* ms = SGX_CAST(ms_sysAdminTrustedProcessAttestation_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_approvedSHA1 = ms->ms_approvedSHA1;
	size_t _len_approvedSHA1 = ms->ms_approvedSHA1_len ;
	char* _in_approvedSHA1 = NULL;
	char* _tmp_approvedConfiguration = ms->ms_approvedConfiguration;
	size_t _len_approvedConfiguration = ms->ms_approvedConfiguration_len ;
	char* _in_approvedConfiguration = NULL;

	CHECK_UNIQUE_POINTER(_tmp_approvedSHA1, _len_approvedSHA1);
	CHECK_UNIQUE_POINTER(_tmp_approvedConfiguration, _len_approvedConfiguration);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_approvedSHA1 != NULL && _len_approvedSHA1 != 0) {
		_in_approvedSHA1 = (char*)malloc(_len_approvedSHA1);
		if (_in_approvedSHA1 == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_approvedSHA1, _len_approvedSHA1, _tmp_approvedSHA1, _len_approvedSHA1)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_approvedSHA1[_len_approvedSHA1 - 1] = '\0';
		if (_len_approvedSHA1 != strlen(_in_approvedSHA1) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}
	if (_tmp_approvedConfiguration != NULL && _len_approvedConfiguration != 0) {
		_in_approvedConfiguration = (char*)malloc(_len_approvedConfiguration);
		if (_in_approvedConfiguration == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_approvedConfiguration, _len_approvedConfiguration, _tmp_approvedConfiguration, _len_approvedConfiguration)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_approvedConfiguration[_len_approvedConfiguration - 1] = '\0';
		if (_len_approvedConfiguration != strlen(_in_approvedConfiguration) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

	ms->ms_retval = sysAdminTrustedProcessAttestation(ms->ms_adminSocket, _in_approvedSHA1, _in_approvedConfiguration, ms->ms_messageLength);
err:
	if (_in_approvedSHA1) free(_in_approvedSHA1);
	if (_in_approvedConfiguration) free(_in_approvedConfiguration);

	return status;
}

static sgx_status_t SGX_CDECL sgx_trustedProcessAttestation(void* pms)
{
	CHECK_REF_POINTER(pms, sizeof(ms_trustedProcessAttestation_t));
	//
	// fence after pointer checks
	//
	sgx_lfence();
	ms_trustedProcessAttestation_t* ms = SGX_CAST(ms_trustedProcessAttestation_t*, pms);
	sgx_status_t status = SGX_SUCCESS;
	char* _tmp_result = ms->ms_result;
	size_t _tmp_resultLength = ms->ms_resultLength;
	size_t _len_result = _tmp_resultLength;
	char* _in_result = NULL;
	char* _tmp_nonce = ms->ms_nonce;
	size_t _len_nonce = ms->ms_nonce_len ;
	char* _in_nonce = NULL;

	CHECK_UNIQUE_POINTER(_tmp_result, _len_result);
	CHECK_UNIQUE_POINTER(_tmp_nonce, _len_nonce);

	//
	// fence after pointer checks
	//
	sgx_lfence();

	if (_tmp_result != NULL && _len_result != 0) {
		if ((_in_result = (char*)malloc(_len_result)) == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		memset((void*)_in_result, 0, _len_result);
	}
	if (_tmp_nonce != NULL && _len_nonce != 0) {
		_in_nonce = (char*)malloc(_len_nonce);
		if (_in_nonce == NULL) {
			status = SGX_ERROR_OUT_OF_MEMORY;
			goto err;
		}

		if (memcpy_s(_in_nonce, _len_nonce, _tmp_nonce, _len_nonce)) {
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}

		_in_nonce[_len_nonce - 1] = '\0';
		if (_len_nonce != strlen(_in_nonce) + 1)
		{
			status = SGX_ERROR_UNEXPECTED;
			goto err;
		}
	}

	trustedProcessAttestation(ms->ms_clientSocket, _in_result, _tmp_resultLength, _in_nonce, ms->ms_messageLength);
err:
	if (_in_result) {
		if (memcpy_s(_tmp_result, _len_result, _in_result, _len_result)) {
			status = SGX_ERROR_UNEXPECTED;
		}
		free(_in_result);
	}
	if (_in_nonce) free(_in_nonce);

	return status;
}

SGX_EXTERNC const struct {
	size_t nr_ecall;
	struct {void* ecall_addr; uint8_t is_priv;} ecall_table[3];
} g_ecall_table = {
	3,
	{
		{(void*)(uintptr_t)sgx_generate_random_number, 0},
		{(void*)(uintptr_t)sgx_sysAdminTrustedProcessAttestation, 0},
		{(void*)(uintptr_t)sgx_trustedProcessAttestation, 0},
	}
};

SGX_EXTERNC const struct {
	size_t nr_ocall;
	uint8_t entry_table[3][3];
} g_dyn_entry_table = {
	3,
	{
		{0, 0, 0, },
		{0, 0, 0, },
		{0, 0, 0, },
	}
};


sgx_status_t SGX_CDECL ocall_print(const char* str)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_str = str ? strlen(str) + 1 : 0;

	ms_ocall_print_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_print_t);
	void *__tmp = NULL;


	CHECK_ENCLAVE_POINTER(str, _len_str);

	ocalloc_size += (str != NULL) ? _len_str : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_print_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_print_t));
	ocalloc_size -= sizeof(ms_ocall_print_t);

	if (str != NULL) {
		ms->ms_str = (const char*)__tmp;
		if (memcpy_s(__tmp, ocalloc_size, str, _len_str)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_str);
		ocalloc_size -= _len_str;
	} else {
		ms->ms_str = NULL;
	}
	
	status = sgx_ocall(0, ms);

	if (status == SGX_SUCCESS) {
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_socketSendBuffer(int monitorSocket, char* buffer)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_buffer = buffer ? strlen(buffer) + 1 : 0;

	ms_ocall_socketSendBuffer_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_socketSendBuffer_t);
	void *__tmp = NULL;


	CHECK_ENCLAVE_POINTER(buffer, _len_buffer);

	ocalloc_size += (buffer != NULL) ? _len_buffer : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_socketSendBuffer_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_socketSendBuffer_t));
	ocalloc_size -= sizeof(ms_ocall_socketSendBuffer_t);

	ms->ms_monitorSocket = monitorSocket;
	if (buffer != NULL) {
		ms->ms_buffer = (char*)__tmp;
		if (memcpy_s(__tmp, ocalloc_size, buffer, _len_buffer)) {
			sgx_ocfree();
			return SGX_ERROR_UNEXPECTED;
		}
		__tmp = (void *)((size_t)__tmp + _len_buffer);
		ocalloc_size -= _len_buffer;
	} else {
		ms->ms_buffer = NULL;
	}
	
	status = sgx_ocall(1, ms);

	if (status == SGX_SUCCESS) {
	}
	sgx_ocfree();
	return status;
}

sgx_status_t SGX_CDECL ocall_socketReceiveBuffer(int monitorSocket, char* buffer, size_t bufferLength)
{
	sgx_status_t status = SGX_SUCCESS;
	size_t _len_buffer = bufferLength;

	ms_ocall_socketReceiveBuffer_t* ms = NULL;
	size_t ocalloc_size = sizeof(ms_ocall_socketReceiveBuffer_t);
	void *__tmp = NULL;

	void *__tmp_buffer = NULL;

	CHECK_ENCLAVE_POINTER(buffer, _len_buffer);

	ocalloc_size += (buffer != NULL) ? _len_buffer : 0;

	__tmp = sgx_ocalloc(ocalloc_size);
	if (__tmp == NULL) {
		sgx_ocfree();
		return SGX_ERROR_UNEXPECTED;
	}
	ms = (ms_ocall_socketReceiveBuffer_t*)__tmp;
	__tmp = (void *)((size_t)__tmp + sizeof(ms_ocall_socketReceiveBuffer_t));
	ocalloc_size -= sizeof(ms_ocall_socketReceiveBuffer_t);

	ms->ms_monitorSocket = monitorSocket;
	if (buffer != NULL) {
		ms->ms_buffer = (char*)__tmp;
		__tmp_buffer = __tmp;
		memset(__tmp_buffer, 0, _len_buffer);
		__tmp = (void *)((size_t)__tmp + _len_buffer);
		ocalloc_size -= _len_buffer;
	} else {
		ms->ms_buffer = NULL;
	}
	
	ms->ms_bufferLength = bufferLength;
	status = sgx_ocall(2, ms);

	if (status == SGX_SUCCESS) {
		if (buffer) {
			if (memcpy_s((void*)buffer, _len_buffer, __tmp_buffer, _len_buffer)) {
				sgx_ocfree();
				return SGX_ERROR_UNEXPECTED;
			}
		}
	}
	sgx_ocfree();
	return status;
}

