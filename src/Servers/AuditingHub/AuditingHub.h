#ifndef _auditinghub_h_
#define _auditinghub_h_

#include <iostream>
#include <thread>
#include <map>
#include <utility>
using namespace std;

class AuditingHub
{
  private:
    string hubUserName;
    string hubKey;
    string monitorHost;
    string hostname;

    unsigned char *approvedConfiguration;
    string approvedSHA1;

    map<long, thread*> temporaryThreads;
    map<string, thread*> remoteHostThreadMap;

   public:
    AuditingHub();
    AuditingHub(string hubUserName, string hubKey, string monitorHost, string hotsName);
    AuditingHub(const AuditingHub&);

    //Getters
    string getHubUserName();
    string getHubKey();
    string getMonitorHost();
    string getHostname();

    unsigned char* getApprovedConfiguration();
    string getApprovedSHA1();

    //TODO this may need to pass the map by reference &
    map<long, thread*> getTemporaryThreads(); 
    map<string, thread*> getRemoteHostThreadMap(); 

    //Setters
    void
    setApprovedConfiguration(unsigned char *approvedConfiguration);
    void setApprovedSH1Configuration(string approvedSHA1Configuration);

    //Functions
    bool checkPermissionAndUnqueue(string remoteHost);
    void removeSession(string remoteHost);
};

#endif