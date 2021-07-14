#pragma once

#include <cstdint>
#include <string>

using namespace std;

namespace graphflow {
namespace common {

struct gf_string_t {

    static constexpr char EMPTY_STRING = 0;

    static const uint64_t PREFIX_LENGTH = 4;
    static const uint64_t STR_LENGTH_PLUS_PREFIX_LENGTH = sizeof(uint32_t) + PREFIX_LENGTH;
    static const uint64_t INLINED_SUFFIX_LENGTH = 8;
    static const uint64_t SHORT_STR_LENGTH = PREFIX_LENGTH + INLINED_SUFFIX_LENGTH;

    gf_string_t() : len{0}, overflowPtr{0} {}

    uint32_t len;
    uint8_t prefix[PREFIX_LENGTH];
    union {
        uint8_t data[INLINED_SUFFIX_LENGTH];
        uint64_t overflowPtr;
    };

    void setOverflowPtrInfo(const uint64_t& pageIdx, const uint16_t& pageOffset);
    void getOverflowPtrInfo(uint64_t& pageIdx, uint16_t& pageOffset);

    inline const uint8_t* getData() const {
        return len <= SHORT_STR_LENGTH ? prefix : reinterpret_cast<uint8_t*>(overflowPtr);
    }

    // This function does *NOT* allocate/resize the overflow buffer, it only copies the content and
    // set the length.
    void set(const string& value);
    void set(const char* value, uint64_t length);
    void set(const gf_string_t& value);

    string getAsString() const;
};

} // namespace common
} // namespace graphflow