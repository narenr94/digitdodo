#pragma once

#include "I7SegmentPlatform.hpp"

#include <string>
#include <memory>
#include <vector>


enum class SevenSegmentDisplayMode { Normal, Blink, Scroll };


class digit_dodo{

    I7SegmentPlatform* m_platform_impl;

    std::vector<SevenSegmentDisplayMode> m_display_mode;

    public:

    digit_dodo(I7SegmentPlatform* t_platform_impl);

    void update_display_value(const std::string& t_group_name, const std::string& t_value);

    void set_mode_normal();
    
    void set_mode_blink(unsigned int t_on_ticks, unsigned int t_off_ticks);

    void set_mode_scroll(unsigned int t_scroll_speed);



};