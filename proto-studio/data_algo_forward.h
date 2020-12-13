template<class unary_predicate>
auto remove_if (unary_predicate match)
{
    auto i = begin();
    auto j = begin();
    while (j != end())
    {
        using std::swap; swap(*i, *j);
        if (not match(*i)) i++;
        j++;
    }
    return i;
}

auto find (value_type const& element) const {
    return std::find(begin(), end(), element);
}

bool contains (value_type const& element) const {
    return find(element) != end();
}

