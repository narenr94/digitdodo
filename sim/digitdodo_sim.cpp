#include "digitdodo_sim.h"
#include "digitdodo.h"

#include <unistd.h>
#include <vector>
#include <string>

#define TICK_TIME_MS 100

std::vector<std::string> volt = { "12.34567", "11.11", "12.21", "20.31", "204.1" };
std::vector<std::string> curr = { "01.22", "11.22", "21.32", "01.42", "11.42" };

std::string scroll_val = "01.23456789";

int i = 0;

void update_trampolines(void* ctx)
{
    digitdodo& dd = digitdodo::getInstance();
    std::string group_name = std::string((const char*) ctx);
    dd.update_display(group_name);
    i++;
    if(i >= 5) i = 0;
    dd.update_display_value(group_name, volt[i]);
}

void toggle_scroll_direction(void* ctx)
{
    static bool rightToLeft = true;
    digitdodo& dd = digitdodo::getInstance();
    std::string group_name = std::string((const char*) ctx);
    if(rightToLeft)
    {
        rightToLeft = false;
        dd.set_scroll_direction(group_name, digitdodo::ScrollDirection::RightToLeft);
    }
    else
    {
        rightToLeft = true;
        dd.set_scroll_direction(group_name, digitdodo::ScrollDirection::LeftToRight);
    }
    
}

int main() {
    
    

    digitdodo& dodo = digitdodo::getInstance();

    TickToucan& tt = TickToucan::instance();
    tt.init(TICK_TIME_MS); 

    initWindow(800, 800, "DigitDodo 7-Segment");

    dodo.update_display_value("curr", curr[2]);

    dodo.set_display_mode("curr", SevenSegmentDisplayMode::Blink, 500);

    dodo.update_display_value("ad", scroll_val);

    dodo.set_scroll_direction("ad", digitdodo::ScrollDirection::LeftToRight); 

    dodo.set_display_mode("ad", SevenSegmentDisplayMode::Scroll, 500);

    tt.scheduleEvery(1500, &update_trampolines, (void*)dodo.get_display_values().find("volt")->first.c_str(), 5000);

    tt.scheduleEvery(20000, &toggle_scroll_direction, (void*)dodo.get_display_values().find("ad")->first.c_str(), 5000);

    while (windowIsOpen()) {

        tt.dispatch();
        pollEvents();
        // clearWindow();        

        displayWindow();

        usleep(1000 * TICK_TIME_MS);
    }

    destroyWindow();
    return 0;
}


