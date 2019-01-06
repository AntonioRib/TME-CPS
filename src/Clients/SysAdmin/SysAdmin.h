#ifndef _SYSADMIN_H_
#define _SYSADMIN_H_

#include <iostream>
#include <string>
#include <thread>
using namespace std;

class SysAdmin {
   private:
    string username;
    string hubHost;
    string remoteHost;
    string key;
    string hubSocket;
 
     void attestLogger(); //TODO
     bool starLocalProxy();
     bool manageNode();

   public:
    SysAdmin();
    void saySomething(std::string message);
};

#endif