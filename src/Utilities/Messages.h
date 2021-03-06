#ifndef _MESSAGES_H_
#define _MESSAGES_H_
#include <iostream>

namespace Messages{
    using namespace std;

    //General
    static const string OK = "OK";
    static const string NOT_OK = "ERROR";

    //Developer -> Monitor
    static const string NEW_APP = "NEW_APP";
    static const string DELETE_APP = "DELETE_APP";

    //Minions -> Monitor
    static const string REGISTER = "REGISTER";

    //AHub -> Monitor
    static const string SET_TRUSTED = "SET_TRUSTED";
    static const string SET_UNTRUSTED = "SET_UNTRUSTED";
    static const string SET_UNTRUSTEDURGENT = "SET_UNTRUSTEDURGENT";

    //Admin -> AHub
    static const string MANAGE = "MANAGE";
    static const string URGENTMANAGE = "URGENTMANAGE";
    static const string MANAGE_TEARDOWN = "MANAGE_TEARDOWN";

    //Monitor -> Minions
    static const string DEPLOY = "DEPLOY";
    static const string REMOVE = "DELETE";

    static const string ATTEST = "ATTEST";
    static const string QUOTE = "QUOTE";
    static const string OK_APPROVED = "OK_APPROVED";
    static const string NOT_APPROVED = "NOT_APPROVED";

    static const string START_MANAGEMENTSESSION = "START_MANAGEMENTSESSION";

    static const string PURGE = "PURGE";
    static const string IMPORT = "IMPORT";
    static const string EXPORT = "EXPORT";

    static const string QUIT = "QUIT";
}
#endif