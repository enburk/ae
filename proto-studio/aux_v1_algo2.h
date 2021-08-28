//#define const
//#include "aux_v1_algo1.h"
//#undef const
//
//
//template<class range>
//void replace_by (range r)
//{
//    int n = min(size(), r.size());
//    for (int i=0; i<n; i++) (*this)[i] = r[i];
//    if (size() > r.size()) from(begin()+n).erase();
//    else insert(end(), r.from(r.begin()+n));
//}
//void replace_by (const char * r)
//    requires std::same_as<value_type, char> {
//        return replace_by(str(r));
//}

//int replace_all (
//    random_access_range auto what,
//    random_access_range auto with)
//{
//    int nn = 0;
//    int pos = 0;
//    while (true) {
//        auto range = from(begin()+pos).first(what); if (range.size() == 0) break;
//        pos = range.begin() - begin() + with.size();
//        range.replace_by(with);
//        nn++;
//    };
//    return nn;
//}
//int replace_all (const char * what, const char * with)
//    requires std::same_as<value_type, char> {
//        return replace_all(str(what), str(with));
//}
//
//template<class unary_predicate>
//void erase_if (unary_predicate match)
//{
//    from(remove_if(match)).erase();
//}


