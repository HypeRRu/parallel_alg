#include <iostream>

#include "common/matrix.h"
#include "ipc/defs.h"
#include "ipc/matrix_algorithms.h"
#include "ipc/unix_socket.h"


int main()
{
    common::Matrix lhs{ 0, 0 };
    common::Matrix rhs{ 0, 0 };

    /// Чтение матриц из сокета от первого процесса
    ipc::UnixSocket client{ ipc::defs::inCalcProcsSocket };
    if ( !client.setupClient() )
    {
        std::cerr << "Unable to setup client for process 2\n";
        return 1;
    }

    if ( !common::Matrix::read( lhs, client ) )
    {
        std::cerr << "Bad data for lhs matrix\n";
        return 1;
    }
    if ( !common::Matrix::read( rhs, client ) )
    {
        std::cerr << "Bad data for rhs matrix\n";
        return 1;
    }

    /// Умножение матриц
    common::Matrix result = ipc::multiplyMatrices( lhs, rhs );

    /// Отправка матрицы третьему процессу
    ipc::UnixSocket server{ ipc::defs::calcOutProcsSocket };
    if ( !server.setupServer() )
    {
        std::cerr << "Unable to setup server for process 2\n";
        return 1;
    }

    ipc::Socket slave = server.acceptConnection();
    if ( !slave.isValid() )
    {
        std::cerr << "Unable to connect to proccess 3\n";
        return 1;
    }

    if ( !common::Matrix::write( result, slave ) )
    {
        std::cerr << "Could not send result matrix\n";
        return 1;
    }

    return 0;
}