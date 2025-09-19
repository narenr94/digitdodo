#include "digit_dodo.h"
#include "digit_dodo_defines.h"

bool isCharValid(digit_dodo_platform::SegmentDisplayType t_type, unsigned char t_char);

unsigned char getCharSegments(digit_dodo_platform::SegmentDisplayType t_type, unsigned char t_char);

digit_dodo& digit_dodo::getInstance()
{
    static digit_dodo instance;
    return instance;
}

digit_dodo::digit_dodo(): m_groups(digit_dodo_platform::getGroups())
{
    m_display_mode.resize(m_groups.size(), SevenSegmentDisplayMode::Normal);
}

bool digit_dodo::update_display_value(const std::string& t_group_name, const std::string& t_value)
{
    bool group_found = false;
    int group_size = 0;
    int group_index = -1;

    for(int i = 0; i < m_groups.size(); ++i)    
    {
        if(m_groups[i].name == t_group_name)
        {
            group_found = true;
            group_size = m_groups[i].length;
            group_index = i;
            break;
        }
    }

    if(!group_found)
        return group_found;

    std::vector<unsigned char> raw_buffer(group_size, 0);
    unsigned char temp_buffer = 0;

    int digit_count = 0;

    for(int i = 0; i < t_value.size() && digit_count < group_size; ++i)
    {
        if(t_value[i] == '.') // Skip dots, they are handled with the previous character
            continue;

        if(isCharValid(m_groups[group_index].type[digit_count], t_value[i]))
        {
            temp_buffer = getCharSegments(m_groups[group_index].type[digit_count], t_value[i]);
        }
        else
        {
            temp_buffer = 0; // Invalid character, turn off segments
        }

        if(i < t_value.size() - 1 && t_value[i + 1] == '.')
        {
            temp_buffer |= 0b00000001; // Set DOT segment
        }

        raw_buffer[digit_count] = temp_buffer;

        digit_count++;
    }

    digit_dodo_platform::updateRawBuffer(t_group_name, raw_buffer);

    return group_found;
}

bool isCharValid(digit_dodo_platform::SegmentDisplayType t_type, unsigned char t_char)
{
    bool valid = false;

    if(t_type == digit_dodo_platform::SegmentDisplayType::SEG_7)
    {
        valid = (t_char >= '0' && t_char <= '9') || (t_char == '.');
    }

    return valid;
}

unsigned char getCharSegments(digit_dodo_platform::SegmentDisplayType t_type, unsigned char t_char)
{
    unsigned char segments = 0;

    if(t_type == digit_dodo_platform::SegmentDisplayType::SEG_7)
    {
        if(t_char >= '0' && t_char <= '9')
        {
            segments = sevenSegmentDigitMap[t_char - '0'];
        }
    }
    
    return segments;
}
