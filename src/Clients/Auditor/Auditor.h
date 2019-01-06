#ifndef _AUDITOR_H_
#define _AUDITOR_H_

#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

class Auditor {
   private:
    unsigned char* monitorSignature;
    unsigned char* hubSignature;
    unsigned char* minionSignature;


   public:
    Auditor();
    void saySomething(std::string message);
    void attestMonitor(const char* hostname);
    void attestLogger(const char* hostname);
    unsigned char* generateSignature(unsigned char* data);
};

#endif