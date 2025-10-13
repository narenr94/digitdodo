#pragma once

#include "baseDigitDisplay.h"

class FourteenDigitDisplay : public BaseDigitDisplay
{
    std::vector<unsigned char> dp_char;

    public:

    FourteenDigitDisplay();
    
    std::vector<unsigned char> getRawBuffer(const std::string& t_value, int group_size) override;

    std::string populate_scrolled_value(std::string& original_value, const int group_size, const digitdodo::ScrollDirection t_direction, int& scroll_pos) override;

    bool isCharValid(unsigned char t_char) override;

    std::vector<unsigned char> getCharSegments(unsigned char t_char);

};