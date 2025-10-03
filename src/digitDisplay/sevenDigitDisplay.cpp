#include "sevenDigitDisplay.h"
#include "digitdodo_platform.h"

#include <unordered_map>

static const std::unordered_map<unsigned char, unsigned char> sevenSegmentCharMap = {
    // digits
    {'0', 0b11111100}, {'1', 0b01100000}, {'2', 0b11011010}, {'3', 0b11110010},
    {'4', 0b01100110}, {'5', 0b10110110}, {'6', 0b10111110}, {'7', 0b11100000},
    {'8', 0b11111110}, {'9', 0b11110110},

    // basic symbols
    {' ', 0b00000000}, {'.', 0b00000001}, {'-', 0b00000010}, {'_', 0b00010000},
    {':', 0b00000001}, {',', 0b00000001}, {'=', 0b00010010}, {'+', 0b01100110},
    {'?', 0b11000110}, {'!', 0b01100000}, {'/', 0b01001000}, {'\\',0b00011000},
    {'*', 0b01011010}, {'%', 0b11001001}, {'\'',0b00000000}, {'"', 0b01000100},
    {'#', 0b01011010}, {'@', 0b11110110},

    // uppercase letters (best 7-seg approximations)
    {'A', 0b11101110}, {'B', 0b00111110}, {'C', 0b10011100}, {'D', 0b01111010},
    {'E', 0b10011110}, {'F', 0b10001110}, {'G', 0b10111100}, {'H', 0b01101110},
    {'I', 0b01100000}, {'J', 0b01110010}, {'K', 0b01101110}, {'L', 0b00011100},
    {'M', 0b01010101}, {'N', 0b01010100}, {'O', 0b11111100}, {'P', 0b11001110},
    {'Q', 0b11100110}, {'R', 0b00001010}, {'S', 0b10110110}, {'T', 0b00011110},
    {'U', 0b01111100}, {'V', 0b00111000}, {'W', 0b01010101}, {'X', 0b01101110},
    {'Y', 0b01110110}, {'Z', 0b11011010},

    // lowercase letters (approximations where meaningful)
    {'a', 0b11101110}, {'b', 0b00111110}, {'c', 0b10011010}, {'d', 0b01111010},
    {'e', 0b11011110}, {'f', 0b10001110}, {'g', 0b11110110}, {'h', 0b00101110},
    {'i', 0b00001000}, {'j', 0b01110000}, {'k', 0b01101110}, {'l', 0b00011100},
    {'m', 0b01010101}, {'n', 0b00101010}, {'o', 0b00111010}, {'p', 0b11001110},
    {'q', 0b11100110}, {'r', 0b00001010}, {'s', 0b10110110}, {'t', 0b00011110},
    {'u', 0b00111000}, {'v', 0b00111000}, {'w', 0b01010101}, {'x', 0b01101110},
    {'y', 0b01110110}, {'z', 0b11011010},

    // brackets and angle symbols
    {'(', 0b00011100}, {')', 0b01110000}, {'<', 0b10010000}, {'>', 0b00100100},
    {'[', 0b10011100}, {']', 0b01110010}, {'{', 0b10011100}, {'}', 0b01110010},

    // remaining common punctuation approximations
    {';', 0b00000001}, {'$', 0b10110110}, {'&', 0b10111110}, {'^', 0b00000100},
    {'`', 0b00000000}, {'~', 0b00000000}, {'|', 0b00001000}
};

bool isCharValid(unsigned char t_char)
{
    bool valid = false;

    auto it = sevenSegmentCharMap.find(t_char);
    if (it != sevenSegmentCharMap.end())
    {
        valid = true;
    }

    if(sevenSegmentCharMap.at(t_char) == sevenSegmentCharMap.at('.'))
    {
        valid = false; // standalone dot is not valid
    }

    return valid;
}

unsigned char getCharSegments(unsigned char t_char)
{
    return sevenSegmentCharMap.at(t_char);
}


std::vector<unsigned char> SevenDigitDisplay::getRawBuffer(const std::string& t_value, int group_size)
{
    std::vector<unsigned char> raw_buffer;
    raw_buffer.reserve(group_size);

    for (size_t i = 0, digit_count = 0; i < t_value.size() && digit_count < group_size; ++i)
    {
        unsigned char c = t_value[i];
        if (!isCharValid(c))
        {
            continue;
        }            

        unsigned char seg = getCharSegments(c);

        // Check if next character is a dot
        if (i + 1 < t_value.size() && (getCharSegments(t_value[i + 1]) == getCharSegments('.')))
        {
            seg |= 0x01;
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
    bool leading_dot_skipped = false;

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

        if (digit_count == 0 && (getCharSegments(c) == getCharSegments('.')))
        {
            leading_dot_skipped = true;
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
        if ((getCharSegments(c) != getCharSegments('.')))
        {
            ++digit_count;
        }


        ++i;
    }

    // Advance scroll position
    if (right_to_left)
    {
        scroll_pos = (scroll_pos + 1 + (leading_dot_skipped ? 1 : 0)) % padded_size;
    }
    else
    {
        scroll_pos = (scroll_pos - 1 - (leading_dot_skipped ? 1 : 0) + padded_size) % padded_size;
    }

    return scrolled;
}
