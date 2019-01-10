#ifndef _AUDITORINTERFACE_H_
#define _AUDITORINTERFACE_H_

#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
using namespace std;

class AuditorInterface {
   private:
    string monitorHost;
    string hostname;
    string hubStore;


   public:
    AuditorInterface();
    AuditorInterface(string monitorHost, string hostname);
    bool setTrusted(string hostname);
};

#endif