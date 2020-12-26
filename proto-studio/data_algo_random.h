#include "data_algo_forward.h"

bool starts_with (random_access_range auto r) const
{
    if (size() < r.size()) return false;
    return std::equal(begin(), begin() + r.size(),
        r.begin(), r.end());
}
bool ends_with (random_access_range auto r) const
{
    if (size() < r.size()) return false;
    return std::equal(end() - r.size(), end(),
        r.begin(), r.end());
}
bool starts_with (char c) const noexcept
    requires std::same_as<value_type, char> {
        return size() > 0 && *begin() == c;
}
bool ends_with (char c) const noexcept
    requires std::same_as<value_type, char> {
        return size() > 0 && *rbegin() == c;
}
bool starts_with (const char* s) const
    requires std::same_as<value_type, char> {
        return starts_with(str(s));
}
bool ends_with (const char* s) const
    requires std::same_as<value_type, char> {
        return ends_with(str(s));
}

auto first () { return range(begin(), begin() + min(1, size())); }
auto last  () { return range(end() - min(1, size()), end()); }

template<class T>
auto first (T r)
    requires random_access_range<T> &&
    std::same_as<typename T::value_type, value_type> {
    auto it = std::search(begin(), end(), r.begin(), r.end());
    return it == end() ? range(end(), end()) :
        range(it, it + r.size());
}
template<class T>
auto first (T r) const
    requires random_access_range<T> &&
    std::same_as<typename T::value_type, value_type> {
    auto it = std::search(begin(), end(), r.begin(), r.end());
    return it == end() ? range(end(), end()) :
        range(it, it + r.size());
}
template<class T>
auto last (T r)
    requires random_access_range<T> &&
    std::same_as<typename T::value_type, value_type> {
    auto it = std::find_end(begin(), end(), r.begin(), r.end());
    return it == end() ? range(end(), end()) :
        range(it, it + r.size());
}
template<class T>
auto last (T r) const
    requires random_access_range<T> &&
    std::same_as<typename T::value_type, value_type> {
    auto it = std::find_end(begin(), end(), r.begin(), r.end());
    return it == end() ? range(end(), end()) :
        range(it, it + r.size());
}
auto first (const char* s) const
    requires std::same_as<value_type, char> {
        return first(str(s));
}
auto first (const char* s)
    requires std::same_as<value_type, char> {
        return first(str(s));
}
auto last (const char* s) const
    requires std::same_as<value_type, char> {
        return last(str(s));
}
auto last (const char* s)
    requires std::same_as<value_type, char> {
        return last(str(s));
}

template<class T>
auto first (one_of<T> r)
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_first_of(begin(), end(), r.begin(), r.end());
    return it == end() ? range(end(), end()) :
        range(it, it + 1);
}
template<class T>
auto first (one_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_first_of(begin(), end(), r.begin(), r.end());
    return it == end() ? range(end(), end()) :
        range(it, it + 1);
}
template<class T>
auto last (one_of<T> r)
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_first_of(rbegin(), rend(), r.begin(), r.end());
    return it == rend() ? range(end(), end()) :
        range(std::next(it).base(),
            std::next(it).base() + 1);
}
template<class T>
auto last (one_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_first_of(rbegin(), rend(), r.begin(), r.end());
    return it == rend() ? range(end(), end()) :
        range(std::next(it).base(),
            std::next(it).base() + 1);
}

template<class T>
auto first (one_not_of<T> r)
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_if_not(begin(), end(), [r](auto e) {
        return std::find(r.begin(), r.end(), e) != r.end(); });
    return it == end() ? range(end(), end()) :
        range(it, it + 1);
}
template<class T>
auto first (one_not_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_if_not(begin(), end(), [r](auto e) {
        return std::find(r.begin(), r.end(), e) != r.end(); });
    return it == end() ? range(end(), end()) :
        range(it, it + 1);
}
template<class T>
auto last (one_not_of<T> r)
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_if_not(rbegin(), rend(), [r](auto e) {
        return std::find(r.begin(), r.end(), e) != r.end(); });
    return it == rend() ? range(end(), end()) :
        range(std::next(it).base(),
            std::next(it).base() + 1);
}
template<class T>
auto last (one_not_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    auto it = std::find_if_not(rbegin(), rend(), [r](auto e) {
        return std::find(r.begin(), r.end(), e) != r.end(); });
    return it == rend() ? range(end(), end()) :
        range(std::next(it).base(),
            std::next(it).base() + 1);
}

bool contains (auto what) const
{
    return first(what).size() != 0;
}

template<class T>
bool contains_only (T r) const
    requires random_access_range<T> &&
    std::same_as<typename T::value_type, value_type> {
    return std::equal(begin(), end(),
        r.begin(), r.end());
}
bool contains_only (const char* s) const
    requires std::same_as<value_type, char> {
        return contains_only(str(s));
}
template<class T>
bool contains_only (one_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    return not contains(one_not_of{r});
}
template<class T>
bool contains_only (one_not_of<T> r) const
    requires std::same_as<typename T::value_type, value_type> {
    return not contains(one_of(r));
}
