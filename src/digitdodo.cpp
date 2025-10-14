#include "digitdodo.h"
#include "baseDigitDisplay.h"
#include "sevenDigitDisplay.h"
#include "fourteenDigitDisplay.h"

#include <cstdio>

digitdodo& digitdodo::getInstance()
{
    static digitdodo instance;
    return instance;
}

digitdodo::digitdodo(): m_groups(digitdodo_platform::getGroups())
{
    for (const auto& group : m_groups) {
        m_group_params[group.name] = {{-1}, std::string(group.length , ' '), true, 0, SevenSegmentDisplayMode::Normal, nullptr};

        switch (group.type)
        {
            case digitdodo_platform::SegmentDisplayType::SEG_7:
                m_group_params[group.name].display_type = new SevenDigitDisplay();
                break;
            case digitdodo_platform::SegmentDisplayType::SEG_14:
                m_group_params[group.name].display_type = new FourteenDigitDisplay();
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
    std::vector<unsigned char> raw_buffer = m_group_params[t_group_name].display_type->getRawBuffer(t_value, m_groups[get_group_index(t_group_name)].length);

    digitdodo_platform::updateRawBuffer(t_group_name, raw_buffer);

    return true;
}

bool digitdodo::update_display_value(const std::string& t_group_name, const std::string t_value, bool t_update_raw)
{
    log_digitdodo("Updating display value for group %s to %s\n", t_group_name.c_str(), t_value.c_str());

    int group_index = get_group_index(t_group_name);
    
    if (group_index == -1)
        return false;

    m_group_params[t_group_name].display_value = "";


    for (char c : t_value)
    {
        if(m_group_params[t_group_name].display_type->isCharValid(c))
        {
            m_group_params[t_group_name].display_value += c;
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

    std::string& original_value = m_group_params[group_name].display_value;

    original_value += std::string(group_size, ' ');
}

void digitdodo::unpad_display_value(unsigned int t_group_index)
{
    const std::string& group_name = m_groups[t_group_index].name;
    const int group_size = m_groups[t_group_index].length;

    std::string& original_value = m_group_params[group_name].display_value;

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

    if(m_group_params[t_group_name].mode == SevenSegmentDisplayMode::Scroll)
    {
        unpad_display_value(group_index);
    }

    update_display_mode(t_group_name, t_mode, t_param);

    m_group_params[t_group_name].mode = t_mode;
    return true;
}

void digitdodo::cancel_group_handle(const std::string& t_group_name)
{
    log_digitdodo("Cancelling group handle for group %s\n", t_group_name.c_str());

    if (m_group_params[t_group_name].handle.idx != -1)
    {
        TickToucan& tt = TickToucan::instance();
        tt.cancel(m_group_params[t_group_name].handle);
        m_group_params[t_group_name].handle = {-1};
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
    
    m_group_params[t_group_name].visible = true; // Ensure visible state starts as true

    TickToucan& tt = TickToucan::instance();

    // Use the key from m_display_values to get a stable pointer
    const char* ctx = m_group_params.find(t_group_name)->first.c_str();

    TickToucan::Handle h = tt.scheduleEvery(t_toggle_ms, &digitdodo::toggle_blink, (void*)ctx);
    
    m_group_params[t_group_name].handle = h;

    if(h.idx == -1)
        return false;

    return true;
}

void digitdodo::turn_off_group(const std::string& t_group_name)
{
    log_digitdodo("Turning off group: %s\n", t_group_name.c_str());

    for(auto& group : m_groups)
    {
        if(group.name == t_group_name)
        {
            std::vector<unsigned char> raw_buffer = m_group_params[t_group_name].display_type->getRawBuffer(std::string(group.length , ' '), group.length);
            
            digitdodo_platform::updateRawBuffer(t_group_name, raw_buffer);
            break;
        }
    }

}

void digitdodo::turn_on_group(const std::string& t_group_name)
{
    log_digitdodo("Turning off group: %s\n", t_group_name.c_str());

    for(auto& group : m_groups)
    {
        if(group.name == t_group_name)
        {
            update_display_value(t_group_name, m_group_params[t_group_name].display_value, true);
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
    if(m_group_params.find(t_group_name) == m_group_params.end())
        return;

    m_group_params[t_group_name].scroll_direction = t_direction;

}

bool digitdodo::set_scroll_mode(const std::string& t_group_name, unsigned int t_speed_ms)
{
    log_digitdodo("Setting scroll mode for group %s with scroll speed %u ms\n", t_group_name.c_str(), t_speed_ms);
    
    m_group_params[t_group_name].scroll_pos = 0;    // Reset scroll position

    int group_index = get_group_index(t_group_name);

    if (group_index == -1)
        return false;

    pad_display_value(group_index);

    TickToucan& tt = TickToucan::instance();

    // Use the key from m_display_values to get a stable pointer
    const char* ctx = m_group_params.find(t_group_name)->first.c_str();

    TickToucan::Handle h = tt.scheduleEvery(t_speed_ms, &digitdodo::scroll_text, (void*)ctx);
    
    m_group_params[t_group_name].handle = h;

    if(h.idx == -1)
        return false;

    return true;
}

bool digitdodo::get_group_visibility(const std::string& t_group_name)
{
    if(m_group_params.find(t_group_name) == m_group_params.end())
        return false;
    return m_group_params[t_group_name].visible;
}

void digitdodo::set_group_visibility(const std::string& t_group_name, bool t_visible)
{
    if(m_group_params.find(t_group_name) == m_group_params.end())
        return;
    m_group_params[t_group_name].visible = t_visible;
}

void digitdodo::toggle_blink(void* ctx)
{
    std::string group_name = std::string((const char*) ctx);
    digitdodo& dd = digitdodo::getInstance();

    bool visible = dd.get_group_visibility(group_name);

    if (visible)
    {
        dd.turn_off_group(group_name);
    }
    else
    {
        dd.turn_on_group(group_name);
    }

    dd.set_group_visibility(group_name, !visible);
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
    return m_group_params[group_name].display_type->populate_scrolled_value(m_group_params[group_name].display_value, m_groups[get_group_index(group_name)].length, m_group_params[group_name].scroll_direction, m_group_params[group_name].scroll_pos);
}

bool digitdodo::update_display(const std::string& t_group_name)
{
    if(m_group_params.find(t_group_name) == m_group_params.end())
        return false;
    return update_segment_output(t_group_name, m_group_params[t_group_name].display_value);
}

    


