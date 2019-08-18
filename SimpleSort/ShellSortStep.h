#pragma once

#include <vector>
#include <utility>

struct ShellStep
{
    ShellStep( size_t N = 1 )
        : m_N( N )
    {
    }
    size_t operator()()
    {
        if( m_N == 1 )
            return 1;
        m_N /= 2;
        return m_N;
    }

private:
    size_t m_N = 1;
};

struct FrankLazarusStep
{
    FrankLazarusStep( size_t N )
        : m_N( N )
    {
        m_N /= 2;
    }
    size_t operator()()
    {
        m_N /= 2;
        return  2 * m_N + 1;
    }

private:
    size_t m_N = 1;
};

struct Sedgewick1982Step
{
    Sedgewick1982Step( size_t N )
        : m_N( N )
    {
        size_t k = 1;
        m_seq.emplace_back( 1 );
        while( true )
        {
            size_t v = size_t(std::pow( size_t(4), k ) + 3 * std::pow( size_t(2), k - 1 ) + 1);
            if( m_N <= v )
                break;

            m_seq.emplace_back( v );
            ++k;
        }
        m_cur_pos = m_seq.size();
    }
    size_t operator()()
    {
        if( m_cur_pos != 0 )
            --m_cur_pos;

        return m_seq[m_cur_pos];
    }

private:
    std::vector< size_t > m_seq;
    size_t m_cur_pos = 0;
    size_t m_N = 1;
};

struct Ciura
{
    size_t operator()()
    {
        if( m_cur_pos != 0 )
            --m_cur_pos;

        return m_seq[m_cur_pos];
    }
private:
    size_t m_seq[8] = { 1, 4, 10, 23, 57, 132, 301, 701 };
    size_t m_cur_pos = std::size( m_seq );
};
