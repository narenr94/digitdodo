#include "digit_dodo.hpp"


std::shared_ptr<digit_dodo> s_pInstance = nullptr;

digit_dodo::digit_dodo(I7SegmentPlatform* t_platform_impl)
{
    m_platform_impl = t_platform_impl;

    for(int i = 0; i < m_platform_impl->getGroups().size(); i++)
    {
        m_display_mode.push_back(SevenSegmentDisplayMode::Normal);
    }
}

void digit_dodo::update_display_value(const std::string& t_group_name, const std::string& t_value)
{
    m_platform_impl->updateRawBuffer(t_group_name, t_value);
}

void digit_dodo::set_mode_normal()
{

}

void digit_dodo::set_mode_blink(unsigned int t_on_ticks, unsigned int t_off_ticks)
{

}

void digit_dodo::set_mode_scroll(unsigned int t_scroll_speed)
{

}
