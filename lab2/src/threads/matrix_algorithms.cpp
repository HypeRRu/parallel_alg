#include "threads/matrix_algorithms.h"

#include <stdexcept>
#include <thread>
#include <vector>
#include "common/matrix.h"

common::Matrix threads::multiplyMatricesSerial( const common::Matrix& lhs, const common::Matrix& rhs )
{
    if ( lhs.columns() != rhs.rows() )
    {
        std::runtime_error{ "Cannot multiply matrices: invalid sizes" };
    }
    common::Matrix result{ lhs.rows(), rhs.columns() };
    for ( size_t pos = 0; pos < lhs.columns(); ++pos )
    {
        common::Matrix::multiplyRC( lhs, rhs, result, pos );
    }
    return result;
} // multiplyMatricesSerial


common::Matrix threads::multiplyMatricesParallel( const common::Matrix& lhs, const common::Matrix& rhs )
{
    if ( lhs.columns() != rhs.rows() )
    {
        std::runtime_error{ "Cannot multiply matrices: invalid sizes" };
    }
    common::Matrix result{ lhs.rows(), rhs.columns() };

    std::vector< std::thread > multiplyThreads;
    multiplyThreads.reserve( lhs.columns() );

    for ( size_t pos = 0; pos < lhs.columns(); ++pos )
    {
        multiplyThreads.emplace_back( &common::Matrix::multiplyRC
            , std::cref( lhs ), std::cref( rhs ), std::ref( result ), pos );
    }

    for ( auto& thread: multiplyThreads )
    {
        thread.join();
    }
    return result;
} // multiplyMatricesParallel
