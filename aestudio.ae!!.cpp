#include <span>
#include <cstdint>
namespace ae
{
    namespace data
    {
        template<typename type_x, typename type_f> auto ae__gt__gt__gt_(type_x x, type_f f)
        {
            f(x);
        };
        template<typename type_f, typename type_x> auto ae__lt__lt__lt_(type_f f, type_x x)
        {
            f(x);
        };
        template<typename x> struct pointer
        // [in scope: array, collection, contiguous.collection, contiguous.collection.range, pointer, system]
        {
            meta::address address;
            auto value()
            {
                meta::value_of(x)::at(address);
            };
        };
        template<typename x> struct collection
        // [in scope: array, collection, contiguous.collection, contiguous.collection.range, pointer, system]
        {
            int size();
            auto ae__tilda__equal_(x element);
            auto ae__tilda__equal_(range(x) r);
        };
        template<typename x> struct contiguous_collection
        // [in scope: array, collection, contiguous.collection, contiguous.collection.range, pointer, system]
        {
            struct iterator
            {
                contiguous_iterator;
            };
            struct sentinel
            {
                contiguous_iterator;
            };
            int size();
            auto size(int size);
            auto first()
            {
                this[0];
            };
            auto last()
            {
                this[( size , - , 1 )];
            };
            auto ae__tilda__equal_(collection(x) c)
            {
                auto i =
                {
                    end;
                }
                data::+= ( size, c_size );
            };
            auto ae__tilda__equal_(x e)
            {
                ( size, ++ );
                constexpr auto last =
                {
                    ( =, e );
                }
            };
        };
        template<typename x> struct contiguous_collection_range
        // [in scope: array, collection, contiguous.collection, contiguous.collection.range, pointer, system]
        {
            pointer(contiguous_collection(x)) collection;
        };
        struct type_system
        {
            struct type_memory
            {
                contiguous_range(byte) request(int bytes);
                auto release(contiguous_range(byte) range);
            };
            type_memory memory;
        };
        type_system system;
        template<typename x> struct array
        // [in scope: array, collection, contiguous.collection, contiguous.collection.range, pointer, system]
        {
            struct type_detail
            {
                ( integer, size );
                contiguous_range(byte) data;
            };
            type_detail detail;
            constexpr auto destructor =
            {
                =;
            }
            auto begin()
            {
                contiguous_iterator(x)(detail::data::address);
            };
            auto end()
            {
                data::+ ( begin, size );
            };
            auto from(iterator i)
            {
                contiguous_collection_range(x)(clip(i), end);
            };
            auto upto(iterator i)
            {
                contiguous_collection_range(x)(begin, clip(i));
            };
            int size()
            {
                detail::size;
            };
            auto size(int n)
            {
                constexpr auto n =
                {
                    ( =, max(n, 0) );
                }
                data::← ( detail::size, n );
            };
            int capacity()
            {
                data::/ ( detail::data::size, meta::size_of(x) );
            };
            auto capacity(int n)
            {
                auto new_data =
                {
                    system::memory::request(( n , * , meta::size_of(x) ));
                }
                data_copy_to(new_data);
                swap(data, new_data);
            };
        };
    }
    namespace data.algorithms
    {
        template<typename ae_args___> struct predicate
        // [in scope: bidirectional.range, binary.predicate, predicate, random.access.range, range, unary.predicate]
        {
        };
        template<typename x, typename y> struct binary_predicate
        // [in scope: bidirectional.range, binary.predicate, predicate, random.access.range, range, unary.predicate]
        {
        };
        template<typename x> struct unary_predicate
        // [in scope: bidirectional.range, binary.predicate, predicate, random.access.range, range, unary.predicate]
        {
        };
        template<typename type_a, typename type_b> auto swap(type_a a, type_b b)
        {
            auto c =
            {
                a;
            }
            data::← ( a, b );
            data::← ( b, c );
        };
    }
    namespace data.iterators
    {
        template<typename x> struct forward_iterator
        // [in scope: bidirectional.iterator, contiguous.iterator, forward.iterator, integer.iterator, random.access.iterator]
        {
            x value();
            auto value(x value);
            auto ++();
        };
        template<typename x> struct bidirectional_iterator
        // [in scope: bidirectional.iterator, contiguous.iterator, forward.iterator, integer.iterator, random.access.iterator]
        {
            x value();
            auto value(x value);
            auto ++();
            auto --();
        };
        template<typename x> struct random_access_iterator
        // [in scope: bidirectional.iterator, contiguous.iterator, forward.iterator, integer.iterator, random.access.iterator]
        {
            auto ++()
            {
                data::+= ( this, 1 );
            };
            auto --()
            {
                data::-= ( this, 1 );
            };
            auto +(int n)
            {
                auto i =
                {
                    this;
                }
                data::+= ( i, n );
            };
            auto -(int n)
            {
                auto i =
                {
                    this;
                }
                data::-= ( i, n );
            };
            auto +=(int n);
            auto -=(int n);
            int -(same i);
        };
        template<typename x> struct contiguous_iterator
        // [in scope: bidirectional.iterator, contiguous.iterator, forward.iterator, integer.iterator, random.access.iterator]
        {
            meta::address address;
            auto value()
            {
                meta::value_of(x)::at(address);
            };
            auto value(x v)
            {
                data::← ( meta::value_of(x)::at(address), v );
            };
            auto +=(int n)
            {
                ( address, +=, n, *, meta::size_of(x) );
            };
            auto -=(int n)
            {
                ( address, -=, n, *, meta::size_of(x) );
            };
            auto -(same i)
            {
                data::/ ( (, meta::size_of(x) );
            };
        };
        struct integer_iterator
        // [in scope: bidirectional.iterator, contiguous.iterator, forward.iterator, integer.iterator, random.access.iterator]
        {
            ( integer, value );
            auto ++()
            {
                ( value, ++ );
            };
            auto --()
            {
                ( value, -- );
            };
            auto +=(int n)
            {
                data::+= ( value, n );
            };
            auto -=(int n)
            {
                data::-= ( value, n );
            };
            auto -(same i)
            {
                data::- ( value, i_value );
            };
        };
    }
    namespace data.ranges
    {
        template<typename x> struct range
        // [in scope: contiguous.range, integer, integer.range, random.access.range, range]
        {
            struct iterator
            {
                forward_iterator(x);
            };
            struct sentinel;
            iterator begin();
            sentinel end();
        };
        template<typename x> struct random_access_range
        // [in scope: contiguous.range, integer, integer.range, random.access.range, range]
        {
            struct iterator
            {
                random_access_iterator;
            };
            struct sentinel;
            same from(iterator i);
            same upto(iterator i);
            auto operator ()(iterator i, iterator j)
            {
                data::ae__ ( from(i), upto(( j , + , 1 )) );
            };
            auto operator ()(iterator i, iterator j)
            {
                data::ae__ ( from(i), upto(j) );
            };
            auto from(int n)
            {
                from(( begin , + , n ));
            };
            auto upto(int n)
            {
                upto(( begin , + , n ));
            };
            auto operator ()(int b, int e)
            {
                data::ae__ ( from(b), upto(( e , - , b , + , 1 )) );
            };
            auto operator ()(int b, int e)
            {
                data::ae__ ( from(b), upto(( e , - , b )) );
            };
            auto operator ()(int i)
            {
                data:::: ( (, value );
            };
            auto operator ()(int i)
            {
                data:::: ( (, value );
            };
        };
        template<typename x> struct contiguous_range
        // [in scope: contiguous.range, integer, integer.range, random.access.range, range]
        {
            struct iterator
            {
                contiguous_iterator;
            };
            ( iterator, begin );
            struct sentinel
            {
                contiguous_iterator;
            };
            ( iterator, end );
            auto size()
            {
                data::- ( end, begin );
            };
            auto clip(iterator i)
            {
                min(max(begin, i), end);
            };
            auto from(iterator i)
            {
                same(clip(i), end);
            };
            auto upto(iterator i)
            {
                same(begin, clip(i));
            };
        };
        struct integer_range
        // [in scope: contiguous.range, integer, integer.range, random.access.range, range]
        {
            struct iterator
            {
                integer_iterator;
            };
            ( iterator, begin );
            struct sentinel
            {
                integer_iterator;
            };
            ( iterator, end );
            auto size()
            {
                data::- ( end, begin );
            };
            auto clip(iterator i)
            {
                min(max(begin, i), end);
            };
            auto from(iterator i)
            {
                same(clip(i), end);
            };
            auto upto(iterator i)
            {
                same(begin, clip(i));
            };
        };
    }
    namespace data.text
    {
        struct symbol
        // [in scope: doc, string, symbol, text]
        {
        };
        struct string
        // [in scope: doc, string, symbol, text]
        {
        };
        struct text
        // [in scope: doc, string, symbol, text]
        {
            struct line
            {
            };
        };
        struct doc
        // [in scope: doc, string, symbol, text]
        {
        };
    }
    namespace gui
    {
        struct screen
        // [in scope: screen, window]
        {
            struct type_size
            {
                int x, y;
            };
            type_size size;
        };
        struct window
        // [in scope: screen, window]
        {
            auto create();
        };
    }
    namespace math
    {
        double ae__sqrt_(double x);
        constexpr auto golden_ratio =
        {
            ( =, (, /, 2 );
        }
    }
    namespace meta
    {
        struct size;
        // [in scope: address, size, value.of]
        struct address;
        // [in scope: address, size, value.of]
        template<typename x> struct value_of
        // [in scope: address, size, value.of]
        {
            x at(address a);
        };
    }
    namespace pix
    {
        struct size
        // [in scope: colors, image, size]
        {
            math::vector(2, integer);
        };
        struct type_colors
        {
            struct RGBA
            {
            };
        };
        type_colors colors;
        template<typename color> struct image
        // [in scope: colors, image, size]
        {
            size size;
        };
    }
    namespace system
    {
        auto out(contiguous_range(byte) data);
        auto err(contiguous_range(byte) data);
        gui::window window;
        gui::screen screen;
        std::span<gui::screen> screens;
    }
    void main_()
    {
    }
}
