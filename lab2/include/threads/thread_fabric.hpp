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
    ThreadFabric()
    {}

    ~ThreadFabric()
    {
        tasks_.cancel();
        wait();
    } // ~ThreadFabric

    void wait()
    {
        for ( auto& producer: producers_ )
        {
            producer.join();
        }
        producers_.clear();
        for ( auto& consumer: consumers_ )
        {
            consumer.join();
        }
        consumers_.clear();
    } // wait

    bool addTask( const Task& task )
    {
        return tasks_.push( task );
    }

    std::shared_ptr< Task > getTask()
    {
        return tasks_.pop_and_wait();
    }

    void addProducer( const std::function< Task&& () >& producer )
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
            if ( !addTask( task ) )
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
            task = getTask();
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
