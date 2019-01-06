#ifndef _APPLICATION_H_
#define _APPLICATION_H_
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