#ifndef NVPARSE_TEXT_HPP_INCLUDED
#define NVPARSE_TEXT_HPP_INCLUDED

#include <cctype>
#include <cwctype>
#include <stdexcept>

namespace nvparsehtml {
// Detect whitespace character
template <typename Ch>
struct whitespace_pred;

template <typename Ch>
struct Text {
    static bool parse_no_entity_translation;
    static bool parse_normalize_whitespace;
    static bool parse_trim_whitespace;
    static bool parse_no_utf8;

    // clang-format off
    // Whitespace (space \n \r \t)
    static constexpr unsigned char lookup_whitespace[256] = 
    {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  1,  0,  0,  // 0
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 1
        1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 2
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 3
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 4
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 5
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 6
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 7
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 8
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 9
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // A
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // B
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // C
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // D
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // E
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   // F
    };

    // Node name (anything but space \n \r \t / > ? \0)
    static constexpr unsigned char lookup_node_name[256] = 
    {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  0,  1,  1,  // 0
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
        0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  // 2
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  // 3
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
    };

    // Text (i.e. PCDATA) (anything but < \0)
    static constexpr unsigned char lookup_text[256] = 
    {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  1,  1,  // 3
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
    };

    // Text (i.e. PCDATA) that does not require processing when ws normalization is disabled 
    // (anything but < \0 &)
    static constexpr unsigned char lookup_text_pure_no_ws[256] = 
    {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
        1,  1,  1,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  1,  1,  // 3
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
    };

    // Text (i.e. PCDATA) that does not require processing when ws normalizationis is enabled
    // (anything but < \0 & space \n \r \t)
    static constexpr unsigned char lookup_text_pure_with_ws[256] = 
    {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  0,  1,  1,  // 0
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
        0,  1,  1,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  1,  1,  1,  // 3
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
    };

    // Attribute name (anything but space \n \r \t / < > = ? ! \0)
    static constexpr unsigned char lookup_attribute_name[256] = 
    {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  0,  1,  1,  // 0
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
        0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  // 2
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  // 3
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
    };

    // Attribute data with single quote (anything but ' \0)
    static constexpr unsigned char lookup_attribute_data_1[256] = 
    {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
        1,  1,  1,  1,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 3
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
    };

    // Attribute data with single quote that does not require processing (anything but ' \0 &)
    static constexpr unsigned char lookup_attribute_data_1_pure[256] = 
    {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
        1,  1,  1,  1,  1,  1,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 3
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
    };

    // Attribute data with double quote (anything but " \0)
    static constexpr unsigned char lookup_attribute_data_2[256] = 
    {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
        1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 3
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
    };

    // Attribute data with double quote that does not require processing (anything but " \0 &)
    static constexpr unsigned char lookup_attribute_data_2_pure[256] = 
    {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 0
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 1
        1,  1,  0,  1,  1,  1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 2
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 3
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 4
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 5
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 6
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 7
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 8
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // 9
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // A
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // B
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // C
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // D
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  // E
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1   // F
    };

    // Digits (dec and hex, 255 denotes end of numeric character reference)
    static constexpr unsigned char lookup_digits[256] = 
    {
     // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 0
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 1
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 2
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  255,255,255,255,255,255,  // 3
        255, 10, 11, 12, 13, 14, 15,255,255,255,255,255,255,255,255,255,  // 4
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 5
        255, 10, 11, 12, 13, 14, 15,255,255,255,255,255,255,255,255,255,  // 6
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 7
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 8
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // 9
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // A
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // B
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // C
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // D
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,  // E
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255   // F
    };
    // clang-format on

    // Insert coded character, using UTF8 or 8-bit ASCII
    static void insert_coded_character(Ch *&text, unsigned long code) {
        if (parse_no_utf8) {
            // Insert 8-bit ASCII character
            // Todo: possibly verify that code is less than 256 and use
            // replacement char otherwise?
            text[0] = static_cast<unsigned char>(code);
            text += 1;
        } else {
            // Insert UTF8 sequence
            if (code < 0x80)  // 1 byte sequence
            {
                text[0] = static_cast<unsigned char>(code);
                text += 1;
            } else if (code < 0x800)  // 2 byte sequence
            {
                text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF);
                code >>= 6;
                text[0] = static_cast<unsigned char>(code | 0xC0);
                text += 2;
            } else if (code < 0x10000)  // 3 byte sequence
            {
                text[2] = static_cast<unsigned char>((code | 0x80) & 0xBF);
                code >>= 6;
                text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF);
                code >>= 6;
                text[0] = static_cast<unsigned char>(code | 0xE0);
                text += 3;
            } else if (code < 0x110000)  // 4 byte sequence
            {
                text[3] = static_cast<unsigned char>((code | 0x80) & 0xBF);
                code >>= 6;
                text[2] = static_cast<unsigned char>((code | 0x80) & 0xBF);
                code >>= 6;
                text[1] = static_cast<unsigned char>((code | 0x80) & 0xBF);
                code >>= 6;
                text[0] = static_cast<unsigned char>(code | 0xF0);
                text += 4;
            } else  // Invalid, only codes up to 0x10FFFF are allowed in Unicode
            {
                throw std::runtime_error("parse error: invalid numeric character entity");
                // RAPIDXML_PARSE_ERROR("invalid numeric character entity",
                // text);
            }
        }
    }

    // Skip characters until predicate evaluates to true
    template <class StopPred>
    static void skip(Ch *&text) {
        Ch *tmp = text;
        while (StopPred::test(*tmp))
            ++tmp;
        text = tmp;
    }

    // Skip characters until predicate evaluates to true while doing the
    // following:
    // - replacing XML character entity references with proper characters
    // (&apos; &amp; &quot; &lt; &gt; &#...;)
    // - condensing whitespace sequences to single space character
    template <class StopPred, class StopPredPure>
    static Ch *skip_and_expand_character_refs(Ch *&text) {
        // If entity translation, whitespace condense and whitespace trimming is
        // disabled, use plain skip
        if (parse_no_entity_translation && !parse_normalize_whitespace && !parse_trim_whitespace) {
            skip<StopPred>(text);
            return text;
        }

        // Use simple skip until first modification is detected
        skip<StopPredPure>(text);

        // Use translation skip
        Ch *src = text;
        Ch *dest = src;
        while (StopPred::test(*src)) {
            // If entity translation is enabled
            if (!(parse_no_entity_translation)) {
                // Test if replacement is needed
                if (src[0] == Ch('&')) {
                    switch (src[1]) {
                        // &amp; &apos;
                        case Ch('a'):
                            if (src[2] == Ch('m') && src[3] == Ch('p') && src[4] == Ch(';')) {
                                *dest = Ch('&');
                                ++dest;
                                src += 5;
                                continue;
                            }
                            if (src[2] == Ch('p') && src[3] == Ch('o') && src[4] == Ch('s') &&
                                src[5] == Ch(';')) {
                                *dest = Ch('\'');
                                ++dest;
                                src += 6;
                                continue;
                            }
                            break;

                        // &quot;
                        case Ch('q'):
                            if (src[2] == Ch('u') && src[3] == Ch('o') && src[4] == Ch('t') &&
                                src[5] == Ch(';')) {
                                *dest = Ch('"');
                                ++dest;
                                src += 6;
                                continue;
                            }
                            break;

                        // &gt;
                        case Ch('g'):
                            if (src[2] == Ch('t') && src[3] == Ch(';')) {
                                *dest = Ch('>');
                                ++dest;
                                src += 4;
                                continue;
                            }
                            break;

                        // &lt;
                        case Ch('l'):
                            if (src[2] == Ch('t') && src[3] == Ch(';')) {
                                *dest = Ch('<');
                                ++dest;
                                src += 4;
                                continue;
                            }
                            break;

                        // &#...; - assumes ASCII
                        case Ch('#'):
                            if (src[2] == Ch('x')) {
                                unsigned long code = 0;
                                src += 3;  // Skip &#x
                                while (1) {
                                    unsigned char digit =
                                        lookup_digits[static_cast<unsigned char>(*src)];
                                    if (digit == 0xFF)
                                        break;
                                    code = code * 16 + digit;
                                    ++src;
                                }
                                insert_coded_character(dest, code);  // Put character in output
                            } else {
                                unsigned long code = 0;
                                src += 2;  // Skip &#
                                while (1) {
                                    unsigned char digit =
                                        lookup_digits[static_cast<unsigned char>(*src)];
                                    if (digit == 0xFF)
                                        break;
                                    code = code * 10 + digit;
                                    ++src;
                                }
                                insert_coded_character(dest, code);  // Put character in output
                            }
                            if (*src == Ch(';'))
                                ++src;
                            else
                                throw std::runtime_error("parse error: expected ;");
                            // RAPIDXML_PARSE_ERROR("expected ;", src);
                            continue;

                        // Something else
                        default:
                            // Ignore, just copy '&' verbatim
                            break;
                    }
                }
            }

            // If whitespace condensing is enabled
            if (parse_normalize_whitespace) {
                // Test if condensing is needed
                if (whitespace_pred<Ch>::test(*src)) {
                    *dest = Ch(' ');
                    ++dest;  // Put single space in dest
                    ++src;   // Skip first whitespace char
                    // Skip remaining whitespace chars
                    while (whitespace_pred<Ch>::test(*src))
                        ++src;
                    continue;
                }
            }

            // No replacement, only copy character
            *dest++ = *src++;
        }

        // Return new end
        text = src;
        return dest;
    }
};
template <typename Ch>
bool Text<Ch>::parse_no_entity_translation = false;

template <typename Ch>
bool Text<Ch>::parse_normalize_whitespace = false;

template <typename Ch>
bool Text<Ch>::parse_trim_whitespace = false;

template <typename Ch>
bool Text<Ch>::parse_no_utf8 = false;

// Detect whitespace character
template <typename Ch>
struct whitespace_pred {
    static unsigned char test(Ch ch) {
        return Text<Ch>::lookup_whitespace[static_cast<unsigned char>(ch)];
    }
};

// Detect node name character
template <typename Ch>
struct node_name_pred {
    static unsigned char test(Ch ch) {
        return Text<Ch>::lookup_node_name[static_cast<unsigned char>(ch)];
    }
};

// Detect attribute name character
template <typename Ch>
struct attribute_name_pred {
    static unsigned char test(Ch ch) {
        return Text<Ch>::lookup_attribute_name[static_cast<unsigned char>(ch)];
    }
};

// Detect text character (PCDATA)
template <typename Ch>
struct text_pred {
    static unsigned char test(Ch ch) {
        return Text<Ch>::lookup_text[static_cast<unsigned char>(ch)];
    }
};

// Detect text character (PCDATA) that does not require processing
template <typename Ch>
struct text_pure_no_ws_pred {
    static unsigned char test(Ch ch) {
        return Text<Ch>::lookup_text_pure_no_ws[static_cast<unsigned char>(ch)];
    }
};

// Detect text character (PCDATA) that does not require processing
template <typename Ch>
struct text_pure_with_ws_pred {
    static unsigned char test(Ch ch) {
        return Text<Ch>::lookup_text_pure_with_ws[static_cast<unsigned char>(ch)];
    }
};

// Detect attribute value character
template <typename Ch, Ch Quote>
struct attribute_value_pred {
    static unsigned char test(Ch ch) {
        if (Quote == Ch('\''))
            return Text<Ch>::lookup_attribute_data_1[static_cast<unsigned char>(ch)];
        if (Quote == Ch('\"'))
            return Text<Ch>::lookup_attribute_data_2[static_cast<unsigned char>(ch)];
        return 0;  // Should never be executed, to avoid warnings on Comeau
    }
};

// Detect attribute value character
template <typename Ch, Ch Quote>
struct attribute_value_pure_pred {
    static unsigned char test(Ch ch) {
        if (Quote == Ch('\''))
            return Text<Ch>::lookup_attribute_data_1_pure[static_cast<unsigned char>(ch)];
        if (Quote == Ch('\"'))
            return Text<Ch>::lookup_attribute_data_2_pure[static_cast<unsigned char>(ch)];
        return 0;  // Should never be executed, to avoid warnings on Comeau
    }
};
}  // namespace nvparsehtml

#endif
