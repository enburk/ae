template<class Type> struct array_on_heap // fixed capacity
{
    typedef Type type;

    struct HEAD { size_t size, capacity, extra_references; };
    HEAD * head = nullptr;

    type * data () const { return (type*)(head+1); }

    size_t size       () const { return head == nullptr ? 0 : head->size; }
    size_t capacity   () const { return head == nullptr ? 0 : head->capacity; }
    size_t references () const { return head == nullptr ? 1 : head->extra_references + 1; }

    const
    type*  readable_data () const { return data (); }
    type*  writable_data ()       { copy_on_write (); return data (); }

    void   new_size (size_t nn){ if( head != nullptr || nn != 0 ) head->size = nn; }

    array_on_heap ( size_t capacity = 0 )
    {
        if( capacity > 0 )
        {
            head = (HEAD*) new byte [ sizeof(HEAD) + capacity*sizeof(type) ];
            head->size = 0;
            head->capacity = capacity;
            head->extra_references = 0;
        }
    }

   ~array_on_heap ()
    {
        if( head != nullptr ) if( head->extra_references-- == 0 )
        {
            for( type* p = data (); p < data () + size (); p++ ) p->~type ();  delete [] (byte*) head;
        }
    }

    array_on_heap              ( const array_on_heap & a ) noexcept { head = a.head; if( head != nullptr ) head->extra_references++; }
    array_on_heap & operator = ( const array_on_heap & a ) noexcept { array_on_heap b (a); std::swap (head,b.head); return *this; }

    array_on_heap              ( array_on_heap && a ) noexcept { std::swap (head, a.head); }
    array_on_heap & operator = ( array_on_heap && a ) noexcept { std::swap (head, a.head); return *this; }

    void copy_on_write ()
    {
        if( head != nullptr ) if( head->extra_references != 0 )
        {
            array_on_heap copy ( capacity () );

            type * dst = copy.data ();
            type * src = this->data ();

            size_t nn = size ();  while (nn--){ new (dst++) type (*src++); copy.head->size++; }

            std::swap ( copy.head, this->head );
        }
    }
};
