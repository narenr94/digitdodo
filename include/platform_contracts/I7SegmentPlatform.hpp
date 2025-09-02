#pragma once

#include <string>
#include <vector>

enum class SegmentDisplayType {
    SEG_7,
    SEG_14,
    SEG_16
};

struct HardwareGroup{

    std::string name;
    int length;
    std::vector<SegmentDisplayType> type;

    HardwareGroup(std::string& t_name, std::vector<SegmentDisplayType>& t_type)
    {
        name = t_name;
        type = t_type;
        length = t_type.size();
    }

};

class I7SegmentPlatform {
public:
    virtual int getDisplayCount() const = 0;
    virtual std::vector<HardwareGroup> getGroups() const = 0;
    virtual void updateRawBuffer(const std::string& t_group_name, const std::string& buffer) = 0;
    virtual ~I7SegmentPlatform(){}
};