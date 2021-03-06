#ifndef _GENERAL_H_
#define _GENERAL_H_
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace Scripts {
    static const string PURGE_MINION = "PurgeMinion.sh";
    static const string DELETE_APP = "DeleteApp.sh";
    static const string IMPORT_MINION = "Import.sh";
    static const string EXPORT_MINION = "Export.sh";
}  // namespace Directories

namespace Directories {
    static const string APPS_DIR_MONITOR = "AppsMonitor/";
    static const string APPS_DIR_MINION = "AppsMinion/";
    static const string APPS_DIR_IMPORTEXPORT = "AppsImport/";
}  // namespace Directories

namespace ProcessBinaries {
    static const string SCP = "/usr/bin/scp";
    static const string SSH = "/usr/bin/ssh";
}  // namespace ProcessBinaries

namespace DebugFlags {
    static const bool debugAuditor      = true;
    static const bool debugDeveloper    = true;
    static const bool debugSysAdmin     = true;
    static const bool debugAuditingHub  = true;
    static const bool debugMinion       = true;
    static const bool debugMonitor      = true;
    }  // namespace DebugFlags

namespace General {

static const unsigned char* const NULL_VALUE = (unsigned char*)"\0";

inline std::vector<std::string> splitString(std::string st) {
    std::istringstream stream(st);
    std::vector<std::string> result((std::istream_iterator<std::string>(stream)),
                                    std::istream_iterator<std::string>());

    return result;
    }

    template <typename M, typename V>
    inline void mapToVec(const M& m, V& v) {
            for (typename M::const_iterator it = m.begin(); it != m.end(); ++it) {
                v.push_back(it->second);
            }
    }

    inline int random_0_to_n(int min, int max) {
            srand(time(NULL));
            return min + (rand() % (int)(max - min + 1));
    }

    inline void stringToCharArray(string message, char* buffer, int length){
            bzero(buffer, length);
            strncpy(buffer, message.c_str(), length);
            buffer[length - 1] = 0;
    }

    inline const std::string currentDateTime() {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
        // for more information about date/time format
        strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);

        return buf;
    }
}

namespace SocketUtils {

const int MESSAGE_BYTES = 2048;

    inline void error(const char* msg) {
        perror(msg);
        exit(1);
    }

    inline sockaddr_in createServerAddress(int port){
        struct sockaddr_in serverAddress;
        bzero((char*)&serverAddress, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(port);
        return serverAddress;
    }

    inline int createServerSocket(sockaddr_in serverAddress) { 
        int serverSocket;
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0)
            error("ERROR opening socket");

        if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
            error("ERROR on binding");

        listen(serverSocket, 5);
        return serverSocket;
    }

    inline void connectToServerSocket(int clientSocket, sockaddr_in serverAddress) {
        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
            error("ERROR connecting");
        }
    }

    inline int acceptClientSocket(int serverSocket){
        struct sockaddr_in clientAddress;
        socklen_t clientAddressLength = sizeof(clientAddress);
        int clientSocket;
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLength);
        if (clientSocket < 0)
            error("ERROR on accept");
        return clientSocket;
    }

    inline int receiveBuffer(int socket, char* buf, int len, int flags) {
        int n;
        n = recv(socket, buf, len, flags);
        if (n < 0)
            error("ERROR reading from socket");
        return n;
    }

    inline int sendBuffer(int socket, char* buf, int len, int flags){
        int n;
        n = send(socket, buf, len, flags);
        if (n < 0)
            error("ERROR writing to socket");
        return n;
    }

    inline struct hostent* getHostByName(string hostname) {
        struct hostent* serverHost;
        serverHost = gethostbyname(hostname.c_str());
        if (serverHost == NULL) {
            error("ERROR, no such host\n");
            throw 10;
        }
        return serverHost;
    }
}  // namespace SocketUtils

#endif