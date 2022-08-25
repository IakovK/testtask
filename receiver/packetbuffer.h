#ifndef PACKETBUFFER_H
#define PACKETBUFFER_H

#include <boost/circular_buffer.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/bind.hpp>

/**
 * based on bounded_buffer sample in https://www.boost.org/doc/libs/1_80_0/doc/html/circular_buffer/examples.html
 */
template <class T>
class PacketBuffer
{
public:

    typedef boost::circular_buffer<T> container_type;
    typedef typename container_type::size_type size_type;
    typedef typename container_type::value_type value_type;
    typedef typename boost::call_traits<value_type>::param_type param_type;

    explicit PacketBuffer(size_type capacity) : m_container(capacity) {}
    
    void push_front(param_type item)
    {
        boost::unique_lock<boost::mutex> lock(m_mutex);
        m_container.push_front(item);
        lock.unlock();
        m_not_empty.notify_one();
    }

    void pop_back(value_type* pItem)
    {
        boost::unique_lock<boost::mutex> lock(m_mutex);
        m_not_empty.wait(lock, boost::bind(&PacketBuffer<value_type>::is_not_empty, this));
        *pItem = m_container.back();
        m_container.pop_back();
        lock.unlock();
    }

private:
    PacketBuffer(const PacketBuffer&);              // Disabled copy constructor
    PacketBuffer& operator = (const PacketBuffer&); // Disabled assign operator

    bool is_not_empty() const { return !m_container.empty(); }

    container_type m_container;
    boost::mutex m_mutex;
    boost::condition_variable m_not_empty;
};

#endif // PACKETBUFFER_H
