//
// Created by Alex on 2020/1/28.
//

#ifndef LSP_URI_H
#define LSP_URI_H

#include <cstddef>
#include <cstring>
#include <string>
#include "json.hpp"
class string_ref {
public:
    static const size_t npos = ~size_t(0);
    using iterator = const char *;
    using const_iterator = const char *;
    using size_type = size_t;
private:
    const char *m_ref;
    size_t m_length;
public:
    string_ref() = default;
    string_ref(std::nullptr_t) = delete;
    constexpr string_ref(const char *ref, size_t mLength) : m_ref(ref), m_length(mLength) {}
    string_ref(const char *ref) : m_ref(ref), m_length(strlen(ref)) {}
    string_ref(const std::string &string) : m_ref(string.c_str()), m_length(string.length()) {}
    inline operator const char*() const { return m_ref; }
    inline std::string str() const { return std::string(m_ref, m_length); }
    inline bool operator==(const string_ref &ref) const {
        return m_length == ref.m_length && strcmp(m_ref, ref.m_ref) == 0;
    }
    inline bool operator>(const string_ref &ref) const { return m_length > ref.m_length; }
    inline bool operator<(const string_ref &ref) const { return m_length < ref.m_length; }
    inline const char *c_str() const { return m_ref; }
    inline bool empty() const { return m_length == 0; }
    iterator begin() const { return m_ref; }
    iterator end() const { return m_ref + m_length; }
    inline const char *data() const { return m_ref; }
    inline size_t size() const { return m_length; }
    char front() const { return m_ref[0]; }
    char back() const { return m_ref[m_length - 1]; }
    template <typename Allocator = std::allocator<char>>
    string_ref copy(Allocator &A = Allocator()) const {
        if (empty())
            return {};
        char *S = A.allocate(m_length);
        std::copy(begin(), end(), S);
        return string_ref(S, m_length);

    }
    char operator[](size_t index) const { return m_ref[index]; }

};
template <typename T>
class option {
public:
    T fStorage;
    bool fHas = false;
    constexpr option() = default;
    option(const T &y) : fStorage(y), fHas(true) {}
    option(T &&y) : fStorage(std::move(y)), fHas(true) {}
    option &operator=(T &&v) {
        fStorage = std::move(v);
        fHas = true;
        return *this;
    }
    option &operator=(const T &v) {
        fStorage = v;
        fHas = true;
        return *this;
    }
    const T *ptr() const { return &fStorage; }
    T *ptr() { return &fStorage; }
    const T &value() const { return fStorage; }
    T &value() { return fStorage; }
    bool has() const { return fHas; }
    const T *operator->() const { return ptr(); }
    T *operator->() { return ptr(); }
    const T &operator*() const { return value(); }
    T &operator*() { return value(); }
    explicit operator bool() const { return fHas; }
};
namespace nlohmann {
    template <typename T>
    struct adl_serializer<option<T>> {
        static void to_json(json& j, const option<T>& opt) {
            if (!opt.has()) {
                j = nullptr;
            } else {
                j = *opt;
            }
        }
        static void from_json(const json& j, option<T>& opt) {
            if (j.is_null()) {
                opt = {};
            } else {
                opt = j.get<T>(); // same as above, but with
                // adl_serializer<T>::from_json
            }
        }
    };
}
struct URI {

};

#endif //LSP_URI_H
