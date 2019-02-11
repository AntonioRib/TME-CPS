#include <tss2/tss2_esys.h>

#include "../../lib/tpm2/tpm2_convert.h"
#include "../../lib/tpm2/files.h"
#include "../../lib/tpm2/log.h"
#include "../../lib/tpm2/pcr.h"
#include "../../lib/tpm2/tpm2_alg_util.h"
#include "../../lib/tpm2/tpm2_auth_util.h"
#include "../../lib/tpm2/tpm2_session.h"
#include "../../lib/tpm2/tpm2_tool.h"
#include "../../lib/tpm2/tpm2_util.h"
#define SUPPORTED_ABI_VERSION \
{ \
    .tssCreator = 1, \
    .tssFamily = 2, \
    .tssLevel = 1, \
    .tssVersion = 108, \
}

namespace TPM{

typedef struct tpm_quote_ctx tpm_quote_ctx;
struct tpm_quote_ctx {
	struct {
		TPMS_AUTH_COMMAND session_data;
		tpm2_session *session;
	} auth;
	char *outFilePath;
	char *signature_path;
	char *message_path;
	tpm2_convert_sig_fmt sig_format;
	TPMI_ALG_HASH sig_hash_algorithm;
	TPM2B_DATA qualifyingData;
	TPML_PCR_SELECTION pcrSelections;
	char *ak_auth_str;
	const char *context_arg;
	tpm2_loaded_object context_object;
	struct {
		UINT16 l : 1;
		UINT16 L : 1;
		UINT16 o : 1;
		UINT16 G : 1;
		UINT16 P : 1;
	} flags;
};

static ESYS_CONTEXT* ctx_init(TSS2_TCTI_CONTEXT *tcti_ctx) {

	TSS2_ABI_VERSION abi_version = SUPPORTED_ABI_VERSION;
	ESYS_CONTEXT *esys_ctx;

	TSS2_RC rval = Esys_Initialize(&esys_ctx, tcti_ctx, &abi_version);
	if (rval != TPM2_RC_SUCCESS) {
		return NULL;
	}

	return esys_ctx;
}


static tpm_quote_ctx ctx;
extern bool output_enabled = true;

void tpm_quote(TPM2B_ATTEST *quoted, TPMT_SIGNATURE *signature){
    // ESYS_CONTEXT *esys_context = (ESYS_CONTEXT *) * state;
	ESYS_CONTEXT *ectx;
    TSS2_TCTI_CONTEXT *tcti;
    Esys_GetTcti(ectx, &tcti);
	ectx  = ctx_init(tcti);

	ctx.auth.session_data = TPMS_AUTH_COMMAND_INIT(TPM2_RS_PW);
	ctx.qualifyingData = TPM2B_EMPTY_INIT;

	TPM2_RC rval;
	TPMT_SIG_SCHEME inScheme;

	//inScheme.scheme = TPM2_ALG_HMAC;
	//inScheme.details.hmac.hashAlg = ctx.sig_hash_algorithm;
	inScheme.scheme = TPM2_ALG_NULL;

	ESYS_TR shandle1 = ESYS_TR_PASSWORD;

	rval = Esys_Quote(ectx, ctx.context_object.tr_handle,
		shandle1, ESYS_TR_NONE, ESYS_TR_NONE,
		&ctx.qualifyingData, &inScheme, &ctx.pcrSelections,
		&quoted, &signature);

	cout << "Quoted: " << (TPM2B *)quoted << "\n";
	cout << "Signature: " << signature << "\n";
    return;

	// free(quoted);
	// free(signature);
    
}

} //namespace TPM