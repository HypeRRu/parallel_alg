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
    for ( size_t row = 0; row < lhs.rows(); ++row )
    {
        for ( size_t column = 0; column < rhs.columns(); ++column )
        {
            result.set( row, column, common::Matrix::multiplyRC( lhs, rhs, row, column ) );
        }
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

    for ( size_t row = 0; row < lhs.rows(); ++row )
    {
        for ( size_t column = 0; column < rhs.columns(); ++column )
        {
            multiplyThreads.emplace_back( []( const common::Matrix& lhs, const common::Matrix& rhs
                                            , common::Matrix& result, size_t row, size_t column )
            {
                result.set( row, column, common::Matrix::multiplyRC( lhs, rhs, row, column ) );
            }, std::cref( lhs ), std::cref( rhs ), std::ref( result ), row, column );
        }
    }

    for ( auto& thread: multiplyThreads )
    {
        thread.join();
    }
    return result;
} // multiplyMatricesParallel
