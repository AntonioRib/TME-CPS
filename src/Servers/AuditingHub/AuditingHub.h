#ifndef _auditinghub_h_
#define _auditinghub_h_

#include <iostream>
#include <thread>
#include <map>
using namespace std;

class AuditingHub
{
  private:
    string username;
    string hubKey;
    string monitorHost;
    string hostname;

    unsigned char* approvedConfiguration;
    string approvedSHA1;

   public:
    AuditingHub();
    AuditingHub(std::string hostname);
    AuditingHub(const AuditingHub&);

    //Getters
    string getUsername();
    string getHubKey();
    string getMonitorHost();
    string getHostname();

    unsigned char* getApprovedConfiguration();
    string getApprovedSHA1();

    //Setters
    void setApprovedConfiguration(unsigned char* approvedConfiguration);

    //Functions
    bool checkPermissionAndUnqueue(string remoteHost);
    void removeSession(string remoteHost);
};

#endif