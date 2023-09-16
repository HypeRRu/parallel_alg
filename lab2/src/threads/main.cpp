#include <iostream>

#include "threads/tasks.h"
#include "threads/thread_factory.hpp"

constexpr size_t producers = 1000;
constexpr size_t consumers = 1000;
constexpr size_t producerLimit = 50;
constexpr size_t consumerLimit = 50;
constexpr size_t outputLimit = 50000;

int main()
{
    threads::ThreadFactory< threads::TaskGen > generationFactory;
    threads::ThreadFactory< threads::TaskOut > outputFactory;

    for ( size_t i = 0; i < producers; ++i )
    {
        generationFactory.addProducer( &threads::Tasks::generateMatrices, producerLimit );
    }
    for ( size_t i = 0; i < consumers; ++i )
    {
        generationFactory.addConsumer( std::bind( 
            &threads::Tasks::multiplyMatricesSerial
            , std::placeholders::_1, std::ref( outputFactory ) ), consumerLimit );
    }
    outputFactory.addConsumer( &threads::Tasks::writeResultMatrix, outputLimit );

    generationFactory.wait();
    outputFactory.wait();
    return 0;
}
