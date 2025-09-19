#pragma once

#include <string>
#include <vector>

namespace digit_dodo_platform {

enum class SegmentDisplayType {
    SEG_7,
    SEG_14,
    SEG_16
};

struct HardwareGroup{

    std::string name;
    int length;
    std::vector<SegmentDisplayType> type;

    HardwareGroup(std::string t_name, std::vector<SegmentDisplayType> t_type)
    {
        name = t_name;
        type = t_type;
        length = t_type.size();
    }

};


std::vector<HardwareGroup>& getGroups();
void updateRawBuffer(const std::string& t_group_name, std::vector<unsigned char>& buffer);

} // namespace digit_dodo_platform
