#include "sevenDigitDisplay.h"
#include "digitdodo_platform.h"

#include <unordered_map>

#define DIGITDODO_SEG7_A    0x01
#define DIGITDODO_SEG7_B    0x02
#define DIGITDODO_SEG7_C    0x04
#define DIGITDODO_SEG7_D    0x08
#define DIGITDODO_SEG7_E    0x10
#define DIGITDODO_SEG7_F    0x20
#define DIGITDODO_SEG7_G    0x40
#define DIGITDODO_SEG7_DP   0x80

static const std::unordered_map<unsigned char, unsigned char> sevenSegmentCharMap = {
    // digits
    {'0', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F},
    {'1', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C},
    {'2', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_G},
    {'3', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_G},
    {'4', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'5', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'6', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'7', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C},
    {'8', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'9', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},

    // basic symbols
    {' ', (unsigned char) 0x00},
    {'.', (unsigned char) DIGITDODO_SEG7_DP},
    {'-', (unsigned char) DIGITDODO_SEG7_G},
    {'_', (unsigned char) DIGITDODO_SEG7_D},   
    {'=', (unsigned char) DIGITDODO_SEG7_D | DIGITDODO_SEG7_G},

    // uppercase letters (best 7-seg approximations)
    {'A', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'B', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'C', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F},
    {'D', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F},
    {'E', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'F', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'G', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'H', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'I', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C},
    {'J', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D},
    {'K', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'L', (unsigned char) DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F},
    {'M', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'N', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'O', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F},
    {'P', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'Q', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'R', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'S', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'T', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C},
    {'U', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F},
    {'V', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F},
    {'W', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F},
    {'X', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'Y', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_D | DIGITDODO_SEG7_F},
    {'Z', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_G},

    // lowercase letters (approximations where meaningful)
    {'a', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_G},
    {'b', (unsigned char) DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'c', (unsigned char) DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_G},
    {'d', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_G},
    {'e', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'f', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'g', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'h', (unsigned char) DIGITDODO_SEG7_C | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'i', (unsigned char) DIGITDODO_SEG7_E},
    {'j', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D},
    {'k', (unsigned char) DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'l', (unsigned char) DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F},
    {'m', (unsigned char) DIGITDODO_SEG7_C | DIGITDODO_SEG7_E | DIGITDODO_SEG7_G},
    {'n', (unsigned char) DIGITDODO_SEG7_C | DIGITDODO_SEG7_E | DIGITDODO_SEG7_G},
    {'o', (unsigned char) DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_G},
    {'p', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'q', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_C | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'r', (unsigned char) DIGITDODO_SEG7_E | DIGITDODO_SEG7_G},
    {'s', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'t', (unsigned char) DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'u', (unsigned char) DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E},
    {'v', (unsigned char) DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E},
    {'w', (unsigned char) DIGITDODO_SEG7_C | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E},
    {'x', (unsigned char) DIGITDODO_SEG7_C | DIGITDODO_SEG7_E},
    {'y', (unsigned char) DIGITDODO_SEG7_B | DIGITDODO_SEG7_F | DIGITDODO_SEG7_G},
    {'z', (unsigned char) DIGITDODO_SEG7_A | DIGITDODO_SEG7_B | DIGITDODO_SEG7_D | DIGITDODO_SEG7_E | DIGITDODO_SEG7_G}
};

SevenDigitDisplay::SevenDigitDisplay()
{
    dp_char = getCharSegments('.');
}

std::vector<unsigned char> SevenDigitDisplay::getRawBuffer(const std::string& t_value, int group_size)
{
    std::vector<unsigned char> raw_buffer;
    raw_buffer.reserve(group_size);

    for (size_t i = 0, digit_count = 0; i < t_value.size() && digit_count < group_size; ++i)
    {
        unsigned char c = t_value[i];
        unsigned char seg = getCharSegments(c);

        if (seg == dp_char)
        {
            continue;
        }        

        // Check if next character is a dot
        if (i + 1 < t_value.size() && (getCharSegments(t_value[i + 1]) == dp_char))
        {
            seg |= dp_char;
        }

        raw_buffer.push_back(seg);
        ++digit_count;
    }

    // Pad with zeros if not enough digits
    while (raw_buffer.size() < group_size)
        raw_buffer.push_back(0);
        
    return raw_buffer;
    
}

std::string SevenDigitDisplay::populate_scrolled_value(std::string& original_value, const int group_size, const digitdodo::ScrollDirection t_direction, int& scroll_pos)
{
    const int padded_size = original_value.size() + group_size;    

    std::string padded_value = original_value + std::string(group_size, ' ');

    bool right_to_left = (t_direction == digitdodo::ScrollDirection::RightToLeft);

    int digit_count = 0;
    int i = 0;
    int non_displayable_char_count = 0;

    std::string scrolled;
    scrolled.reserve(group_size);

    while (digit_count < group_size)
    {
        int index;
        if (right_to_left)
        {
            index = (scroll_pos + i) % padded_size;
        }
        else
        {
            index = scroll_pos - group_size + 1 + i;
            if (index < 0) index += padded_size;
            else if (index >= padded_size) index -= padded_size;
        }

        char c = padded_value[index];
        unsigned char seg = getCharSegments(c);

        if ((digit_count == 0) && seg == dp_char)
        {
            non_displayable_char_count += 1;
            
            if (right_to_left)
            {
                ++i;
            }
            else
            {
                i--;
            }

            continue;
        }

        scrolled += c;
        if (seg != dp_char)
        {
            ++digit_count;
        }


        ++i;
    }

    // Advance scroll position
    if (right_to_left)
    {
        scroll_pos = (scroll_pos + 1 + non_displayable_char_count) % padded_size;
    }
    else
    {
        scroll_pos = (scroll_pos - 1 - non_displayable_char_count + padded_size) % padded_size;
    }

    return scrolled;
}

bool SevenDigitDisplay::isCharValid(unsigned char t_char)
{
    bool valid = false;

    auto it = sevenSegmentCharMap.find(t_char);
    if (it != sevenSegmentCharMap.end())
    {
        valid = true;
    }

    return valid;
}

unsigned char SevenDigitDisplay::getCharSegments(unsigned char t_char)
{
    if(!isCharValid(t_char))
    {
        return sevenSegmentCharMap.at(' ');
    }
    return sevenSegmentCharMap.at(t_char);
}
