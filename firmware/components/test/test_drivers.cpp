#include "unity.h"
#include "driver/gpio.h"
#include "gpio_driver.hpp"
#include "led_driver.hpp"
#include "button_handler.hpp"

TEST_CASE("LedDriver initialization", "[led]")
{
    LedDriver led(GPIO_NUM_8);
    TEST_ASSERT_EQUAL(8, led.get_pin());
}

TEST_CASE("LedDriver on/off state", "[led]")
{
    LedDriver led(GPIO_NUM_8);
    TEST_ASSERT_FALSE(led.get_state());
}

TEST_CASE("LedDriver toggle changes state", "[led]")
{
    LedDriver led(GPIO_NUM_8);
    led.off();
    TEST_ASSERT_FALSE(led.get_state());
    
    led.toggle();
    TEST_ASSERT_TRUE(led.get_state());
    
    led.toggle();
    TEST_ASSERT_FALSE(led.get_state());
}

TEST_CASE("GpioDriver get_pin returns correct pin", "[gpio]")
{
    GpioDriver gpio(GPIO_NUM_9, GPIO_MODE_INPUT);
    TEST_ASSERT_EQUAL(9, gpio.get_pin());
}

TEST_CASE("ButtonHandler initialization", "[button]")
{
    ButtonHandler button(GPIO_NUM_9, 200000);
    TEST_ASSERT_FALSE(button.check_pressed());
}
