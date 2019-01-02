#ifndef _sysadmin_h_
#define _sysadmin_h_

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