#include "digitdodo.h"
#include "baseDigitDisplay.h"
#include "sevenDigitDisplay.h"
#include "fourteenDigitDisplay.h"

#include <cstdio>

bool isCharValid(digitdodo_platform::SegmentDisplayType t_type, unsigned char t_char);

unsigned char getCharSegments(digitdodo_platform::SegmentDisplayType t_type, unsigned char t_char);

digitdodo& digitdodo::getInstance()
{
    static digitdodo instance;
    return instance;
}

digitdodo::digitdodo(): m_groups(digitdodo_platform::getGroups())
{
    m_display_mode.resize(m_groups.size(), SevenSegmentDisplayMode::Normal);

    for (const auto& group : m_groups) {
        m_group_handles[group.name] = {-1};
        m_display_values[group.name] = std::string(m_groups.size(), ' ');
        m_group_visibility[group.name] = true;
        m_group_scroll_direction[group.name] = digitdodo::ScrollDirection::RightToLeft; // true = right to left
        m_group_scroll_position[group.name] = 0;
        switch (group.type)
        {
            case digitdodo_platform::SegmentDisplayType::SEG_7:
                m_display_types[group.name] = new SevenDigitDisplay();
                break;
            case digitdodo_platform::SegmentDisplayType::SEG_14:
                m_display_types[group.name] = new FourteenDigitDisplay();
                break;
            
            default:
                break;
        }

    }

}

int digitdodo::get_group_index(const std::string& t_group_name)
{
    for (int i = 0; i < m_groups.size(); ++i)
    {
        if (m_groups[i].name == t_group_name)
        {
            return i;
        }
    }
    return -1;
}

bool digitdodo::update_segment_output(const std::string& t_group_name, const std::string& t_value)
{    
    std::vector<unsigned char> raw_buffer = m_display_types[t_group_name]->getRawBuffer(t_value, m_groups[get_group_index(t_group_name)].length);

    digitdodo_platform::updateRawBuffer(t_group_name, raw_buffer);

    return true;
}

bool digitdodo::update_display_value(const std::string& t_group_name, const std::string& t_value, bool t_update_raw)
{
    log_digitdodo("Updating display value for group %s to %s\n", t_group_name.c_str(), t_value.c_str());
    
    int group_index = get_group_index(t_group_name);
    
    if (group_index == -1)
        return false;

    m_display_values[t_group_name] = "";

    for (char c : t_value)
    {
        if (m_display_types[t_group_name]->isCharValid(c))
        {
            m_display_values[t_group_name] += c;
        }
    }

    if(t_update_raw)
    {
        update_segment_output(t_group_name, t_value);
    }

    return true;
}

void digitdodo::pad_display_value(unsigned int t_group_index)
{
    const std::string& group_name = m_groups[t_group_index].name;
    const int group_size = m_groups[t_group_index].length;

    std::string& original_value = m_display_values[group_name];

    original_value += std::string(group_size, ' ');
}

void digitdodo::unpad_display_value(unsigned int t_group_index)
{
    const std::string& group_name = m_groups[t_group_index].name;
    const int group_size = m_groups[t_group_index].length;

    std::string& original_value = m_display_values[group_name];

    original_value = original_value.substr(0, original_value.size() - group_size);
}

bool digitdodo::set_display_mode(const std::string& t_group_name, SevenSegmentDisplayMode t_mode, unsigned int t_param)
{
    log_digitdodo("Setting display mode for group %s to %d with param %u\n", t_group_name.c_str(), static_cast<int>(t_mode), t_param);

    int group_index = -1;
    for (int i = 0; i < m_groups.size(); ++i)
    {
        if (m_groups[i].name == t_group_name)
        {
            group_index = i;
            break;
        }
    }

    if (group_index == -1)
        return false;

    cancel_group_handle(t_group_name);

    if(m_display_mode[group_index] == SevenSegmentDisplayMode::Scroll)
    {
        unpad_display_value(group_index);
    }

    update_display_mode(t_group_name, t_mode, t_param);

    m_display_mode[group_index] = t_mode;
    return true;
}

void digitdodo::cancel_group_handle(const std::string& t_group_name)
{
    log_digitdodo("Cancelling group handle for group %s\n", t_group_name.c_str());

    auto it = m_group_handles.find(t_group_name);
    if (it != m_group_handles.end() && it->second.idx != -1)
    {
        TickToucan& tt = TickToucan::instance();
        tt.cancel(it->second);
        it->second = {-1};
    }
}

bool digitdodo::update_display_mode(const std::string& t_group_name, SevenSegmentDisplayMode t_mode, unsigned int t_param)
{
    bool ret = false;
    log_digitdodo("Updating display mode for group %s to %d with param %u\n", t_group_name.c_str(), static_cast<int>(t_mode), t_param);

    switch (t_mode)
    {
        case SevenSegmentDisplayMode::Normal:
            // No periodic updates needed
            break;
        case SevenSegmentDisplayMode::Blink:
            ret = set_blink_mode(t_group_name, t_param);
            break;
        case SevenSegmentDisplayMode::Scroll:
            ret = set_scroll_mode(t_group_name, t_param);
            break;
        default:
            break;
    }

    return ret;
}

bool digitdodo::set_blink_mode(const std::string& t_group_name, unsigned int t_toggle_ms)
{
    log_digitdodo("Setting blink mode for group %s with toggle %u ms\n", t_group_name.c_str(), t_toggle_ms);
    
    TickToucan& tt = TickToucan::instance();

    // Use the key from m_display_values to get a stable pointer
    const char* ctx = m_display_values.find(t_group_name)->first.c_str();

    TickToucan::Handle h = tt.scheduleEvery(t_toggle_ms, &digitdodo::toggle_blink, (void*)ctx);
    
    m_group_handles[t_group_name] = h;

    if(h.idx == -1)
        return false;

    return true;
}

std::unordered_map<std::string, std::string>& digitdodo::get_display_values()
{
    return m_display_values;
}

void digitdodo::turn_off_group(const std::string& t_group_name)
{
    log_digitdodo("Turning off group: %s\n", t_group_name.c_str());

    for(auto& group : m_groups)
    {
        if(group.name == t_group_name)
        {
            std::vector<unsigned char> raw_buffer(group.length, 0);
            digitdodo_platform::updateRawBuffer(t_group_name, raw_buffer);
            break;
        }
    }

}

void digitdodo::log_digitdodo(const char* format, ...)
{
#ifdef ENABLE_LOG
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
#endif
}

void digitdodo::set_scroll_direction(const std::string& t_group_name, ScrollDirection t_direction)
{
    if (m_group_scroll_direction.find(t_group_name) != m_group_scroll_direction.end())
    {
        m_group_scroll_direction[t_group_name] = t_direction;
    }

}

bool digitdodo::set_scroll_mode(const std::string& t_group_name, unsigned int t_speed_ms)
{
    log_digitdodo("Setting scroll mode for group %s with scroll speed %u ms\n", t_group_name.c_str(), t_speed_ms);
    
    m_group_scroll_position[t_group_name] = 0; // Reset scroll position

    int group_index = get_group_index(t_group_name);

    if (group_index == -1)
        return false;

    pad_display_value(group_index);

    TickToucan& tt = TickToucan::instance();

    // Use the key from m_display_values to get a stable pointer
    const char* ctx = m_display_values.find(t_group_name)->first.c_str();

    TickToucan::Handle h = tt.scheduleEvery(t_speed_ms, &digitdodo::scroll_text, (void*)ctx);
    
    m_group_handles[t_group_name] = h;

    if(h.idx == -1)
        return false;

    return true;
}

void digitdodo::toggle_blink(void* ctx)
{
    std::string group_name = std::string((const char*) ctx);
    digitdodo& dd = digitdodo::getInstance();

    bool& visible = dd.m_group_visibility[group_name];

    if (visible)
    {
        dd.turn_off_group(group_name);
    }
    else
    {
        dd.update_display_value(group_name, dd.get_display_values()[group_name], true);
    }

    visible = !visible;
}

void digitdodo::scroll_text(void* ctx)
{
    std::string group_name = std::string((const char*) ctx);
    digitdodo& dd = digitdodo::getInstance();

    std::string scrolled_value = dd.populate_scrolled_value(group_name);

    dd.update_segment_output(group_name, scrolled_value);

}

std::string digitdodo::populate_scrolled_value(std::string& group_name)
{
    return m_display_types[group_name]->populate_scrolled_value(m_display_values[group_name], m_groups[get_group_index(group_name)].length, m_group_scroll_direction[group_name], m_group_scroll_position[group_name]);
}




bool digitdodo::update_display(const std::string& t_group_name)
{
    if(m_display_values.find(t_group_name) == m_display_values.end())
        return false;
    return update_segment_output(t_group_name, m_display_values[t_group_name]);
}

    


