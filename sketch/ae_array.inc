template<class type, size_t stack_capacity = 0> class array
{
    enum { layout_capacity = stack_capacity == 0 ? 0 : max ( stack_capacity, ( sizeof(array_on_heap<type>) + sizeof(type) - 1 ) / sizeof(type) ) };

    // example of correct layout:
    // AbcAbcAbcS // 3 type elements by 3 bytes + size of stacked array
    // 12345678   // 8 bytes: 64-bit pointer to the head of heapped array

    typedef array_helper< array_on_stack <type, layout_capacity> > Stacked;
    typedef array_helper< array_on_heap  <type>                  > Heapped;
    union U
    {
        Stacked stacked;  U () : stacked (), heapped () {}
        Heapped heapped; ~U (){ if( stacked.size () <= stacked.capacity () ) stacked.~Stacked (); else heapped.~Heapped (); }
    }
    u; public:
    
    bool stacked () const { return u.stacked.size () <= u.stacked.capacity (); }
    bool heapped () const { return u.stacked.size () >  u.stacked.capacity (); }

    size_t size       () const { return stacked () ? u.stacked.size       () : u.heapped.size       (); }
    size_t capacity   () const { return stacked () ? u.stacked.capacity   () : u.heapped.capacity   (); }
    size_t references () const { return stacked () ? u.stacked.references () : u.heapped.references (); }
    const
    type*  readable_data () const { return stacked () ? u.stacked.readable_data () : u.heapped.readable_data (); }
    type*  writable_data ()       { return stacked () ? u.stacked.writable_data () : u.heapped.writable_data (); }

    chunk<const type> readable () const { return chunk<const type>( readable_data (), size () ); }
    chunk<      type> writable ()       { return chunk<      type>( writable_data (), size () ); }

    array& reserve (size_t nn)
    {
        if( nn <= capacity () ) return *this;
        if( nn == capacity () + 1 ) nn = nn*2;

        Heapped copy (nn);  references () == 1 ? copy.absorb (writable ()) : copy.append (readable ());  clear ();

        if( stacked () ){ *((void**)(writable_data ())) = nullptr; u.stacked.new_size ( capacity () + 1 ); }

        u.heapped = copy;

        return *this;
    }

    array& shrink_to_fit ()
    {
        if( size () == capacity () || stacked () ) return *this;

        if( size () > u.stacked.capacity () )
        {
            Heapped copy (nn);  references () == 1 ? copy.absorb (writable ()) : copy.append (readable ());  u.heapped = copy;
        }
        else
        {
            Heapped copy = u.heapped;  u.heapped = Heapped ();  u.stacked.new_size (0);

            copy.references () == 1 ? u.stacked.absorb (copy.writable ()) : u.stacked.append (copy.readable ());
        }

        return *this;
    }

    array& remove   ( size_t pos, size_t nn = 1 )
    {
        if( nn == 0 ) return *this;  assert_in_release ( pos + nn <= size () );
        
        if( stacked () ) u.stacked.remove (pos,nn); else
        {
            if( references () == 1 ) u.heapped.remove (pos,nn); else
            {
                Heapped copy (size () - nn);  chunk<const type> chunk1 = readable ();  chunk1.size  = pos;
                copy.append  (chunk1);        chunk<const type> chunk2 = readable ();  chunk2.size -= pos+nn; chunk2.data += pos+nn;
                copy.append  (chunk2);        u.heapped = copy;
            }
        }
            
        return *this;
    }
    array& clear    (           ){ remove ( 0, size () ); return *this; }
    array& truncate (           ){ if( size () != 0 ) remove ( size () - 1 ); return *this; }
    array& truncate ( size_t nn ){ nn = min ( nn, size () ); remove ( nn, size () - nn ); return *this; }

    array& append ( type && element )
    {
        reserve ( size () + 1 ); stacked () ? u.stacked.append (std::forward<type>(element)) : u.heapped.append (std::forward<type>(element)); return *this;
    }
    array& append ( const type & element, size_t nn = 1 )
    {
        reserve ( size () + nn ); stacked () ? u.stacked.append (element,nn) : u.heapped.append (element,nn); return *this;
    }
    array& append ( chunk<const type> copyee )
    {
        Heapped copy;

        if( size () + copyee.size > capacity () && readable_data () <= copyee.data && copyee.data < readable_data () + size () )
        {
            copy = Heapped (copyee.size);  copy.append (copyee);  copyee = copy.readable ();
        }

        reserve ( size () + copyee.size );  stacked () ? u.stacked.append (copyee) : u.heapped.append (copyee);

        return *this;
    }
    template<size_t N>
    array& append ( const array<type,N> & a )
    {
        if( size () == 0 && a.heapped () )
        {
            if( stacked () ){ *((void**)(writable_data ())) = nullptr; u.stacked.new_size ( capacity () + 1 ); }; u.heapped = a.u.heapped;
        }
        else append ( a.readable () );

        return *this;
    }


    array              (                 ){}
    array              ( const array & a ){ append (a); }
    array& operator += ( const array & a ){ append (a); return *this; }
    array& operator  = ( const array & a ){ if( this == &a ) return *this; clear (); append (a); return *this; }

    template<size_t N> array              ( const array<type,N> & a ){ append (a); }
    template<size_t N> array& operator += ( const array<type,N> & a ){ append (a); return *this; }
    template<size_t N> array& operator  = ( const array<type,N> & a ){ if( this == &a ) return *this; clear (); append (a); return *this; }

    array& operator += ( const type &  element ){ append (element); return *this; }
    array& operator += (       type && element ){ append (std::forward<type>(element)); return *this; }

    array& operator += ( chunk<const type> copyee ){ append (copyee); return *this; }

    /*
    template<size_t N> array& operator  = ( const array<type,N> & a ){ array copy; copy.append (a.readable ()); *this = copy; return *this; }





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

#include "ae_array_unit_test.inc"