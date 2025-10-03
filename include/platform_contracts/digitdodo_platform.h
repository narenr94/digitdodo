#pragma once

#include <string>
#include <vector>

namespace digitdodo_platform {

/**
 * @enum SegmentDisplayType
 * @brief Supported physical segment display encodings.
 *
 * The enum lists the different segment architectures that the platform
 * may expose. The digitdodo controller uses this to map characters to
 * the appropriate segment bitmask for each digit position.
 */
enum class SegmentDisplayType {
    SEG_7,      /**< Standard 7-segment digit (0-9, limited letters). */
    SEG_14,     /**< 14-segment display for broader alphanumeric support. */
    SEG_16      /**< 16-segment display or other extended-segment variant. */
};

/**
 * @struct HardwareGroup
 * @brief Metadata describing a contiguous group of segment digits on the hardware.
 *
 * A HardwareGroup represents a logical display region (for example "volt",
 * "curr", "ad") comprised of one or more digit positions. Each position has
 * an associated SegmentDisplayType which determines how characters are encoded
 * into segment bitmasks.
 */
struct HardwareGroup{

    std::string name;                       /**< Human-readable group identifier. */
    int length;                             /**< Number of digit positions in the group. */
    SegmentDisplayType type;                /**< Per-position segment type list. */

    /**
     * @brief Construct a HardwareGroup.
     * @param t_name Group identifier.
     * @param t_type Vector describing the segment type for each digit position.
     *
     * The group's length is derived from the size of @p t_type.
     */
    HardwareGroup(std::string t_name, SegmentDisplayType t_type, int t_length)
    {
        name = t_name;
        type = t_type;
        length = t_length;
    }

};

/**
 * @brief Retrieve the platform's list of configured hardware groups.
 * @return Reference to a vector containing HardwareGroup entries.
 *
 * The returned vector describes all display groups available on the platform.
 * Implementations should return a reference to a stable, process-lifetime
 * container; callers (the digitdodo controller) rely on the reference remaining
 * valid for the program lifetime.
 */
std::vector<HardwareGroup>& getGroups();

/**
 * @brief Write a raw segment buffer to the named hardware group.
 * @param t_group_name Target group identifier (must match an entry returned by getGroups()).
 * @param buffer Vector of segment bytes, one per digit position, ordered left-to-right.
 *
 * Implementations must convert the provided per-digit segment bytes into
 * whatever bus/driver sequence is required by the hardware. The buffer length
 * should be equal to the HardwareGroup::length for the named group; if the
 * lengths differ, the implementation may truncate or pad as appropriate or
 * return silently, depending on platform policy.
 */
void updateRawBuffer(const std::string& t_group_name, std::vector<unsigned char>& buffer);

} // namespace digitdodo_platform
