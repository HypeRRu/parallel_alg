#ifndef THREADS_THREAD_SAFE_QUEUE_HPP
#define THREADS_THREAD_SAFE_QUEUE_HPP

#include <queue>
#include <condition_variable>
#include <atomic>
#include <memory>
#include <type_traits>
#if defined( ALLOW_SHARED_LOCKS )
#include <shared_mutex>
#else // !defined( ALLOW_SHARED_LOCKS )
#include <mutex>
#endif // defined( ALLOW_SHARED_LOCKS )


namespace threads
{

template < typename T >
class ThreadSafeQueue
{
public:
    ThreadSafeQueue( const ThreadSafeQueue& ) = delete;
    ThreadSafeQueue& operator=( const ThreadSafeQueue& ) = delete;
    
    bool canceled() const
    {
#if defined( ALLOW_SHARED_LOCKS )
        std::shared_lock< std::shared_mutex > lockGuard{ mtx_ };
#else // !defined( ALLOW_SHARED_LOCKS )
        std::unique_lock< std::mutex > lockGuard{ mtx_ };
#endif // defined( ALLOW_SHARED_LOCKS )
        return cancel_.load( std::memory_order_acquire );
    }

    void cancel()
    {
#if defined( ALLOW_SHARED_LOCKS )
        std::unique_lock< std::shared_mutex > lockGuard{ mtx_ };
#else // !defined( ALLOW_SHARED_LOCKS )
        std::unique_lock< std::mutex > lockGuard{ mtx_ };
#endif // defined( ALLOW_SHARED_LOCKS )
        cancel_.store( true, std::memory_order_release );
        cond_.notify_all();
    } // cancel

    bool push( const T& value )
    {
        if ( cancel_.load( std::memory_order_acquire ) )
        {
            return false;
        }
#if defined( ALLOW_SHARED_LOCKS )
        std::unique_lock< std::shared_mutex > lockGuard{ mtx_ };
#else // !defined( ALLOW_SHARED_LOCKS )
        std::unique_lock< std::mutex > lockGuard{ mtx_ };
#endif // defined( ALLOW_SHARED_LOCKS )
        container_.push( value );
        cond_.notify_one();
        return true;
    } // push

    std::shared_ptr< T > pop_and_wait()
    {
        std::shared_ptr< T > value = nullptr;
        while ( true )
        {
#if defined( ALLOW_SHARED_LOCKS )
            std::unique_lock< std::shared_mutex > lockGuard{ mtx_ };
#else // !defined( ALLOW_SHARED_LOCKS )
            std::unique_lock< std::mutex > lockGuard{ mtx_ };
#endif // defined( ALLOW_SHARED_LOCKS )
            cond_.wait( lockGuard, [ this ](){
                return !empty() || cancel_.load( std::memory_order_acquire );
            } );
            if ( empty() )
            {
                /// Случайная разблокировка
                continue;
            }
            if ( cancel_.load( std::memory_order_acquire ) )
            {
                /// Окончание работы
                break;
            }
            value = std::make_shared( container_.front() );
            container_.pop();
        }
        return value;
    } // pop_and_wait

    size_t size() const
    {
#if defined( ALLOW_SHARED_LOCKS )
        std::shared_lock< std::shared_mutex > lockGuard{ mtx_ };
#else // !defined( ALLOW_SHARED_LOCKS )
        std::unique_lock< std::mutex > lockGuard{ mtx_ };
#endif // defined( ALLOW_SHARED_LOCKS )
        return container_.size();
    } // size

    bool empty() const
    {
#if defined( ALLOW_SHARED_LOCKS )
        std::shared_lock< std::shared_mutex > lockGuard{ mtx_ };
#else // !defined( ALLOW_SHARED_LOCKS )
        std::unique_lock< std::mutex > lockGuard{ mtx_ };
#endif // defined( ALLOW_SHARED_LOCKS )
        return container_.empty();
    } // empty

private:
#if defined( ALLOW_SHARED_LOCKS )
    mutable std::shared_mutex mtx_;     ///< Мьютекс для синхронизации операций с очередью.
#else // !defined( ALLOW_SHARED_LOCKS )
    mutable std::mutex mtx_;            ///< Мьютекс для синхронизации операций с очередью.
#endif // defined( ALLOW_SHARED_LOCKS )
    std::queue< T > container_;         ///< Очередь.
    std::condition_variable cond_;      ///< Условная переменная для синхронизации операций с очередью.
    std::atomic_bool cancel_ = false;   ///< Атомарный флаг нужно ли закончить обработку очереди.

}; // class ThreadSafeQueue

} // namespace threads

#endif // THREADS_THREAD_SAFE_QUEUE_HPP
