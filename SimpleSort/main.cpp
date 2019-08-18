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

VectorArray<int> Shuffle( size_t count )
{
    VectorArray<int> res( count );

    std::vector<int> s( count );
    std::iota( std::begin( s ), std::end( s ), 0 );
    std::random_shuffle( std::begin( s ), std::end( s ) );

    std::for_each( std::begin( s ), std::end( s ), [&]( auto v ){ res.add( v ); } );

    return res;
}

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

        const size_t i = 0;

        std::vector< ResTest > reses;

        ResTest insertSort;
        insertSort.name = "InsertSort";
        insertSort.seq = "-";
        insertSort.count = count;
        insertSort.times = Tests<acc_t>(
            count, []( IArray<int>& arr ){ InsertSort( arr ); } );
        reses.emplace_back( insertSort );

        std::cout << reses.back().name << '\n';

        ResTest shellSort_1;
        shellSort_1.name = "ShellSort";
        shellSort_1.seq = "1";
        shellSort_1.count = count;
        shellSort_1.times = Tests< acc_t >(
            count, [count]( IArray<int>& arr ){ ShellSort( arr, ShellStep( 1 ) ); } );
        reses.emplace_back( shellSort_1 );

        std::cout << reses.back().name << '\n';

        ResTest shellSort;
        shellSort.name = "ShellSort";
        shellSort.seq = "ShellStep";
        shellSort.count = count;
        shellSort.times = Tests< acc_t >(
            count, [count]( IArray<int>& arr ){ ShellSort( arr, ShellStep( count ) ); } );
        reses.emplace_back( shellSort );

        std::cout << reses.back().name << '\n';

        ResTest shellSort2;
        shellSort2.name = "ShellSort";
        shellSort2.seq = "FrankLazarusStep";
        shellSort2.count = count;
        shellSort2.times = Tests< acc_t >(
            count, [count]( IArray<int>& arr ){ ShellSort( arr, FrankLazarusStep( count ) ); } );
        reses.emplace_back( shellSort2 );

        std::cout << reses.back().name << '\n';

        ResTest shellSort3;
        shellSort3.name = "ShellSort";
        shellSort3.seq = "Sedgewick1982Step";
        shellSort3.count = count;
        shellSort3.times = Tests< acc_t >(
            count, [count]( IArray<int>& arr ){ ShellSort( arr, Sedgewick1982Step( count ) ); } );
        reses.emplace_back( shellSort3 );

        std::cout << reses.back().name << '\n';

        ResTest shellSort4;
        shellSort4.name = "ShellSort";
        shellSort4.seq = "Ciura";
        shellSort4.count = count;
        shellSort4.times = Tests< acc_t >(
            count, []( IArray<int>& arr ){ ShellSort( arr, Ciura() ); } );
        reses.emplace_back( shellSort4 );

        std::cout << reses.back().name << '\n';

        return reses;
    }

    void Save( const std::vector<ResTest>& results, const std::string& name, const std::string& acc )
    {
        std::ofstream ofs( name );
        if( !ofs )
            throw std::runtime_error( "not open file " + name );

        ofs << "NameAlg;Seq;Time Random," << acc <<
            ";Time Random 5, " << acc <<
            ";Time Random 10%, " + acc + "\n";
        for( auto& res : results )
        {
            ofs << res.name << ';' << res.seq << ';' <<
                std::get<0>( res.times ) << ';' <<
                std::get<1>( res.times ) << ';' <<
                std::get<2>( res.times ) << '\n';
        }
    }

    template<class Acc,class Func>
    auto Tests( size_t count, Func func )
    {
        auto arr = Shuffle( count );
        auto arr5 = ShufflePartElement( count, 5 );
        auto arr10 = ShufflePrecent( count, 10 );

        return std::make_tuple( 
            Test<Acc>( arr, func ),
            Test<Acc>( arr5, func ),
            Test<Acc>( arr10, func ) );
    }
    template<class Acc, class Func>
    size_t Test( IArray<int>& arr, Func func )
    {
        using namespace std::chrono;
        auto s = high_resolution_clock::now();
        func( arr );
        auto e = high_resolution_clock::now();

        for( size_t i = 1; i < arr.size(); ++i )
            if( arr.get( i - 1 ) > arr.get( i ) )
                throw std::runtime_error( "incorect algorithm sort!" );

        return size_t(duration_cast<Acc>( e - s ).count());
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