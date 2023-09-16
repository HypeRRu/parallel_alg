#include <iostream>

#include "threads/tasks.h"
#include "threads/thread_fabric.hpp"


int main()
{
    threads::ThreadFabric< threads::TaskGen > fabricGenCalc;
    threads::ThreadFabric< threads::TaskOut > fabricOut;

    fabricGenCalc.addProducer( &threads::Tasks::generateMatrices );
    fabricGenCalc.addConsumer( std::bind( 
        &threads::Tasks::multiplyMatricesSerial
        , std::placeholders::_1, std::ref( fabricOut ) ) );

    fabricOut.addConsumer( &threads::Tasks::writeResultMatrix );

    fabricGenCalc.wait();
    fabricOut.wait();
    return 0;
}
