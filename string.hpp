#ifndef NVPARSE_STRING_HPP_INCLUDED
#define NVPARSE_STRING_HPP_INCLUDED

#include <cctype>
#include <cwctype>

namespace nvparsehtml {
template <typename Ch>
class String {
    Ch *m_c;
    size_t m_length;

   public:
    String() : m_c(nullptr), m_length(0) {
    }
    String(const Ch *text, size_t length) : m_length(length) {
        m_c = const_cast<Ch *>(text);
    }

    size_t length() const {
        return m_length;
    }

    bool empty() const {
        return m_length == 0;
    }

    bool contains(Ch ch) const {
        for (size_t i = 0; i < m_length; ++i) {
            if (m_c[i] == ch)
                return true;
        }
        return false;
    }

    const Ch &operator[](const size_t i) const {
        return m_c[i];
    }

    Ch *data() const {
        Ch *c = m_c;
        return c;
    }

    void to_uppercase() {
        for (size_t i = 0; i < m_length; ++i) {
            m_c[i] = std::toupper(m_c[i]);
        }
    }

    void to_lowercase() {
        for (size_t i = 0; i < m_length; ++i) {
            m_c[i] = std::tolower(m_c[i]);
        }
    }
};

template <>
void String<wchar_t>::to_uppercase() {
    for (size_t i = 0; i < m_length; ++i) {
        m_c[i] = std::towupper(m_c[i]);
    }
}

template <>
void String<wchar_t>::to_lowercase() {
    for (size_t i = 0; i < m_length; ++i) {
        m_c[i] = std::towlower(m_c[i]);
    }
}

template <typename Ch>
void to_uppercase(Ch *text, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        text[i] = std::toupper(text[i]);
    }
}
template <>
void to_uppercase<wchar_t>(wchar_t *text, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        text[i] = std::towupper(text[i]);
    }
}

template <typename Ch>
void to_lowercase(Ch *text, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        text[i] = std::tolower(text[i]);
    }
}
template <>
void to_lowercase<wchar_t>(wchar_t *text, size_t length) {
    for (size_t i = 0; i < length; ++i) {
        text[i] = std::towlower(text[i]);
    }
}

template <typename Ch>
inline bool operator<(const String<Ch> &lhs, const String<Ch> &rhs) {
    const Ch *rhs_c = rhs.data();
    size_t rhs_length = rhs.length();
    const Ch *lhs_c = lhs.data();
    size_t lhs_length = lhs.length();

    size_t minLen = lhs_length < rhs_length ? lhs_length : rhs_length;
    for (size_t i = 0; i < minLen; ++i) {
        if (lhs_c[i] < rhs_c[i])
            return true;
        if (lhs_c[i] > rhs_c[i])
            return false;
    }
    if (lhs_length >= rhs_length)
        return false;
    return true;
}
template <typename Ch>
inline bool operator>(const String<Ch> &lhs, const String<Ch> &rhs) {
    return rhs < lhs;
}
template <typename Ch>
inline bool operator<=(const String<Ch> &lhs, const String<Ch> &rhs) {
    return !(lhs > rhs);
}
template <typename Ch>
inline bool operator>=(const String<Ch> &lhs, const String<Ch> &rhs) {
    return !(lhs < rhs);
}

template <typename Ch>
inline bool operator==(const String<Ch> &lhs, const String<Ch> &rhs) {
    const Ch *rhs_c = rhs.data();
    size_t rhs_length = rhs.length();
    const Ch *lhs_c = lhs.data();
    size_t lhs_length = lhs.length();

    if (lhs_length != rhs_length)
        return false;
    for (size_t i = 0; i < lhs_length; ++i) {
        if (lhs_c[i] != rhs_c[i])
            return false;
    }
    return true;
}
template <typename Ch>
inline bool operator!=(const String<Ch> &lhs, const String<Ch> &rhs) {
    return !(lhs == rhs);
}

template <typename Ch>
inline bool compare_ci(const String<Ch> &lhs, const String<Ch> &rhs) {
    const Ch *rhs_c = rhs.data();
    size_t rhs_length = rhs.length();
    const Ch *lhs_c = lhs.data();
    size_t lhs_length = lhs.length();

    if (lhs_length != rhs_length)
        return false;
    for (size_t i = 0; i < lhs_length; ++i) {
        if (std::towlower(lhs_c[i]) != std::towlower(rhs_c[i]))
            return false;
    }
    return true;
}
}  // namespace nvparsehtml

#endif
