#ifndef _MESSAGESSGX_H_
#define _MESSAGESSGX_H_
// #include <iostream>
// #include <cstdio>
// #include <cstring>
// #include "sgx_urts.h"

namespace MessagesSGX{
    using namespace std;

    //General
    static const char* OK = "OK";
    static const char* NOT_OK = "ERROR";

    //Developer -> Monitor
    static const char* NEW_APP = "NEW_APP";
    static const char* DELETE_APP = "DELETE_APP";

    //Minions -> Monitor
    static const char* REGISTER = "REGISTER";

    //AHub -> Monitor
    static const char* SET_TRUSTED = "SET_TRUSTED";
    static const char* SET_UNTRUSTED = "SET_UNTRUSTED";

    //Admin -> AHub
    static const char* MANAGE = "MANAGE";
    static const char* MANAGE_TEARDOWN = "MANAGE_TEARDOWN";

    //Monitor -> Minions
    static const char* DEPLOY = "DEPLOY";
    static const char* REMOVE = "DELETE";

    static const char* ATTEST = "ATTEST";
    static const char* QUOTE = "QUOTE";
    static const char* OK_APPROVED = "OK_APPROVED";
    static const char* NOT_APPROVED = "NOT_APPROVED";

    static const char* START_MANAGEMENTSESSION = "START_MANAGEMENTSESSION";

    static const char* PURGE = "PURGE";

    static const char* QUIT = "QUIT";
}
#endif