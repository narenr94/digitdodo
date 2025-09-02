#pragma once

#include "platform_contracts/I7SegmentPlatform.hpp"

#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>


class SimI7SegmentPlatformImp : public I7SegmentPlatform
{
    private:

        int m_displayCount;
        std::vector<HardwareGroup> m_groups;

        sf::RenderWindow m_window;        

    public:

        SimI7SegmentPlatformImp();
        int getDisplayCount() const override;
        std::vector<HardwareGroup> getGroups() const override;
        void updateRawBuffer(const std::string& t_group_name, const std::string& buffer) override;
        ~SimI7SegmentPlatformImp();
        sf::RenderWindow& getWindow();


};