//
// Created by Alex on 2020/1/28.
//

#ifndef LSP_URI_H
#define LSP_URI_H

#include <cstddef>
#include <cstring>
#include <string>
#include "json.hpp"
using json = nlohmann::json;
class string_ref {
public:
    static const size_t npos = ~size_t(0);
    using iterator = const char *;
    using const_iterator = const char *;
    using size_type = size_t;
private:
    const char *m_ref = nullptr;
    size_t m_length = 0;
public:
    string_ref() : m_ref(nullptr), m_length(0) {}
    constexpr string_ref(const char *ref, size_t mLength) : m_ref(ref), m_length(mLength) {}
    string_ref(const char *ref) : m_ref(ref), m_length(strlen(ref)) {}
    string_ref(const std::string &string) : m_ref(string.c_str()), m_length(string.length()) {}
    inline operator const char*() const { return m_ref; }
    inline std::string str() const { return std::string(m_ref, m_length); }
    inline bool operator==(const string_ref &ref) const {
        return m_length == ref.m_length && strcmp(m_ref, ref.m_ref) == 0;
    }
    inline bool operator==(const char *ref) const {
        return strcmp(m_ref, ref) == 0;
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
            if (opt.has()) {
                j = opt.value();
            } else {
                j = nullptr;
            }
        }
        static void from_json(const json& j, option<T>& opt) {
            if (j.is_null()) {
                opt = option<T>();
            } else {
                opt = option<T>(j.get<T>());
            }
        }
    };
}

struct URIForFile {
    std::string file;
    explicit operator bool() const { return !file.empty(); }
    friend bool operator==(const URIForFile &LHS, const URIForFile &RHS) {
        return LHS.file == RHS.file;
    }
    friend bool operator!=(const URIForFile &LHS, const URIForFile &RHS) {
        return !(LHS == RHS);
    }
    friend bool operator<(const URIForFile &LHS, const URIForFile &RHS) {
        return LHS.file < RHS.file;
    }
    URIForFile(const char *str) : file(str) {}
    URIForFile() = default;
    inline std::string &str() { return file; }
};
using DocumentUri = string_ref;

#endif //LSP_URI_H
