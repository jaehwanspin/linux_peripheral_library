#ifndef __LPL_GENERIC_LED_HPP__
#define __LPL_GENERIC_LED_HPP__

#include <condition_variable>
#include <thread>
#include <chrono>
#include <type_traits>
#include <mutex>
#include <atomic>

#include "../lpl_traits.hpp"
#include "../common.hpp"
#include "../peripheral/pwm.hpp"

namespace lpl
{
namespace generic
{

template <typename _PWMDevice, typename _Kind>
class led;

template <typename _PWMDevice>
class led<_PWMDevice, lpl_traits::gpio_traits>
{
    static constexpr int ah = _PWMDevice::____device_type;
public:
    template <typename _Rep = int64_t, typename _Period = std::milli>
    explicit led(int gpio, uint64_t period = 1'000, lpl::led::brightness_t max_brightness = 100, uint64_t duty_per_count = 1,
        const std::chrono::duration<_Rep, _Period>& dim_time =
            std::chrono::duration<_Rep, _Period>(100)) :
        _dev(gpio, period, 0), _duty_per_count(duty_per_count),
        _dim_time_milli(std::chrono::duration_cast<std::chrono::milliseconds>(dim_time).count()),
        _max_brightness(max_brightness),
        _brightness(0),
        _dim_th(&led::_dim_handler, this)
    {
        this->_dev.set_enable(true);
    }

    ~led()
    {
        this->_cv.~condition_variable();
        if (this->_dim_th.joinable())
            this->_dim_th.join();
    }

    
    void set_brightness(lpl::led::brightness_t brightness)
    {
        this->_brightness = brightness;
        this->_cv.notify_one();
    }

private:
    void _dim_handler()
    {
        {
            std::unique_lock<std::mutex> lk(this->_mtx);
            this->_cv.wait(lk, [&]() {
                return this->_do_set.load();
            });
        }
        this->_do_set.store(false);

        auto now = std::chrono::high_resolution_clock::now();

        lpl::led::brightness_t cur_brightness =
            (this->_dev.duty_cycle() * this->_max_brightness) / this->_dev.period();

        while (cur_brightness != this->_brightness)
        {
            this->_dev.set_duty_cycle(
                this->_dev.duty_cycle() +
                    (cur_brightness < this->_brightness ? this->_duty_per_count : (-1 * this->_duty_per_count))
            );

            // now += std::chrono::milliseconds(
            //      / this->_dim_time_milli
            //     );
            std::this_thread::sleep_until(now);
            cur_brightness =
                (this->_dev.period() * this->_max_brightness) / this->_dev.duty_cycle();   
        }
    }

private:
    _PWMDevice _dev;
    lpl::led::brightness_t _brightness;
    lpl::led::brightness_t _max_brightness;
    uint64_t _duty_per_count;
    uint32_t _dim_time_milli;
    std::atomic_bool _do_set;
    std::mutex _mtx;
    std::condition_variable _cv;
    std::thread _dim_th;
};

template <typename _PWMDevice>
class led<_PWMDevice, lpl_traits::pwm_traits>
{
public:
    template <typename _Rep = int64_t, typename _Period = std::milli>
    explicit led(int pwm_chip_num, int pwm_num, uint64_t period = 1'000'000, lpl::led::brightness_t max_brightness = 100,
        uint64_t duty_per_count = 1, const std::chrono::duration<_Rep, _Period>& dim_time =
            std::chrono::duration<_Rep, _Period>(100)) :
        _dev(pwm_chip_num, pwm_num, period, 0), _duty_per_count(duty_per_count),
        _dim_time_milli(std::chrono::duration_cast<std::chrono::milliseconds>(dim_time).count()),
        _max_brightness(max_brightness),
        _brightness(0),
        _dim_th(&led::_dim_handler, this)
    {
        this->_dev.set_enable(true);
    }

    ~led()
    {
        this->_cv.~condition_variable();
        if (this->_dim_th.joinable())
            this->_dim_th.join();
    }

    void set_brightness(lpl::led::brightness_t brightness)
    {
        this->_brightness = brightness;
        this->_cv.notify_one();
    }

private:
    void _dim_handler()
    {

    }

private:
    _PWMDevice _dev;
    lpl::led::brightness_t _brightness;
    lpl::led::brightness_t _max_brightness;
    uint64_t _duty_per_count;
    uint32_t _dim_time_milli;
    std::atomic_bool _do_set;
    std::mutex _mtx;
    std::condition_variable _cv;
    std::thread _dim_th;
};

}    
}


#endif 