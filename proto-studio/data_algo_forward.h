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
    return range(i, end());
}


