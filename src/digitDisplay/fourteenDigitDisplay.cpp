#include "fourteenDigitDisplay.h"
#include "digitdodo_platform.h"

#include <unordered_map>

#define DIGITDODO_SEG14_A   0x01
#define DIGITDODO_SEG14_B   0x02
#define DIGITDODO_SEG14_C   0x04
#define DIGITDODO_SEG14_D   0x08
#define DIGITDODO_SEG14_E   0x10
#define DIGITDODO_SEG14_F   0x20
#define DIGITDODO_SEG14_G1  0x40
#define DIGITDODO_SEG14_G2  0x80 
#define DIGITDODO_SEG14_H   0x01
#define DIGITDODO_SEG14_I   0x02
#define DIGITDODO_SEG14_J   0x04
#define DIGITDODO_SEG14_K   0x08
#define DIGITDODO_SEG14_L   0x10
#define DIGITDODO_SEG14_M   0x20
#define DIGITDODO_SEG14_DP  0x40

   /*

      ___A___
    F|\H | I/|B
      G1- -G2
    E|/K | J\|C
      ___D___   .d


    Binary/bit-line (overall bits 15..0) 
    bit15 bit14 bit13 bit12     bit11 bit10 bit9 bit8 
    0     DP    M     L         K     J     I    H 

    bit7 bit6 bit5 bit4 bit3 bit2 bit1 bit0
    G2   G1   F    E    D    C    B    A 0

    Byte masks (for reference) lowByte (A..G2) LSB->MSB: b0 A=0x01, b1 B=0x02, b2 C=0x04, b3 D=0x08, b4 E=0x10, b5 F=0x20, b6 G1=0x40, b7 G2=0x80 highByte (H..DP) LSB->MSB: b0 H=0x01, b1 I=0x02, b2 J=0x04, b3 K=0x08, b4 L=0x10, b5 M=0x20, b6 DP=0x40, b7 unused=0x80
   
    */

// Map entries use { lowByte, highByte } where highByte uses the H..DP masks directly.
static const std::unordered_map<unsigned char, std::vector<unsigned char>> fourteenSegmentMap = {
    // digits
    {'0', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F), (unsigned char)0x00 }}, // A B C D E F
    {'1', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C), (unsigned char)0x00 }}, // B C
    {'2', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // A B D E G1 G2
    {'3', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // A B C D G1 G2
    {'4', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // B C F G1 G2
    {'5', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // A C D F G1 G2
    {'6', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // A C D E F G1 G2
    {'7', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C), (unsigned char)0x00 }}, // A B C
    {'8', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // all A..G2
    {'9', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // A B C D F G1 G2

    // uppercase letters
    {'A', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // A B C E F G1 G2
    {'B', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_G2), (unsigned char)(DIGITDODO_SEG14_L | DIGITDODO_SEG14_M) }}, // like 0 + M
    {'C', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F), (unsigned char)0x00 }}, // A D E F
    {'D', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D), (unsigned char)(DIGITDODO_SEG14_L | DIGITDODO_SEG14_M) }}, // A B C D + M
    {'E', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1), (unsigned char)0x00 }}, // A D E F G1
    {'F', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1), (unsigned char)0x00 }}, // A E F G1
    {'G', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // A C D E F G2
    {'H', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // B C E F G1 G2 + L+M
    {'I', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_D), (unsigned char)(DIGITDODO_SEG14_L | DIGITDODO_SEG14_M) }}, // B C + M
    {'J', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E), (unsigned char)0x00 }}, // B C D E
    {'K', { (unsigned char)(DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1), (unsigned char)(DIGITDODO_SEG14_I | DIGITDODO_SEG14_J) }}, // E F G1 G2 + L
    {'L', { (unsigned char)(DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F), (unsigned char)0x00 }}, // D E F
    {'M', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F), (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_I) }}, // B C E F + L+M
    {'N', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F), (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_J) }}, // B C E F + L
    {'O', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F), (unsigned char)0x00 }}, // same as 0
    {'P', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // A B E F G1
    {'Q', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F), (unsigned char)(DIGITDODO_SEG14_J) }}, // 0 + L (Q tail)
    {'R', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1), (unsigned char)(DIGITDODO_SEG14_J) }}, // P + L
    {'S', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // like 5
    {'T', { (unsigned char)(DIGITDODO_SEG14_A), (unsigned char)(DIGITDODO_SEG14_L | DIGITDODO_SEG14_M) }}, // A + M
    {'U', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F), (unsigned char)0x00 }}, // B C D E F
    {'V', { (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_I) }}, // E F
    {'W', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F), (unsigned char)(DIGITDODO_SEG14_J | DIGITDODO_SEG14_K) }}, // B C E F + M
    {'X', { (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_I | DIGITDODO_SEG14_J | DIGITDODO_SEG14_K) }}, // L+M
    {'Y', { (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_I | DIGITDODO_SEG14_M) }}, // M
    {'Z', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_D), (unsigned char)(DIGITDODO_SEG14_I | DIGITDODO_SEG14_K) }}, // rectangular Z

    // lowercase (mapped to uppercase shapes)
    {'a', { (unsigned char)(DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_G1), (unsigned char)(DIGITDODO_SEG14_M) }},
    {'b', { (unsigned char)(DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'c', { (unsigned char)(DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'d', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'e', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'f', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1), (unsigned char)0x00 }},
    {'g', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'h', { (unsigned char)(DIGITDODO_SEG14_C | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'i', { (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_M) }},
    {'j', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E), (unsigned char)0x00 }},
    {'k', { (unsigned char)(DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1), (unsigned char)(DIGITDODO_SEG14_I | DIGITDODO_SEG14_J) }},
    {'l', { (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_L | DIGITDODO_SEG14_M) }},
    {'m', { (unsigned char)(DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F), (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_I) }},
    {'n', { (unsigned char)(DIGITDODO_SEG14_C | DIGITDODO_SEG14_E | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'o', { (unsigned char)(DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'p', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'q', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_B | DIGITDODO_SEG14_C | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'r', { (unsigned char)(DIGITDODO_SEG14_E | DIGITDODO_SEG14_G1), (unsigned char)0x00 }},
    {'s', { (unsigned char)(DIGITDODO_SEG14_A | DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }},
    {'t', { (unsigned char)(DIGITDODO_SEG14_D | DIGITDODO_SEG14_E | DIGITDODO_SEG14_F | DIGITDODO_SEG14_G1), (unsigned char)0x00 }},
    {'u', { (unsigned char)(DIGITDODO_SEG14_C | DIGITDODO_SEG14_D | DIGITDODO_SEG14_E), (unsigned char)0x00 }},
    {'v', { (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_I) }},
    {'w', { (unsigned char)(DIGITDODO_SEG14_C | DIGITDODO_SEG14_E), (unsigned char)(DIGITDODO_SEG14_J | DIGITDODO_SEG14_K) }},
    {'x', { (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_I | DIGITDODO_SEG14_J | DIGITDODO_SEG14_K) }},
    {'y', { (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_I | DIGITDODO_SEG14_M) }},
    {'z', { (unsigned char)(DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_D), (unsigned char)(DIGITDODO_SEG14_K) }},

    // symbols
    {'-', { (unsigned char)(DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // G1 only
    {'_', { (unsigned char)(DIGITDODO_SEG14_D), (unsigned char)0x00 }}, // D only
    {'=', { (unsigned char)(DIGITDODO_SEG14_D | DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)0x00 }}, // D + G1
    {'*', { (unsigned char)(DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_I | DIGITDODO_SEG14_J | DIGITDODO_SEG14_K | DIGITDODO_SEG14_L | DIGITDODO_SEG14_M) }},
    {'/', { (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_I | DIGITDODO_SEG14_K) }},
    {'\\',{ (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_H | DIGITDODO_SEG14_J) }},
    {'.', { (unsigned char)0x00, (unsigned char)(DIGITDODO_SEG14_DP) }}, // DP
    {' ', { (unsigned char)0x00, (unsigned char)0x00 }},
    {'+', { (unsigned char)(DIGITDODO_SEG14_G1 | DIGITDODO_SEG14_G2), (unsigned char)(DIGITDODO_SEG14_L | DIGITDODO_SEG14_M) }}
};

FourteenDigitDisplay::FourteenDigitDisplay()
{
    dp_char = getCharSegments('.');
}

std::vector<unsigned char> FourteenDigitDisplay::getRawBuffer(const std::string& t_value, int group_size)
{
    std::vector<unsigned char> raw_buffer;
    raw_buffer.reserve(group_size);

    for (size_t i = 0, digit_count = 0; i < t_value.size() && digit_count < group_size; ++i)
    {
        unsigned char c = t_value[i];

        std::vector<unsigned char> seg = getCharSegments(c);

        if (seg == dp_char)
        {
            continue;
        }             

       

        // Check if next character is a dot
        if (i + 1 < t_value.size() && (getCharSegments(t_value[i + 1]) == dp_char))
        {
            seg[1] |= 0x40;
        }

        raw_buffer.push_back(seg[0]);
        raw_buffer.push_back(seg[1]);
        ++digit_count;
    }

    // Pad with zeros if not enough digits
    while (raw_buffer.size() < group_size)
        raw_buffer.push_back(0);
        
    return raw_buffer;
    
}

std::string FourteenDigitDisplay::populate_scrolled_value(std::string& original_value, const int group_size, const digitdodo::ScrollDirection t_direction, int& scroll_pos)
{
    const int padded_size = original_value.size();    

    std::string padded_value = original_value;

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
        std::vector<unsigned char> seg = getCharSegments(c);

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

bool FourteenDigitDisplay::isCharValid(unsigned char t_char)
{
    bool valid = false;

    auto it = fourteenSegmentMap.find(t_char);
    if (it != fourteenSegmentMap.end())
    {
        valid = true;
    }

    return valid;
}

std::vector<unsigned char> FourteenDigitDisplay::getCharSegments(unsigned char t_char)
{
    if(!isCharValid(t_char))
    {
        return fourteenSegmentMap.at(' ');
    }
    return fourteenSegmentMap.at(t_char);
}
