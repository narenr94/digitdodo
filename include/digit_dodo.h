#pragma once

#include "digit_dodo_platform.h"

#include <string>
#include <memory>
#include <vector>


enum class SevenSegmentDisplayMode { Normal, Blink, Scroll };


class digit_dodo{

    std::vector<digit_dodo_platform::HardwareGroup>& m_groups;

    std::vector<SevenSegmentDisplayMode> m_display_mode;

    digit_dodo();

    public:

    static digit_dodo& getInstance();    

    bool update_display_value(const std::string& t_group_name, const std::string& t_value);

};