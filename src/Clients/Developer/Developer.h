#ifndef _DEVELOPER_H_
#define _DEVELOPER_H_

#include <iostream>
#include <string>
using namespace std;

class Developer {
   private:
    string monitorHost;
    string username;
    string key;
    string appDir;
    int instances;

    void attestMonitor(); //TODO
    bool sendSyncMessageAndGetResponse(string message);
    bool deleteApp();
    bool sendApp();
    bool deployApp();

    public: 
    Developer();
    void saySomething(std::string message);
};

#endif