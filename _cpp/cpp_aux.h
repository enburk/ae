template<class type> struct deque : public std::deque<type>
{
    using base = std::deque<type>;

    int size () const { return (int) base::size (); }

    void operator += (const type  & e) { base::push_back (e); }
    void operator += (const deque & a) { base::insert (base::end (), a.begin (), a.end ()); }
};

template<class type> struct array : public std::vector<type>
{
    using base = std::vector<type>;

    int size () const { return (int) base::size (); }

    void operator += (const type  & e) { base::push_back (e); }
    void operator += (const array & a) { base::insert (base::end (), a.begin (), a.end ()); }

    friend array operator + (const array & a, const type  & b) { array tt; tt += a; tt+= b; return tt; }
    friend array operator + (const type  & a, const array & b) { array tt; tt += a; tt+= b; return tt; }
};

struct string : public std::string
{
    using base = std::string;

    string (               ) : base ( ) {}
    string (const base & s ) : base (s) {}
    string (const char * s ) : base (s) {}
    string (char c, int n=1) { if (n > 0) *this = base (n,c); }

    int size () const { return (int) base::size (); }

    string & operator  = (const string & s){ base::operator  = (s); return *this; }
    string & operator += (const string & s){ base::operator += (s); return *this; }

    const int max = std::numeric_limits<int>::max ();

    string substr (int pos, int num) const { if (pos < 0) num += pos, pos = 0; num = std::min (num, size () - pos); return num > 0 ? base::substr (pos, num) : base (); }

    string from (int pos) const { return substr (pos, max); }
    string head (int num) const { return substr (0, num); }
    string tail (int num) const { return substr (size () - num, max); }

    bool headed (string s) const { if (size () < s.size ()) return false; for (int i=0; i<s.size (); i++) if ((*this) [          i] != s [            i]) return false; return true; }
    bool tailed (string s) const { if (size () < s.size ()) return false; for (int i=0; i<s.size (); i++) if ((*this) [size ()-1-i] != s [s.size ()-1-i]) return false; return true; }

    void append    (                  string s){ *this += s; }
    void insert    (int pos,          string s){ *this = head (pos) + s + from (pos); }
    void overwrite (int pos,          string s){ erase (pos, s.size ()); insert (pos, s); }
    void replace   (int pos, int num, string s){ erase (pos, num      ); insert (pos, s); }
    void erase     (int pos, int num = 1      ){ if (pos < 0) num += pos, pos = 0; num = std::min (num, size () - pos); if (num > 0) base::erase (pos, num); }
    void truncate  (int pos                   ){ erase (pos, max); }
    void truncate  (                          ){ erase (size () - 1); }
    void strip     (const string & set = " "  ){ trimr (set); triml (set); }
    void triml     (const string & set = " "  ){ size_type n = find_first_not_of (set); *this = n == npos ? string () : from ((int) n  ); }
    void trimr     (const string & set = " "  ){ size_type n = find_last_not_of  (set); *this = n == npos ? string () : head ((int) n+1); }
};
