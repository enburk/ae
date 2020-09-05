struct Unit_test_array
{
    struct log
    {
        static auto & Log (){ static std::vector< std::string > Log_; return Log_; }

        static void put ( const std::string & s ){ Log ().push_back (s); }

        static void print (){ for(auto s : Log ()) std::cout << s << std::endl; std::cout << "=====" << std::endl; Log ().clear (); }

        static void check ( const std::string & title, const std::vector< std::string > & true_log )
        {
            if( Log () != true_log ){ std::cout << "===== " << title << " =====" << std::endl; print (); assert_in_release (false); } else Log ().clear ();
        }
    };

    struct element
    {
        std::string s;

       ~element         (                   )           { log::put ( "dtor: " + s ); }
        element         (                   ) : s ("_") { log::put ( "ctor: " + s ); }
        element         ( char            c ) : s (1,c) { log::put ( "ctor: " + s ); }
        element         ( const element & e ) : s (e.s) { log::put ( "copy: " + s ); }
        element         (       element&& e ) : s (std::forward<std::string>(e.s))
                                                        { log::put ( "move: " + s ); }
        void operator = ( char            c )           { log::put ( "assg: " + s + "=" + c   ); s = c;   }
        void operator = ( const element & e )           { log::put ( "copy: " + s + "=" + e.s ); s = e.s; }
        void operator = (       element&& e )           { log::put ( "move: " + s + "=" + e.s ); s = std::forward<std::string>(e.s); }
    };

    static void test ()
    {
        std::cout << sizeof(array<char[3],1>) << std::endl;

        //assert_in_release ( sizeof(char*) == 4 ? sizeof(array<char[3],1>) ==  4 : true );
        //assert_in_release ( sizeof(char*) == 8 ? sizeof(array<char[3],1>) == 10 : true );

        std::string title; std::vector< std::string > true_log;

        true_log = { "ctor: 1", "move: 1", "dtor: ",
                     "ctor: 2", "move: 2", "dtor: ",
                     "ctor: 3", "move: 3", "dtor: ", "move: 1=3", "dtor: 2", "-----", "dtor: 3" };

        title =                           "a += '1'; a += '2'; a += '3'; a.remove (0,2);";
        { array<element,3> a;              a += '1'; a += '2'; a += '3'; a.remove (0,2); log::put ("-----"); }; log::check (title, true_log);
        { array<element> a; a.reserve (3); a += '1'; a += '2'; a += '3'; a.remove (0,2); log::put ("-----"); }; log::check (title, true_log);

        true_log = { "ctor: 1", "move: 1", "dtor: ", "copy: 1", "copy: 1", "dtor: 1", "-----", "dtor: 1", "dtor: 1" };

        title =                           "a += '1'; a += a; a += a.readable ()[0]; a.truncate ();";
        { array<element,3> a;              a += '1'; a += a; a += a.readable ()[0]; a.truncate (); log::put ("-----"); }; log::check (title, true_log);
        { array<element> a; a.reserve (3); a += '1'; a += a; a += a.readable ()[0]; a.truncate (); log::put ("-----"); }; log::check (title, true_log);

        true_log = { "ctor: 1", "move: 1", "dtor: ", "copy: 1", "copy: 1", "dtor: 1", "-----", "dtor: 1", "dtor: 1" };

        title =                                 "a.append ('1'); c = b = a; a.clear ();";
        { array<element,3> a, b, c;              a.append ('1'); c = b = a; a.clear (); log::put ("-----"); }; log::check (title, true_log);

        true_log = { "ctor: 1", "move: 1", "dtor: ", "-----", "dtor: 1" };

        title =                                 "a.append ('1'); c = b = a; a.clear ();";
        { array<element> a, b, c; a.reserve (3); a.append ('1'); c = b = a; a.clear (); log::put ("-----"); }; log::check (title, true_log);

        assert_in_release (( array<element,1>().capacity () == 1 )); // bacause sizeof (std::string) >= sizeof (void*)

        true_log = { "ctor: 1", "move: 1", "dtor: ",
                     "ctor: 2", "move: 2", "dtor: ",
                     "ctor: 3", "move: 1", "move: 2", "dtor: ", "dtor: ", "move: 3", "dtor: ", "-----",
                     "dtor: 1", "dtor: 2", "dtor: 3" };

        title =              "a += '1'; a += '2'; a += '3'";
        { array<element,2> a; a += '1'; a += '2'; a += '3'; log::put ("-----"); }; log::check (title, true_log);
        { array<element>   a; a += '1'; a += '2'; a += '3'; log::put ("-----"); }; log::check (title, true_log);

        //{ array<element,2> a; a += '1'; a += '2'; a += '3'; }; log::print ();
        //{ array<element>   a; a += '1'; a += '2'; a += '3'; }; log::print ();

        int i = 5;
        int j = 6;
        int k = max (i,j);

        array<char> a;
        array<char,1> b;

        std::cout << std::string (a.stacked () ? "on stack" : "on heap") << std::endl;
        std::cout << std::string (b.stacked () ? "on stack" : "on heap") << std::endl;

        /*
        auto nn = [=]( std::string s )
        {
            container<int> result (99);  for( int i=0; i<s.size(); i++ ) result.append ( s [i] );  return result;
        };

        assert_in_release ( [=]()
        {
            container<int> a = nn ("012345"); chunk<int> c = a.writable (); c>>=1; c<<=1; a.replace (3,3,c); return a == nn ("0121234");
        }());
        */
    }
};
inline void unit_test_array (){ Unit_test_array::test (); }
