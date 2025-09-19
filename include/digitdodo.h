#pragma once

#include "digitdodo_platform.h"
#include "ticktoucan.h"

#include <string>
#include <memory>
#include <vector>


enum class SevenSegmentDisplayMode { Normal, Blink, Scroll };


class digitdodo{

    std::vector<digitdodo_platform::HardwareGroup>& m_groups;

    std::vector<SevenSegmentDisplayMode> m_display_mode;

    digitdodo();

    public:

    static digitdodo& getInstance();    

    bool update_display_value(const std::string& t_group_name, const std::string& t_value);

};