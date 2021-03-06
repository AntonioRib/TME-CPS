#ifndef _MINION_H_
#define _MINION_H_

#include <iostream>
#include <map>
#include <thread>
#include <utility>
#include <vector>
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
#include "../Application/Application.h"

using namespace std;

class Minion
{
    private:
    string ipAddress;
    map<string, Application*> minionApplications;

    public:
    Minion();
    Minion(string ipAddress);
    Minion(const Minion&);

    //Getters
    // string getMonitorHost();
    string getHostname();
    string getIpAddress();
    map<string, Application*> getApplications();

    void addApp(Application* app);
    void removeApp(string appID);
};

#endif