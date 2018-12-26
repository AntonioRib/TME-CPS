#ifndef _auditinghub_h_
#define _auditinghub_h_

#include <iostream>
#include <thread>

class AuditingHub
{
  private:
    ;

  public:
    AuditingHub();
    AuditingHub(std::string name);
    AuditingHub(const AuditingHub&);
    std::string name;
};

#endif