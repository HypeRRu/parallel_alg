#ifndef FINE_GRAINED_THREAD_SAFE_QUEUE_HPP
#define FINE_GRAINED_THREAD_SAFE_QUEUE_HPP

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <memory>

namespace threads
{

template < typename T >
class ThreadSafeQueue
{
public:
    ThreadSafeQueue();

    void push( std::shared_ptr< T > value );
    std::shared_ptr< T > try_pop();
    std::shared_ptr< T > pop_and_wait();
    void cancel();
private:
    struct Node
    {
        std::shared_ptr< T > value;
        std::shared_ptr< Node > next;
    }; // Node

    std::shared_ptr< Node > getTail();
    std::shared_ptr< Node > popHead();

    std::shared_ptr< Node > head_;
    std::shared_ptr< Node > tail_;

    std::mutex headMtx_;
    std::mutex tailMtx_;

    std::condition_variable cond_;
    std::atomic_bool canceled_;

}; // ThreadSafeQueue


template < typename T >
ThreadSafeQueue< T >::ThreadSafeQueue()
    : canceled_{ false }
{
    tail_ = std::make_shared< Node >( nullptr, nullptr ); // null node
    head_ = tail_; // head is null
}


template < typename T >
std::shared_ptr< Node > ThreadSafeQueue< T >::getTail()
{
    std::unique_lock< std::mutex > tailLockGuard{ tailMtx_ };
    return tail_;
}


template < typename T >
std::shared_ptr< Node > ThreadSafeQueue< T >::popHead()
{
    std::unique_lock< std::mutex > headLockGuard{ headMtx_ };

    if ( getTail() == head_ )
    {
        return nullptr;
    }
    std::shared_ptr< Node > oldHead = head_;
    head_ = head_->next;
    return oldHead;
}


template < typename T >
void ThreadSafeQueue< T >::push( std::shared_ptr< T > value )
{
    std::shared_ptr< Node > newTail = std::make_shared( nullptr, nullptr );

    std::unique_lock< std::mutex > tailLockGuard{ tailMtx_ };
    tail_->next = newTail;
    tail_->value = value;
    tail_ = newTail;
} // push


template < typename T >
std::shared_ptr< T > ThreadSafeQueue< T >::try_pop()
{
    std::shared_ptr< Node > node = popHead();
    return node.get() ? node->value : nullptr;
} // try_pop


template < typename T >
std::shared_ptr< T > ThreadSafeQueue< T >::pop_and_wait()
{
    /// @todo
} // pop_and_wait

} // namespace threads

#endif // FINE_GRAINED_THREAD_SAFE_QUEUE_HPP
