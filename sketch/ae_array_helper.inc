//============================================================================================================================//
template<class Array> struct array_helper : public Array
{
    using typename
          Array::type;
    using Array::size;
    using Array::new_size;
    using Array::capacity;
    using Array::references;
    using Array::readable_data;
    using Array::writable_data;

    array_helper () = default;
    array_helper (size_t capacity) : Array (capacity) {}

    chunk<const type> readable () const { return chunk<const type>( readable_data (), size () ); }
    chunk<      type> writable ()       { return chunk<      type>( writable_data (), size () ); }

    void remove ( size_t pos, size_t nn )
    {
        if( nn == 0 ) return;

        type* data = writable_data ();

        type* dst = data + pos;
        type* src = data + pos + nn;

        while ( src != data + size ()  ) *dst++ = std::move (*src++);
        while ( dst != data + pos + nn ) (dst++)->~type ();

        new_size ( size () - nn );
    }

    void append ( type && element )
    {
        new ( writable_data () + size () ) type (std::forward<type>(element)); new_size ( size () + 1 );
    }
    void append ( const type & element, size_t nn )
    {
        type* data = writable_data ();  while (nn--){ new ( data + size () ) type (element); new_size ( size () + 1 ); }
    }
    void append ( chunk<const type> copyee )
    {
        type* data = writable_data ();  while (copyee.size--){ new ( data + size () ) type (*copyee.data++ ); new_size ( size () + 1 ); }
    }
    void absorb ( chunk<type> movyee )
    {
        type* data = writable_data ();  while (movyee.size--){ new ( data + size () ) type (std::move(*movyee.data++)); new_size ( size () + 1 ); }
    }

    /*
    friend bool operator != ( const container & c1, const container & c2 ){ return ! ( c1 == c2 ); }
    friend bool operator == ( const container & c1, const container & c2 )
    {
        if( c1.head    == c2.head    ) return true;
        if( c1.size () != c1.size () ) return false;

        const type * p1 = c1.data ();
        const type * p2 = c2.data ();

        size_t nn = c1.size (); while (nn--) if( *p1++ != *p2++ ) return false;

        return true;
    }

    void insert ( size_t pos, const Type & element, size_t nn = 1 )
    {
        assert_in_release ( size () + nn <= capacity () );

        while (nn--){ new ( data () + size () ) Type (element); size_( size+1 ); }
    }

    void insert ( size_t pos, chunk<const Type> copyee )
    {
        assert_in_release ( size () + copyee.size <= capacity () );

        while (copee.size--){ new ( data () + size () ) Type (*copyee.pointer++); size_( size+1 ); }
    }

    void replace ( size_t start, size_t finish, chunk<const type> replacer )
    {
        assert_in_debug( start  <= size () );
        assert_in_debug( finish <= size () );
        assert_in_debug( start  <= finish  );
        assert_in_debug( size () - (finish-start) + replacer.size <= capacity () );

        copy_on_write ();

        type * ptr_start  = data () + start;
        type * ptr_finish = data () + finish;
        type * ptr_end    = data () + size ();

        try
        {
            while ( ptr_start < ptr_finish && replacer.size != 0 )
            {
                *ptr_start++ = *replacer.pointer;  replacer >>= 1;
            }
            if ( ptr_start < ptr_finish ) // && replacer.size == 0 )
            {
                while ( ptr_finish < ptr_end ) *ptr_start++ = *ptr_finish++;

                while ( ptr_start < ptr_end ){ (ptr_start++)->~type(); head->size--; }
            }
            else if( replacer.size != 0 ) // && ptr_start == ptr_finish
            {
                if( replacer.pointer + replacer.size <= ptr_start ||
                    replacer.pointer                 >= ptr_end )
                {
                }
                else
                {
                }
            }
        }
        catch (...){ throw; } // rollback is imposibble, state is inconsistent
    }


    */


};
