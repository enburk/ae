template<class unary_predicate>
auto remove_if (unary_predicate match)
{
    auto i = begin();
    auto j = begin();
    while (j != end())
    {
        std::swap(*i, *j);
        if (not match(*i)) i++;
        j++;
    }
    return i;
}
