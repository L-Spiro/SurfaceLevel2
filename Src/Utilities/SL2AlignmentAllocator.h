/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A resampler.
 */


#pragma once

#include <cstdint>
#include <malloc.h>


namespace sl2 {

	template <typename T, size_t N = 16>
    class CAlignmentAllocator {
    public:
        typedef T value_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        typedef T * pointer;
        typedef const T * const_pointer;

        typedef T & reference;
        typedef const T & const_reference;

        public :
        inline                                                      CAlignmentAllocator() throw () {}

        template <typename T2>
        inline                                                      CAlignmentAllocator( const CAlignmentAllocator<T2, N> & ) throw () {}

        inline                                                      ~CAlignmentAllocator() throw () {}

        inline pointer                                              address( reference r ) {
        return &r;
        }

        inline const_pointer                                        address( const_reference r ) const { return &r; }

        inline pointer                                              allocate( size_type n ) { return (pointer)_aligned_malloc(n*sizeof(value_type), N); }

        inline void                                                 deallocate( pointer p, size_type ) {
            _aligned_free( p );
        }

        inline void                                                 construct( pointer p, const value_type & wert ) {
        new ( p ) value_type( wert );
        }

        inline void                                                 destroy( pointer p ) {
            p->~value_type();
        }

        inline size_type                                            max_size() const throw () {
            return size_type( -1 ) / sizeof( value_type );
        }

        template <typename T2>
        struct rebind {
            typedef CAlignmentAllocator<T2, N>                       other;
        };

        bool                                                        operator != ( const CAlignmentAllocator<T,N> &other ) const  {
            return !(*this == other);
        }

        // Returns true if and only if storage allocated from *this
        // can be deallocated from other, and vice versa.
        // Always returns true for stateless allocators.
        bool                                                        operator == ( const CAlignmentAllocator<T,N> &other ) const {
            return true;
        }
    };


}	// namespace sl2
