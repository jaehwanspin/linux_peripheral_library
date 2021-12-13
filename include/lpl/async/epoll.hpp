#ifndef __LPL_ASYNC_EPOLL_HPP__
#define __LPL_ASYNC_EPOLL_HPP__
///////////////////////////////////////////////////////////////////////
//                    _     ___ _   _ _   ___  __                    //
//                   | |   |_ _| \ | | | | \ \/ /                    //
//                   | |    | ||  \| | | | |\  /                     //
//                   | |___ | || |\  | |_| |/  \                     //
//                   |_____|___|_| \_|\___//_/\_\                    //
//                                                                   //
//   ____  _____ ____  ___ ____  _   _ _____ ____      _    _        //
//  |  _ \| ____|  _ \|_ _|  _ \| | | | ____|  _ \    / \  | |       //
//  | |_) |  _| | |_) || || |_) | |_| |  _| | |_) |  / _ \ | |       //
//  |  __/| |___|  _ < | ||  __/|  _  | |___|  _ <  / ___ \| |___    //
//  |_|   |_____|_| \_\___|_|   |_| |_|_____|_| \_\/_/   \_\_____|   //
//                                                                   //
//             _     ___ ____  ____      _    ______   __            //
//            | |   |_ _| __ )|  _ \    / \  |  _ \ \ / /            //
//            | |    | ||  _ \| |_) |  / _ \ | |_) \ V /             //
//            | |___ | || |_) |  _ <  / ___ \|  _ < | |              //
//            |_____|___|____/|_| \_\/_/   \_\_| \_\|_|              //
//                                                                   //
//        _   _   _ _____ _   _  ___  ____            _ ___ _   _    //
//       / \ | | | |_   _| | | |/ _ \|  _ \   _      | |_ _| \ | |   //
//      / _ \| | | | | | | |_| | | | | |_) | (_)  _  | || ||  \| |   //
//     / ___ \ |_| | | | |  _  | |_| |  _ <   _  | |_| || || |\  |   //
//    /_/   \_\___/  |_| |_| |_|\___/|_| \_\ (_)  \___/|___|_| \_|   //
//                                                                   //
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//                     _____ ____   ___  _     _                     //
//                    | ____|  _ \ / _ \| |   | |                    //
//                    |  _| | |_) | | | | |   | |                    //
//                    | |___|  __/| |_| | |___| |___                 //
//                    |_____|_|    \___/|_____|_____|                //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#include <chrono>

#include <sys/unistd.h>
#include <sys/fcntl.h>
#include <sys/epoll.h>

#include <etl/vector.h>

#include "../common.hpp"


namespace lpl
{

namespace async
{


using event_t = uint32_t;
namespace events
{
    constexpr event_t in        = EPOLLIN;
    constexpr event_t pri       = EPOLLPRI;
    constexpr event_t out       = EPOLLOUT;
    constexpr event_t rdnorm    = EPOLLRDNORM;
    constexpr event_t rdband    = EPOLLRDBAND;
    constexpr event_t wrnorm    = EPOLLWRNORM;
    constexpr event_t wrband    = EPOLLWRBAND;
    constexpr event_t msg       = EPOLLMSG;
    constexpr event_t err       = EPOLLERR;
    constexpr event_t hup       = EPOLLHUP;
    constexpr event_t rdhup     = EPOLLRDHUP;
    constexpr event_t exclusive = EPOLLEXCLUSIVE;
    constexpr event_t wakeup    = EPOLLWAKEUP;
    constexpr event_t oneshot   = EPOLLONESHOT;
    constexpr event_t et        = EPOLLET; // event triggered
};

/**
 * @author Jin
 * @brief  Epoll wrapper
 * for asynchronous event polling
 */
class epoll
{

public:
    template <typename _PeriDevice>
    explicit epoll(_PeriDevice& device, event_t event_types);
    template <typename _PeriDevice>
    epoll(_PeriDevice& device, const etl::vector<event_t, 15>& event_types);
    ~epoll();

    template <typename _Rep, typename _Period, std::enable_if_t<
        (_Period::num == 1 && _Period::den <= 1000) ||
        (_Period::num >= 1 && _Period::den == 1)
    >* = nullptr>
    int wait(int max_events, const std::chrono::duration<_Rep, _Period>& timeout);
    int wait(int max_events);

private:
    void _create_epoll_fd(event_t event_types);

private:
    struct epoll_event _context;
    struct epoll_event _events;
    file_descriptor_t _device_fd;
    file_descriptor_t _epoll_fd;
};

template <typename _PeriDevice>
epoll::epoll(_PeriDevice& device, event_t event_types) :
    _device_fd(device.fd())
{
    std::memset(&this->_context, 0, sizeof(this->_context));
    std::memset(&this->_events, 0, sizeof(this->_events));
    this->_create_epoll_fd(event_types);
}

template <typename _PeriDevice>
epoll::epoll(_PeriDevice& device, const etl::vector<event_t, 15>& event_types) :
    _device_fd(device.fd())
{
    std::memset(&this->_context, 0, sizeof(this->_context));
    std::memset(&this->_events, 0, sizeof(this->_events));

    event_t real_event_types = 0;
    for (auto event_type : event_types)
        real_event_types |= event_type;
    this->_create_epoll_fd(real_event_types);
}

epoll::~epoll()
{
    ::epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL,
        this->_device_fd, &this->_events);
    ::close(this->_epoll_fd);
}

/**
 * @author Jin
 * @brief wait(asynchronous) for events and get events
 * @param timeout milliseconds
 */
template <typename _Rep, typename _Period, std::enable_if_t<
    (_Period::num == 1 && _Period::den <= 1000) ||
    (_Period::num >= 1 && _Period::den == 1)
    >* = nullptr>
int epoll::wait(int max_events, const std::chrono::duration<_Rep, _Period>& timeout)
{
    int millisec = 0;
#if __cplusplus >= 201703L
    if constexpr (std::is_same<decltype(timeout), std::chrono::milliseconds>::value)
    {
        millisec = timeout.count();
    }
    else
    {

        millisec = 
            std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
    }
#else
    millisec = 
        std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
#endif
    
    auto num_events =
        ::epoll_wait(this->_epoll_fd, &this->_events, max_events, millisec);

    return num_events;
}

/**
 * @author Jin
 * @brief wait(asynchronous) for events and get events
 */
int epoll::wait(int max_events)
{
    auto num_events =
        ::epoll_wait(this->_epoll_fd, &this->_events, max_events, -1/* forever */);
    return num_events;
}

/**
 * @author Jin
 * @brief create and register my epoll fd in my my my system
 */
void epoll::_create_epoll_fd(event_t event_types)
{
    this->_epoll_fd = ::epoll_create(1);
    this->_context.events = event_types;
    this->_context.data.fd = this->_device_fd;
    ::epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD,
        this->_device_fd, &this->_events);
    
}



}

}


#endif