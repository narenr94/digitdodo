#pragma once

#include "digitdodo_platform.h"
#include "ticktoucan.h"

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdarg>
#include <cstdio>


enum class SevenSegmentDisplayMode { Normal, Blink, Scroll };


class digitdodo{

    std::vector<digitdodo_platform::HardwareGroup>& m_groups;

    std::vector<SevenSegmentDisplayMode> m_display_mode;

    std::unordered_map<std::string, TickToucan::Handle> m_group_handles;

    std::unordered_map<std::string, std::string> m_display_values;

    std::unordered_map<std::string, bool> m_group_visibility;


    digitdodo();

    void cancel_group_handle(const std::string& t_group_name);

    bool update_display_mode(const std::string& t_group_name, SevenSegmentDisplayMode t_mode, unsigned int t_param);

    bool set_blink_mode(const std::string& t_group_name, unsigned int t_toggle_ms);

    static void toggle_blink(void* ctx);

    int get_group_index(const std::string& t_group_name);

    void log_digitdodo(const char* format, ...);

    public:

    static digitdodo& getInstance();    

    bool update_display_value(const std::string& t_group_name, const std::string& t_value, bool t_update_raw = false);

    bool update_display(const std::string& t_group_name);

    bool set_display_mode(const std::string& t_group_name, SevenSegmentDisplayMode t_mode, unsigned int t_param);

    std::unordered_map<std::string, std::string>& get_display_values();

    void turn_off_group(const std::string& t_group_name);

};