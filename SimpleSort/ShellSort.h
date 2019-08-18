#pragma once

template< class Ty, class Func >
void ShellSort( IArray<Ty>& v, Func func )
{
    if( v.size() < 2 )
        return;

    while( true )
    {
        size_t cur_step = func();
       
        for( size_t shift = 0; shift < cur_step; ++shift )
            InsertSubArrSort( v, shift, cur_step );

        if( cur_step == 1 )
            break;
    }
}

template< class Ty >
void InsertSubArrSort( IArray< Ty >& arr, size_t shift, size_t step )
{
    for( size_t i = shift; i < arr.size(); i += step )
    {
        for( size_t j = i; step <= j; j -= step )
        {
            if( arr.get( j ) < arr.get( j - step ) )
                std::swap( arr.get( j ), arr.get( j - step ) );
            else
                break;
        }
    }
}
