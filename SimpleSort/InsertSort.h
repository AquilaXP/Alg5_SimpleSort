#pragma once

#include "Array/IArray.h"

template< class Ty >
void InsertSort( IArray< Ty >& v )
{
    for( size_t i = 1; i < v.size(); ++i )
    {
        for( size_t j = i; j != 0; --j )
        {
            if( v.get( j ) < v.get( j - 1 ) )
                std::swap( v.get( j ), v.get( j - 1 ) );
            else
                break;
        }
    }
}

template< class Ty >
void InsertSort( IArray< Ty >& v, size_t first, size_t last )
{
    for( size_t i = first + 1; i < last; ++i )
    {
        for( size_t j = i; j != first; --j )
        {
            if( v.get( j ) < v.get( j - 1 ) )
                std::swap( v.get( j ), v.get( j - 1 ) );
            else
                break;
        }
    }
}


template< class BidirIt >
void InsertSort( BidirIt first, BidirIt last )
{
    if( first == last )
        return;

    auto firstSort = first;
    auto lastSort = std::next( first );

    auto firstUnsort = lastSort;
    auto lastUnsort = last;

    while( firstUnsort != lastUnsort )
    {
        auto cur = firstUnsort;
        auto next = std::prev( lastSort );
        while( true )
        {
            if( *cur < *next )
            {
                using std::swap;

                std::swap( *cur, *next );
            }
            else
                break;

            if( next == firstSort )
                break;

            --next;
            --cur;
        }
        ++lastSort;
        ++firstUnsort;
    }
}
