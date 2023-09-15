#include "threads/tasks.h"

#include "common/defs.h"
#include "threads/matrix_algorithms.h"

#include <iostream>
#include <fstream>


#define DEBUG

threads::TaskGen&& threads::Tasks::generateMatrices()
{
    constexpr size_t rows1 = 30;
    constexpr size_t columns1Rows2 = 40;
    constexpr size_t columns2 = 50;
    
    threads::TaskGen task{ { 0, 0 }, { 0, 0 } };
    task.first = threads::generateMatrix( rows1, columns1Rows2 );
#ifdef DEBUG
    std::cout << "Generated matrix lhs\n";
    common::Matrix::write( task.first, std::cout );
#endif // DEBUG
    task.second = threads::generateMatrix( columns1Rows2, columns2 );
#ifdef DEBUG
    std::cout << "Generated matrix rhs\n";
    common::Matrix::write( task.second, std::cout );
#endif // DEBUG
    return std::move( task );
} // generateMatrices


void threads::Tasks::multiplyMatricesSerial( const TaskGen& task, ThreadFabric< TaskOut >& outQueue )
{
    const TaskOut& taskOut = threads::multiplyMatricesSerial( task.first, task.second );
    outQueue.addTask( taskOut );
#ifdef DEBUG
    std::cout << "Result matrix\n";
    common::Matrix::write( taskOut, std::cout );
#endif // DEBUG
} // multiplyMatricesSerial


void threads::Tasks::multiplyMatricesParallel( const TaskGen& task
    , ThreadFabric< TaskOut >& outQueue, size_t threadsCount )
{
    const TaskOut& taskOut = threads::multiplyMatricesParallel( task.first, task.second, threadsCount );
    outQueue.addTask( taskOut );
#ifdef DEBUG
    std::cout << "Result matrix\n";
    common::Matrix::write( taskOut, std::cout );
#endif // DEBUG
} // multiplyMatricesParallel


void threads::Tasks::writeResultMatrix( const threads::TaskOut& task )
{
    /// Вывод результата в файл
    std::ofstream outStream( common::defs::outFile, std::ios_base::out | std::ios_base::trunc );
    if ( !outStream.is_open() )
    {
        std::cerr << "Cannot open file " << common::defs::outFile << '\n';
        return;
    }
    if ( !common::Matrix::write( task, outStream ) )
    {
        std::cerr << "Could not write result matrix\n";
        return;
    }
} // writeResultMatrix
