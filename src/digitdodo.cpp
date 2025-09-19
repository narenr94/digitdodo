#include "digitdodo.h"
#include "digitdodo_defines.h"

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
}

bool digitdodo::update_display_value(const std::string& t_group_name, const std::string& t_value)
{
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

    int group_size = m_groups[group_index].length;
    std::vector<unsigned char> raw_buffer;
    raw_buffer.reserve(group_size);

    for (size_t i = 0, digit_count = 0; i < t_value.size() && digit_count < group_size; ++i)
    {
        unsigned char c = t_value[i];
        if (!isCharValid(m_groups[group_index].type[digit_count], c) || c == '.')
            continue;

        unsigned char seg = getCharSegments(m_groups[group_index].type[digit_count], c);

        // Check if next character is a dot
        if (i + 1 < t_value.size() && t_value[i + 1] == '.')
            seg |= 0x01;

        raw_buffer.push_back(seg);
        ++digit_count;
    }

    // Pad with zeros if not enough digits
    while (raw_buffer.size() < group_size)
        raw_buffer.push_back(0);

    digitdodo_platform::updateRawBuffer(t_group_name, raw_buffer);
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
    }
    
    return segments;
}
