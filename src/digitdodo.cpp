#include "digitdodo.h"
#include "digitdodo_defines.h"

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
        m_display_values[group.name] = "";
        m_group_visibility[group.name] = true;
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

bool digitdodo::update_display(const std::string& t_group_name)
{
    int group_index = get_group_index(t_group_name);
    int group_size = m_groups[group_index].length;
    std::vector<unsigned char> raw_buffer;
    raw_buffer.reserve(group_size);

    for (size_t i = 0, digit_count = 0; i < m_display_values[t_group_name].size() && digit_count < group_size; ++i)
    {
        unsigned char c = m_display_values[t_group_name][i];
        if (!isCharValid(m_groups[group_index].type[digit_count], c) || c == '.')
        {
            continue;
        }
            

        unsigned char seg = getCharSegments(m_groups[group_index].type[digit_count], c);

        // Check if next character is a dot
        if (i + 1 < m_display_values[t_group_name].size() && m_display_values[t_group_name][i + 1] == '.')
        {
            seg |= 0x01;
        }

        raw_buffer.push_back(seg);
        ++digit_count;
    }

    // Pad with zeros if not enough digits
    while (raw_buffer.size() < group_size)
        raw_buffer.push_back(0);

    digitdodo_platform::updateRawBuffer(t_group_name, raw_buffer);

    return true;
}

bool digitdodo::update_display_value(const std::string& t_group_name, const std::string& t_value, bool t_update_raw)
{
    log_digitdodo("Updating display value for group %s to %s\n", t_group_name.c_str(), t_value.c_str());
    
    int group_index = get_group_index(t_group_name);
    
    if (group_index == -1)
        return false;

    m_display_values[t_group_name] = t_value;

    if(t_update_raw)
    {
        update_display(t_group_name);
    }

    return true;
}

bool isCharValid(digitdodo_platform::SegmentDisplayType t_type, unsigned char t_char)
{
    bool valid = false;

    if(t_type == digitdodo_platform::SegmentDisplayType::SEG_7)
    {
        valid = (t_char >= '0' && t_char <= '9') || (t_char == '.');
    }

    return valid;
}

unsigned char getCharSegments(digitdodo_platform::SegmentDisplayType t_type, unsigned char t_char)
{
    unsigned char segments = 0;

    if(t_type == digitdodo_platform::SegmentDisplayType::SEG_7)
    {
        if(t_char >= '0' && t_char <= '9')
        {
            segments = sevenSegmentDigitMap[t_char - '0'];
        }
        else if(t_char == ' ')
        {
            segments = sevenSegmentCharMap[' '];
        }
    }
    
    return segments;
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
    log_digitdodo("Updating display mode for group %s to %d with param %u\n", t_group_name.c_str(), static_cast<int>(t_mode), t_param);

    switch (t_mode)
    {
        case SevenSegmentDisplayMode::Normal:
            // No periodic updates needed
            break;
        case SevenSegmentDisplayMode::Blink:
            set_blink_mode(t_group_name, t_param);
            break;
        case SevenSegmentDisplayMode::Scroll:
            // Implement scroll logic here
            break;
        default:
            break;
    }

    return true;
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
#ifdef LOG_DIGITDODO
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
#endif
 }
