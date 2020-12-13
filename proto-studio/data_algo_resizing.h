void replace_by (input_range auto r)
{
    int n = min(size(), r.size());
    for (int i=0; i<n; i++) (*this)[i] = r[i];
    if (size() > r.size()) from(begin()+n).erase();
    else insert(end(), r.from(r.begin()+n));
}

template<class unary_predicate>
void erase_if (unary_predicate match)
{
    from(remove_if(match)).erase();
}
