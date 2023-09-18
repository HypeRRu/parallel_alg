#ifndef THREADS_THREAD_SAFE_QUEUE_HPP
#define THREADS_THREAD_SAFE_QUEUE_HPP

#include <list>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <type_traits>
#include <mutex>
#include <iostream>

#define FINE_GRAINED_LOCK

namespace threads
{

template < typename T >
class ThreadSafeQueue
{
public:
    ThreadSafeQueue()
        : cancel_{ false }
    {}
    ThreadSafeQueue( const ThreadSafeQueue& ) = delete;
    ThreadSafeQueue& operator=( const ThreadSafeQueue& ) = delete;
    
    bool canceled() const
    {
        return cancel_.load( std::memory_order_acquire );
    }

    void cancel()
    {
        cancel_.store( true, std::memory_order_release );
        cond_.notify_all();
    } // cancel

    bool push( T&& value )
    {
        if ( cancel_.load( std::memory_order_acquire ) )
        {
            return false;
        }
        // std::cerr << "before pushing " << &container_ << "\n";
#if defined( FINE_GRAINED_LOCK )
        std::unique_lock< std::mutex > lockGuard{ tailMtx_ };
#else // !defined( FINE_GRAINED_LOCK )
        std::unique_lock< std::mutex > lockGuard{ mtx_ };
#endif // defined( FINE_GRAINED_LOCK )
        // std::cerr << "pushing " << &container_ << "\n";
        container_.push_back( value );
        // std::cerr << "after push  " << &container_ << " " << container_.size() << '\n';
        cond_.notify_one();
        return true;
    } // push

    std::shared_ptr< T > pop_and_wait()
    {
        std::shared_ptr< T > value = nullptr;
        while ( true )
        {
            // std::cerr << "before pop cond " << &container_ << "\n";
#if defined( FINE_GRAINED_LOCK )
            std::unique_lock< std::mutex > lockGuard{ headMtx_ };
#else // !defined( FINE_GRAINED_LOCK )
            std::unique_lock< std::mutex > lockGuard{ mtx_ };
#endif // defined( FINE_GRAINED_LOCK )
            cond_.wait( lockGuard, [ this ](){
                return !container_.empty() || cancel_.load( std::memory_order_acquire );
            } );
            // std::cerr << "after pop cond " << &container_ << "\n";
            if ( cancel_.load( std::memory_order_acquire ) )
            {
                /// Окончание работы
                break;
            }
            if ( container_.empty() )
            {
                /// Случайная разблокировка
                continue;
            }
            // std::cerr << "poping " << &container_ << "\n";
            value = std::make_shared< T >( container_.front() );
            container_.pop_front();
            // std::cerr << "after pop " << &container_ << " " << container_.size() << '\n';
            break;
        }
        return value;
    } // pop_and_wait

private:
#if defined( FINE_GRAINED_LOCK )
    mutable std::mutex headMtx_;        ///< Мьютекс для синхронизации операций с концом очереди.
    mutable std::mutex tailMtx_;        ///< Мьютекс для синхронизации операций с началом очереди.
#else // !defined( FINE_GRAINED_LOCK )
    mutable std::mutex mtx_;            ///< Мьютекс для синхронизации операций с очередью.
#endif // defined( FINE_GRAINED_LOCK )
    std::list< T > container_;          ///< Очередь.
    std::condition_variable cond_;      ///< Условная переменная для синхронизации операций с очередью.
    std::atomic_bool cancel_;           ///< Атомарный флаг нужно ли закончить обработку очереди.

}; // class ThreadSafeQueue

} // namespace threads

#endif // THREADS_THREAD_SAFE_QUEUE_HPP
