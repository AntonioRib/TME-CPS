#ifndef _monitor_h_
#define _monitor_h_

#include <iostream>
#include <thread>

class Monitor {
    private:
        ;

    protected:
        ;

    public:
        Monitor();
        Monitor(std::string name);
        Monitor(const Monitor&);
        std::string name;
};

#endif