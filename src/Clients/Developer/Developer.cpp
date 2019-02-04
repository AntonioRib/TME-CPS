#include "Developer.h"
#include <tss2/tss2_esys.h>

#include "tpm2_convert.h"
#include "files.h"
#include "log.h"
#include "pcr.h"
#include "tpm2_alg_util.h"
#include "tpm2_auth_util.h"
#include "tpm2_session.h"
#include "tpm2_tool.h"
#include "tpm2_util.h"
#define SUPPORTED_ABI_VERSION \
{ \
    .tssCreator = 1, \
    .tssFamily = 2, \
    .tssLevel = 1, \
    .tssVersion = 108, \
}
using namespace std;

const int REQUEST_FLAG_INDEX = 1;
const int MONITOR_HOST_FLAG_INDEX = 2;
const int USERNAME_FLAG_INDEX = 4;
const int KEY_FLAG_INDEX = 6;
const int APPDIR_FLAG_INDEX = 8;
const int INSTANCES_FLAG_INDEX = 10;

const int ERROR_CODE = 255;

//  Usage:
//  Developer -h: help
//  Developer

Developer::Developer() {
    std::cout << "Developer created\n";
}

Developer::Developer(string monitorHost, string username, string key, string appDir, int instances) : monitorHost{monitorHost}, username{username}, key{key}, appDir{appDir}, instances{instances} {
    std::cout << "Developer created\n";
}

Developer::Developer(string monitorHost, string username, string key, string appDir) : monitorHost{monitorHost}, username{username}, key{key}, appDir{appDir}  {
    std::cout << "Developer created\n";
}

void Developer::attestMonitor(int monitorSocket){
    char buffer[SocketUtils::MESSAGE_BYTES];
    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    string attestationRequestString = Messages::ATTEST + " " + AttestationConstants::NONCE;
    General::stringToCharArray(attestationRequestString, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugDeveloper)
        cout << "Wrote: " << buffer << " to server\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugDeveloper)
        cout << "Recieved from server: " << buffer << "\n";

    string quote(buffer);
    vector<string> splittedQuote = General::splitString(quote);

    if (splittedQuote[0] == Messages::QUOTE && splittedQuote[1] == AttestationConstants::QUOTE && splittedQuote[2] == AttestationConstants::PCR_SHA1 && splittedQuote[3] == AttestationConstants::PCR_SHA1) {
        string approvedMessage = Messages::OK_APPROVED;
        General::stringToCharArray(approvedMessage, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugDeveloper)
            cout << "Wrote: " << buffer << " to server\n";
    } else {
        General::stringToCharArray(Messages::NOT_APPROVED, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugDeveloper)
            cout << "Wrote: " << buffer << " to server\n";
    }
}

bool Developer::sendSyncMessageAndGetResponse(string message){
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(monitorHost);

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::MONITOR_DEVELOPER_PORT);
    bcopy((char *)serverHost->h_addr, (char *)&serverAddress.sin_addr.s_addr, serverHost->h_length);
    int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(monitorSocket, serverAddress);
    if (DebugFlags::debugDeveloper)
        cout << "Connected to the server\n";

    attestMonitor(monitorSocket);

    bool sendResult = true;
    vector<string> splittedMessage = General::splitString(message);
    if (splittedMessage[0] == Messages::NEW_APP){
        sendResult = Developer::sendApp();
    }

    if (!sendResult)
        return sendResult;

    char buffer[SocketUtils::MESSAGE_BYTES];
    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    General::stringToCharArray(message, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugDeveloper)
        cout << "Wrote: " << buffer << " to server\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugDeveloper)
        cout << "Recieved from server: " << buffer << "\n";

    string response(buffer);
    vector<string> splittedResponse = General::splitString(response);

    if (splittedResponse[0] == Messages::OK) {
        close(monitorSocket);
        return true;
    } else if (splittedResponse[0] == Messages::NOT_OK) {
        close(monitorSocket);
        return false;
    }
    close(monitorSocket);
    return false;
}

bool Developer::deleteApp(){
    if (DebugFlags::debugDeveloper)
        cout << "Deleting app on monitor\n";

    string message = Messages::DELETE_APP + " " + Developer::username + " " + appDir.substr(appDir.find("/")+1);
    bool messageResult;
    messageResult = sendSyncMessageAndGetResponse(message);

    if (!messageResult)
        return false;
    return true;
}

bool Developer::sendApp(){
    char* scpArgsStream;
    int size = asprintf(&scpArgsStream, "%s -r -i %s -oStrictHostKeyChecking=no %s %s@%s:%s",
                        ProcessBinaries::SCP.c_str(), key.c_str(), appDir.c_str(), username.c_str(),
                        monitorHost.c_str(), Directories::APPS_DIR_MONITOR.c_str());//, appDir.substr(appDir.find("/")+1).c_str());
    string scpArgsStreamStr(scpArgsStream);
    std::vector<std::string> scpArgsStreamVec = General::splitString(scpArgsStreamStr);
    char* scpArgsStreamCharVec[scpArgsStreamVec.size()];
    int i = 0;
    for (const std::string& str : scpArgsStreamVec) {
        scpArgsStreamCharVec[i] = const_cast<char*>(str.c_str());
        // cout << scpArgsStreamCharVec[i] << "\n";
        i++;
    }
    scpArgsStreamCharVec[i] = NULL;
    if (DebugFlags::debugDeveloper)
        cout << "Executing command: " << scpArgsStream << "\n";
    fflush(NULL);
    pid_t pid = fork();
    if (pid == 0) {
        int result = execvp(ProcessBinaries::SCP.c_str(), scpArgsStreamCharVec);
        if (result == -1) {
            if (DebugFlags::debugDeveloper){
                cout << "Command failed\n";
                cout << "Result: " << result << "\n";
                cout << strerror(errno) << "\n";
            }
            exit(-1);
        }
        cout << "Result: " << result << "\n";
        cout << strerror(errno) << "\n";
        exit(0);
    }

    if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == ERROR_CODE)
            return false;
    }
    return true;
}

bool Developer::deployApp(){
    if (DebugFlags::debugDeveloper)
        cout << "Deploying app on monitor\n";

    string message = Messages::NEW_APP + " " + Developer::username + " " + appDir.substr(appDir.find("/") + 1) + " " + to_string(Developer::instances);
    bool messageResult;
    messageResult = sendSyncMessageAndGetResponse(message);

    if (!messageResult)
        return false;
    return true;
}

void printHelp() {
    std::cout << "Usage to Deploy App: Developer -deploy -m monitorHost -u username -k key -a appDir -i #instances\n";
    std::cout << "Usage to Delete App: Developer -delete -m monitorHost -u username -k key -a appDir \n";
}

static ESYS_CONTEXT* ctx_init(TSS2_TCTI_CONTEXT *tcti_ctx) {

	TSS2_ABI_VERSION abi_version = SUPPORTED_ABI_VERSION;
	ESYS_CONTEXT *esys_ctx;

	TSS2_RC rval = Esys_Initialize(&esys_ctx, tcti_ctx, &abi_version);
	if (rval != TPM2_RC_SUCCESS) {
		return NULL;
	}

	return esys_ctx;
}

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

static tpm_quote_ctx ctx;
extern bool output_enabled = true;

static bool write_output_files(TPM2B_ATTEST *quoted, TPMT_SIGNATURE *signature) {

	bool res = true;
	if (ctx.signature_path) {
		res &= tpm2_convert_sig_save(signature, ctx.sig_format, ctx.signature_path);
	}

	if (ctx.message_path) {
		res &= files_save_bytes_to_file(ctx.message_path,
			(UINT8*)quoted->attestationData,
			quoted->size);
	}

	return res;
}

int main(int argc, char* argv[]) {
	TSS2_TCTI_CONTEXT *tcti = NULL;
	ESYS_CONTEXT *ectx = NULL;
	ectx  = ctx_init(tcti);
	ctx.auth.session_data = TPMS_AUTH_COMMAND_INIT(TPM2_RS_PW);
	ctx.qualifyingData = TPM2B_EMPTY_INIT;
	TPM2B_DIGEST *random_bytes;


	TPM2_RC rval;
	TPMT_SIG_SCHEME inScheme;
	TPM2B_ATTEST *quoted = NULL;
	TPMT_SIGNATURE *signature = NULL;

	if (!ctx.flags.G || !get_signature_scheme(ectx, ctx.context_object.tr_handle,
		ctx.sig_hash_algorithm, &inScheme)) {
		inScheme.scheme = TPM2_ALG_NULL;
	}

	ESYS_TR shandle1 = tpm2_auth_util_get_shandle(ectx,
		ctx.context_object.tr_handle,
		&ctx.auth.session_data, ctx.auth.session);
	if (shandle1 == ESYS_TR_NONE) {
		LOG_ERR("Failed to get shandle");
		return -1;
	}

	rval = Esys_Quote(ectx, ctx.context_object.tr_handle,
		shandle1, ESYS_TR_NONE, ESYS_TR_NONE,
		&ctx.qualifyingData, &inScheme, &ctx.pcrSelections,
		&quoted, &signature);
	if (rval != TPM2_RC_SUCCESS)
	{
		LOG_PERR(Esys_Quote, rval);
		return -1;
	}

	tpm2_tool_output("quoted: ");
	tpm2_util_print_tpm2b((TPM2B *)quoted);
	tpm2_tool_output("\nsignature:\n");
	tpm2_tool_output("  alg: %s\n", tpm2_alg_util_algtostr(signature->sigAlg, tpm2_alg_util_flags_sig));

	UINT16 size;
	BYTE *sig = tpm2_convert_sig(&size, signature);
	tpm2_tool_output("  sig: ");
	tpm2_util_hexdump(sig, size);
	tpm2_tool_output("\n");
	free(sig);

	bool res = write_output_files(quoted, signature);

	free(quoted);
	free(signature);
	//--//
    string monitorHost;
    string username;
    string key;
    string appDir;
    string instances;
    Developer* developer;

    if (argc != 10 && argc != 12) {
        std::cout << "Invalid number of fields\n";
        printHelp();
        return -1;
    }
    string command = argv[REQUEST_FLAG_INDEX];
    bool commandResult = false;
    if(command == "-deploy"){
        monitorHost = argv[MONITOR_HOST_FLAG_INDEX+1];
        username = argv[USERNAME_FLAG_INDEX+1];
        key = argv[KEY_FLAG_INDEX+1];
        appDir = argv[APPDIR_FLAG_INDEX+1];
        instances = argv[INSTANCES_FLAG_INDEX+1];
        developer = new Developer(monitorHost, username, key, appDir, atoi(instances.c_str()));
        commandResult = developer->deployApp();
    } else if (command == "-delete"){
        monitorHost = argv[MONITOR_HOST_FLAG_INDEX+1];
        username = argv[USERNAME_FLAG_INDEX+1];
        key = argv[KEY_FLAG_INDEX+1];
        appDir = argv[APPDIR_FLAG_INDEX+1];
        developer = new Developer(monitorHost, username, key, appDir);
        commandResult = developer->deleteApp();
    } else {
        std::cout << "Unknown Command\n";
        printHelp();
    }

    if(commandResult)
        cout << "Success";
    else 
        cout << "Failed";

    return 0;
}