#include "ipc/matrix_algorithms.h"

#include <stdexcept>
#include "common/matrix.h"

common::Matrix ipc::multiplyMatrices( const common::Matrix& lhs, const common::Matrix& rhs )
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
} // multiplyMatrices
