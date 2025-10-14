#pragma once

#include "digitdodo_platform.h"
#include "ticktoucan.h"

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cstdarg>
#include <cstdio>


enum class SevenSegmentDisplayMode { Normal, Blink, Scroll };

//forward declaration
class BaseDigitDisplay;


class digitdodo{

    public:

    /**
     * @enum ScrollDirection
     * @brief Direction of text scrolling for a display group.
     *
     * LeftToRight scrolls the visible window from right-most characters toward left.
     * RightToLeft scrolls the visible window from left-most characters toward right.
     */
    enum ScrollDirection { LeftToRight, RightToLeft };

    private:

    struct s_display_params{

        TickToucan::Handle handle;
        std::string display_value;
        bool visible;
        int scroll_pos;
        SevenSegmentDisplayMode mode;
        BaseDigitDisplay* display_type;
        ScrollDirection scroll_direction;
    };

    std::vector<digitdodo_platform::HardwareGroup>& m_groups;

    std::unordered_map<std::string, s_display_params> m_group_params;

    digitdodo();

    /**
     * @brief Cancel and clean up any scheduled handle associated with the group.
     * @param t_group_name Name of the group whose scheduled handle should be cancelled.
     *
     * If a TickToucan handle exists for the group, it is cancelled and the
     * stored handle is reset. Safe to call when no handle is active.
     */
    void cancel_group_handle(const std::string& t_group_name);

    /**
     * @brief Update the internal mode state and perform mode-specific setup.
     * @param t_group_name Name of the group.
     * @param t_mode Mode to apply.
     * @param t_param Mode-specific parameter (for example timing).
     * @return True if mode-specific setup succeeded, false otherwise.
     *
     * Called by set_display_mode to perform the actual work of enabling blink,
     * scroll or other behaviors. This may schedule callbacks or initialize state.
     */
    bool update_display_mode(const std::string& t_group_name, SevenSegmentDisplayMode t_mode, unsigned int t_param);

    /**
     * @brief Enable blink behavior for a group by scheduling periodic toggles.
     * @param t_group_name Target group name.
     * @param t_toggle_ms Blink toggle interval in milliseconds.
     * @return True if the blink scheduling succeeded, false otherwise.
     *
     * Schedules a recurring task that toggles the group's visibility at the
     * requested interval.
     */
    bool set_blink_mode(const std::string& t_group_name, unsigned int t_toggle_ms);

    /**
     * @brief Callback used to toggle blink state for a scheduled group.
     * @param ctx Context pointer passed by the scheduler (expected to be a stable group-name pointer).
     *
     * This static function is the scheduled callback that toggles the group's
     * visible state and updates the hardware accordingly.
     */
    static void toggle_blink(void* ctx);

    /**
     * @brief Obtain the index of a named hardware group.
     * @param t_group_name Name of the group to lookup.
     * @return Zero-based index of the group in m_groups on success, or -1 if not found.
     *
     * Helper to resolve group names to their corresponding HardwareGroup index.
     */
    int get_group_index(const std::string& t_group_name);

    /**
     * @brief Log diagnostic messages from digitdodo.
     * @param format printf-style format string.
     * @param ... Arguments matched to format specifiers.
     *
     * Conditionally emits log output depending on build-time logging configuration.
     * This wrapper centralizes formatting and flushing behavior used across the class.
     */
    void log_digitdodo(const char* format, ...);

    /**
     * @brief Enable scroll behavior for a group by scheduling periodic updates.
     * @param t_group_name Target group name.
     * @param t_speed_ms Scroll update interval in milliseconds.
     * @return True if scheduling succeeded, false otherwise.
     *
     * Prepares and schedules the scroll callback that will periodically update the
     * visible window for the group.
     */
    bool set_scroll_mode(const std::string& t_group_name, unsigned int t_speed_ms);

    /**
     * @brief Static callback used by the scheduler to advance scrolling text.
     * @param ctx Context pointer passed by the scheduler (expected to be a stable group-name pointer).
     *
     * This function is invoked periodically to compute the next visible frame for
     * the group's scrolling text and push the updated value to the display.
     */
    static void scroll_text(void* ctx);

    /**
     * @brief Compute the next scrolled string for the specified group.
     * @param group_name Name of the target group. The function reads the group's
     *        preserved source text and the current scroll position to produce a
     *        window of length equal to the group's physical digit count.
     * @return String containing the next visible window (not stored internally).
     *
     * This routine respects the configured scroll direction and handles decimal
     * point placement (dots) when computing the visible frame. It does not
     * permanently modify the preserved source string; it only returns the next
     * frame which callers typically push to the hardware.
     */
    std::string populate_scrolled_value(std::string& group_name);

    void pad_display_value(unsigned int t_group_index);

    void unpad_display_value(unsigned int t_group_index);

    public:

    /**
     * @brief Obtain the singleton instance of digitdodo.
     * @return Reference to the single digitdodo instance.
     *
     * This implements the Meyers singleton pattern; the instance is lazily
     * initialized on first use and lives for the lifetime of the program.
     */
    static digitdodo& getInstance();  
    
    /**
     * @brief Update the rendered content for a hardware display group.
     * @param t_group_name Name of the hardware group to update.
     * @return True on success, false on error.
     *
     * Converts the logical display string for the named group into the
     * underlying segment representation and writes it to the platform layer.
     * Does not modify application-level state beyond performing the update.
     */
    bool update_display(const std::string& t_group_name);

    /**
     * @brief Set the display value for a group and optionally refresh hardware.
     * @param t_group_name Name of the group to update.
     * @param t_value New textual value assigned to the group.
     * @param t_update_raw If true, immediately convert and write the new value to hardware.
     * @return True when the value was accepted, false on error (for example unknown group).
     *
     * Stores the provided textual value for the group. When t_update_raw is true,
     * the function calls the lower-level update routine to reflect the change on the
     * physical display.
     */
    bool update_display_value(const std::string& t_group_name, const std::string t_value, bool t_update_raw = false);

    /**
     * @brief Convert a textual value and push segment output to hardware.
     * @param t_group_name Name of the target group.
     * @param t_value Textual value to be rendered on the hardware.
     * @return True on success, false on error.
     *
     * This helper prepares the segment data for the provided textual value and
     * sends it to the platform-specific layer that performs the actual write to
     * the display hardware. Does not update the internal maintained display value.
     */
    bool update_segment_output(const std::string& t_group_name, const std::string& t_value);
    
    /**
     * @brief Configure the display mode for a group (normal, blink, scroll, etc.).
     * @param t_group_name Name of the group to configure.
     * @param t_mode Desired display mode.
     * @param t_param Mode-specific parameter (for example blink/scroll interval in ms).
     * @return True when the mode was set and any required timers or tasks were successfully scheduled, false otherwise.
     *
     * Cancels any previously scheduled group-specific operations before applying
     * the requested mode. Mode-specific setup (timers, callbacks) is performed as needed.
     */
    bool set_display_mode(const std::string& t_group_name, SevenSegmentDisplayMode t_mode, unsigned int t_param);

    /**
     * @brief Turn off (clear) the specified display group.
     * @param t_group_name Name of the group to turn off.
     *
     * Writes a zeroed segment buffer to the hardware for the named group,
     * effectively turning off all segments for that group.
     */
    void turn_off_group(const std::string& t_group_name);

    /**
     * @brief Turn on the specified display group.
     * @param t_group_name Name of the group to turn off.
     *
     * Writes valid segment buffer to the hardware for the named group,
     * effectively turning on that group.
     */
    void turn_on_group(const std::string& t_group_name);

    /**
     * @brief Set the scroll direction for a specific group.
     * @param t_group_name Name of the group.
     * @param t_direction Desired scrolling direction (LeftToRight or RightToLeft).
     *
     * Updates the internal scroll direction used by the group's scrolling logic.
     * No scheduling or immediate display change is performed by this function.
     */
    void set_scroll_direction(const std::string& t_group_name, ScrollDirection t_direction);

    bool get_group_visibility(const std::string& t_group_name);

    void set_group_visibility(const std::string& t_group_name, bool t_visible);
};