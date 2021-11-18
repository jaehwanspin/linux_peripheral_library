#ifndef __LPL_EPOLL_EPOLL_HPP__
#define __LPL_EPOLL_EPOLL_HPP__
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

#include <sys/epoll.h>

#include "lpl/common.hpp"


namespace lpl
{

namespace epoll
{

/**
 * @author Jin
 * @brief  Epoll wrapper
 * for asynchronous event polling
 */
class epoll
{

public:
    explicit epoll(file_descriptor_t device_fd);
    ~epoll();

    int wait();

private:
    void _create_epoll_fd();

private:
    struct epoll_event _context;
    struct epoll_event _events;
    file_descriptor_t _device_fd;
    file_descriptor_t _epoll_fd;
};


epoll::epoll(file_descriptor_t device_fd) :
    _device_fd(device_fd)
{
    std::memset(&this->_context, 0, sizeof(this->_context));
    std::memset(&this->_events, 0, sizeof(this->_events));
    this->_create_epoll_fd();
}

epoll::~epoll()
{
    ::epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL,
        this->_device_fd, &this->_events);
    ::close(this->_epoll_fd);
}

/**
 * @author Jin
 * @brief wait(asynchronous) and get events
 */
int epoll::wait()
{
    ch8_t<256> buf = { 0, };
    auto num_events =
        ::epoll_wait(this->_epoll_fd, &this->_events, 1, -1);
    if (num_events > 0)
    {
        auto a = ::lseek64(this->_device_fd, 0, SEEK_SET);
        auto b = ::read(this->_device_fd, buf, sizeof(buf));
        std::cout << buf << std::endl;
    }

    return num_events;
}

/**
 * @author Jin
 * @brief create and register my epoll fd on my my my system
 */
void epoll::_create_epoll_fd()
{
    this->_epoll_fd = ::epoll_create(1);
    this->_context.events = EPOLLPRI;
    this->_context.data.fd = this->_device_fd;
    ::epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD,
        this->_device_fd, &this->_events);
}



}

}


#endif