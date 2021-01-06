bool empty () const { return begin() == end(); }

template<class unary_predicate>
auto remove_if (unary_predicate match)
{
    auto i = begin();
    auto j = begin();
    while (j != end())
    {
        using std::swap; swap(*i, *j);
        if (not match(*i)) ++i;
        ++j;
    }
    return i;
}

template<class unary_predicate>
auto find_if (unary_predicate match)
{
    auto i = begin();
    while (i != end())
    {
        if (match(*i)) return i;
        ++i;
    }
    return i;
}

auto find (value_type const& element) const
{
    return std::find(begin(), end(), element);
}

bool contains (value_type const& element) const
{
    return find(element) != end();
}

//generator<std::pair<int, element&>>
auto enumerate() const
{
    std::vector<std::pair<int, value_type const&>> ee;

    int n = size(); ee.reserve(n);

    for (int i=0; i<n; i++) ee.emplace_back(i, *(begin()+i));

    return ee;
}