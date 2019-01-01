#ifndef _application_h_
#define _application_h_
#include <iostream>
using namespace std;

class Application {
    string appID;

   private:
    unsigned char* approvedConfiguration;
    ;

   protected:
    ;

   public:
    Application();
    Application(string appID);
    string getAppID();
};

#endif