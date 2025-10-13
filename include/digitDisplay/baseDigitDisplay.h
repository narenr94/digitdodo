#pragma once

#include "digitdodo.h"

#include <string>
#include <vector>

class BaseDigitDisplay
{
    public:
    
    virtual std::vector<unsigned char> getRawBuffer(const std::string& t_value, int group_size) = 0;

    virtual std::string populate_scrolled_value(std::string& original_value, const int group_size, const digitdodo::ScrollDirection t_direction, int& scroll_pos) = 0;

    virtual bool isCharValid(unsigned char t_char) = 0;

};