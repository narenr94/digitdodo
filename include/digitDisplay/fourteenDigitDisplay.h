#pragma once

#include "baseDigitDisplay.h"

class FourteenDigitDisplay : public BaseDigitDisplay
{
    public:
    
    std::vector<unsigned char> getRawBuffer(const std::string& t_value, int group_size) override;

    std::string populate_scrolled_value(std::string& original_value, const int group_size, const digitdodo::ScrollDirection t_direction, int& scroll_pos) override;

};