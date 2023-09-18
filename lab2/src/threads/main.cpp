#include <iostream>

#include "threads/tasks.h"
#include "threads/thread_factory.hpp"

constexpr size_t producers = 2;
constexpr size_t consumers = 6;
constexpr size_t producerLimit = 15000;
constexpr size_t consumerLimit = 5000;
constexpr size_t outputLimit = 30000;

int main( int argc, char** argv )
{
    size_t tCount = 1;
    if ( argc > 2 )
    {
        std::cerr << "Usage: threading [thread-count]\n";
        return 1;
    } else if ( argc == 2 )
    {
        tCount = std::stoull( argv[ 1 ], nullptr, 10 );
        if ( tCount == 0 )
        {
            std::cerr << "Threads count can not be 0\n";
            return 1;
        }
    }

    threads::ThreadFactory< threads::TaskGen > generationFactory;
    threads::ThreadFactory< threads::TaskOut > outputFactory;

    for ( size_t i = 0; i < producers; ++i )
    {
        generationFactory.addProducer( &threads::Tasks::generateMatrices, producerLimit );
    }
    for ( size_t i = 0; i < consumers; ++i )
    {
        if ( tCount == 1 )
        {
            generationFactory.addConsumer( std::bind( 
                &threads::Tasks::multiplyMatricesSerial
                , std::placeholders::_1, std::ref( outputFactory ) ), consumerLimit );
        }
        else
        {
            generationFactory.addConsumer( std::bind( 
                &threads::Tasks::multiplyMatricesParallel
                , std::placeholders::_1, std::ref( outputFactory ), tCount ), consumerLimit );
        }
    }
    outputFactory.addConsumer( &threads::Tasks::writeResultMatrix, outputLimit );

    generationFactory.wait();
    outputFactory.wait();
    return 0;
}
