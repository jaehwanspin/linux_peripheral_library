#ifndef __LPL_IO_CONTEXT_HPP__
#define __LPL_IO_CONTEXT_HPP__

#include <string>
// #include <filesystem>
#include <boost/filesystem.hpp>

namespace lpl
{

/**
 * @author Jin
 * @brief saves all linux peripheral paths
 */
class io_context
{
    using path = boost::filesystem::path;
    using dir_iter = boost::filesystem::directory_iterator;

public:
    io_context()
    {
        // path p(__peri_path);
        // dir_iter peri_dir_iter(p);
    }

private:

};

}

#endif