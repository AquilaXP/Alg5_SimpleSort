#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include <chrono>
#include <tuple>
#include <fstream>

#include "Array/VectorArray.h"

#include "InsertSort.h"
#include "ShellSort.h"
#include "ShellSortStep.h"

/// Создать последовательность и перемешать
VectorArray<int> Shuffle( size_t count )
{
    VectorArray<int> res( count );

    std::vector<int> s( count );
    std::iota( std::begin( s ), std::end( s ), 0 );
    std::random_shuffle( std::begin( s ), std::end( s ) );

    std::for_each( std::begin( s ), std::end( s ), [&]( auto v ){ res.add( v ); } );

    return res;
}

/// Создать последовательность и перемешать count_shuffle элементов
VectorArray<int> ShufflePartElement( size_t count, size_t count_shuffle )
{
    std::vector<int> arr( count );
    std::iota( std::begin( arr ), std::end( arr ), 0 );
    VectorArray<int> res( count );
    std::for_each( std::begin( arr ), std::end( arr ), [&]( auto v ){ res.add( v ); } );

    if( count_shuffle < 2 )
        return res;

    std::mt19937_64 engine(1);
    std::uniform_int_distribution<size_t> dist{ 0, count - 1 };

    // используем set чтобы не было дубликатов
    std::set<size_t> s;
    while( s.size() < count_shuffle )
    {
        s.insert( dist( engine ) );
    }
    // перемешиваем индексы которые будем менять между собой
    std::vector<size_t> random_pos( std::begin( s ), std::end( s ) );
    std::random_shuffle( std::begin( random_pos ), std::end( random_pos ) );

    // мешаем
    for( size_t i = 1; i < random_pos.size(); ++i )
    {
        size_t index1 = random_pos[i - 1];
        size_t index2 = random_pos[i];
        std::swap( res.get( index1 ), res.get( index2 ) );
    }
    return res;
}

/// Создать последовательность и перемешать precent % элементов
VectorArray<int> ShufflePrecent( size_t count, size_t precent )
{
    size_t count_shufle = size_t(count * precent * 0.01);
    return ShufflePartElement( count, count_shufle );
}

class Tester
{
    static const std::string acc;
    using acc_t = std::chrono::milliseconds;

    struct ResTest
    {
        std::string name;
        std::string seq;
        std::tuple<size_t, size_t, size_t> times;
        size_t count;
        std::string acc;
    };
public:
    

    void Execute( size_t maxCount )
    {
        std::vector< std::vector< ResTest > > resuts;
        std::vector< size_t > counts;
        for( size_t i = 20; i <= maxCount; i *= 2 )
        {
            resuts.emplace_back( ExecC( i ) );
            counts.emplace_back( i );
        }

        std::ofstream ofs( "Sorts.csv" );
        if( !ofs )
            throw std::runtime_error( "not open file Sorts.csv" );

        ofs << "NameAlg;Seq;count;Time Random," << acc <<
            ";Time Random 5, " << acc <<
            ";Time Random 10%, " + acc + "\n";
        for( size_t i = 0; i < counts.size(); ++i )
        {
            for( size_t j = 0; j < resuts[0].size(); ++j )
            {
                auto& res = resuts[i][j];
                ofs << res.name << ';' << res.seq << ';' <<
                    res.count << ';' <<
                    std::get<0>( res.times ) << ';' <<
                    std::get<1>( res.times ) << ';' <<
                    std::get<2>( res.times ) << '\n';
            }
        }
    }
private:
    std::vector< ResTest > ExecC( size_t count )
    {
        std::cout << count << '\n';
        
        std::vector< ResTest > resultes;

        ResTest res = LaunchTestAlg( "InsertSort", "-", count,
            []( IArray<int>& arr ){ InsertSort( arr ); } );
        resultes.emplace_back( res );

        res = LaunchTestAlg( "ShellSort", "step-1", count,
            [count]( IArray<int>& arr ){ ShellSort( arr, ShellStep( 1 ) ); } );
        resultes.emplace_back( res );

        res = LaunchTestAlg( "ShellSort", "ShellStep", count,
            [count]( IArray<int>& arr ){ ShellSort( arr, ShellStep( count ) ); } );
        resultes.emplace_back( res );

        res = LaunchTestAlg( "ShellSort", "FrankLazarusStep", count,
            [count]( IArray<int>& arr ){ ShellSort( arr, FrankLazarusStep( count ) ); } );
        resultes.emplace_back( res );

        res = LaunchTestAlg( "ShellSort", "Sedgewick1982Step", count,
            [count]( IArray<int>& arr ){ ShellSort( arr, Sedgewick1982Step( count ) ); } );
        resultes.emplace_back( res );

        res = LaunchTestAlg( "ShellSort", "Ciura", count,
            []( IArray<int>& arr ){ ShellSort( arr, Ciura() ); } );
        resultes.emplace_back( res );

        return resultes;
    }

    template< class Func >
    auto LaunchTestAlg( const std::string& name, const std::string& seq, size_t count, Func func )
    {
        ResTest algSort;
        algSort.name = name;
        algSort.seq = seq;
        algSort.count = count;
        algSort.times = Tests( count, func );
        std::cout << name << '-' << seq << '\n';
        return algSort;
    }

    template< class Func >
    auto Tests( size_t count, Func func )
    {
        auto arr = Shuffle( count );
        auto arr5 = ShufflePartElement( count, 5 );
        auto arr10 = ShufflePrecent( count, 10 );

        return std::make_tuple( 
            Test( arr, func ),
            Test( arr5, func ),
            Test( arr10, func ) );
    }

    template< class Func >
    size_t Test( IArray<int>& arr, Func func )
    {
        using namespace std::chrono;
        auto s = high_resolution_clock::now();
        func( arr );
        auto e = high_resolution_clock::now();

        for( size_t i = 1; i < arr.size(); ++i )
            if( arr.get( i - 1 ) > arr.get( i ) )
                throw std::runtime_error( "incorect algorithm sort!" );

        return size_t(duration_cast<acc_t>( e - s ).count());
    }
};

const std::string Tester::acc = "milliseconds";

int main()
{
    try
    {
        Tester tester;
        tester.Execute( 100000 );
    }
    catch( const std::exception& err )
    {
        std::cerr << err.what() << '\n';
        return 1;
    }

    return 0;
}
