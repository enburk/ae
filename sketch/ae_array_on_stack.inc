template<class Type, size_t Capacity> struct array_on_stack
{
    typedef Type type;

    constexpr static size_t enough_bytes (size_t n){ return n == 0 ? 0 : 1 + enough_bytes (n >> 8); }

    template<size_t nn> struct size_type    { typedef size_t   type; };
    template<>          struct size_type<1> { typedef uint8_t  type; };
    template<>          struct size_type<2> { typedef uint16_t type; };
    template<>          struct size_type<3> { typedef uint32_t type; };
    template<>          struct size_type<4> { typedef uint32_t type; };

    typedef typename size_type<enough_bytes(Capacity+1)>::type Size_t;

    byte   data [Capacity*sizeof(type)];
    Size_t size_= 0; // size goes strictly below data

    size_t size       () const { return (size_t) size_; }
    size_t capacity   () const { return Capacity; }
    size_t references () const { return 1; }

    const
    type*  readable_data () const { return (type*) data; }
    type*  writable_data ()       { return (type*) data; }

    void   new_size (size_t nn){ size_ = (Size_t) nn; }

    void clear (){ for( type* p = writable_data (); p < writable_data () + size (); p++ ) p->~type (); size_= 0; }

    array_on_stack () = default;
   ~array_on_stack (){ clear (); }

    array_on_stack              ( const array_on_stack & a ){ *this = a; }
    array_on_stack & operator = ( const array_on_stack & a )
    {
        if( this == &a ) return *this; clear (); for( size_t i=0; i < a.size (); i++ )
        {
            new (writable_data () + i) type (a.readable_data ()[i]);  size_++;
        }
        return *this;
    }
    array_on_stack              ( array_on_stack && a ){ *this = std::forward (a); }
    array_on_stack & operator = ( array_on_stack && a )
    {
        if( this == &a ) return *this; clear (); for( size_t i=0; i < a.size (); i++ )
        {
            new (writable_data () + i) type (std::forward (a.readable_data ()[i]));  size_++;
        }
        return *this;
    }
};

template<class Type> struct array_on_stack<Type,0>
{
    typedef Type type;

    size_t size       () const { return 1; } // special for dynamic array
    size_t capacity   () const { return 0; }
    size_t references () const { return 1; }

    const
    type*  readable_data () const { return nullptr; }
    type*  writable_data ()       { return nullptr; }

    void   new_size (size_t){}

    array_on_stack (){}
    array_on_stack ( const array_on_stack & ){}
    array_on_stack & operator = ( const array_on_stack & a ){}
};
