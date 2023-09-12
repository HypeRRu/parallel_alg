#ifndef THREADS_THREAD_FABRIC_HPP
#define THREADS_THREAD_FABRIC_HPP

#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <mutex>
#include "threads/thread_safe_queue.hpp"

namespace threads
{

template < typename Task >
class ThreadFabric
{
public:
    ~ThreadFabric()
    {
        tasks_.cancel();
    } // ~ThreadFabric

    void addProducer( const std::function< const Task&() >& producer )
    {
        std::unique_lock< std::mutex > lockGuard{ producersMtx_ };
        producers_.emplace_back( &ThreadFabric::produce, this, producer );
    } // addProducer

    void addConsumer( const std::function< void( const Task& ) >& consumer )
    {
        std::unique_lock< std::mutex > lockGuard{ producersMtx_ };
        producers_.emplace_back( &ThreadFabric::consume, this, consumer );
    } // addConsumer

private:
    void produce( std::function< const Task&() > producer )
    {
        while ( true )
        {
            const Task& task = producer();
            if ( !tasks_.push( task ) )
            {
                break;
            }
        }
    } // produce

    void consume( std::function< void( const Task& ) > consumer )
    {
        std::shared_ptr< Task > task;
        while ( true )
        {
            task = tasks_.pop();
            if ( !task.get() )
            {
                break;
            }
            consumer( *task.get() );
        }
    } // consume

private:
    std::mutex producersMtx_;
    std::mutex concumersMtx_;
    std::vector< std::thread > producers_;
    std::vector< std::thread > consumers_;
    ThreadSafeQueue< Task > tasks_;
}; // class ThreadFabric

} // namespace threads

#endif // THREADS_THREAD_FABRIC_HPP
