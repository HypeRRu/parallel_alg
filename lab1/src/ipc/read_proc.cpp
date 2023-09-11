#include <fstream>
#include <iostream>

#include "common/matrix.h"
#include "ipc/defs.h"
#include "ipc/unix_socket.h"


int main()
{
    common::Matrix lhs{ 0, 0 };
    common::Matrix rhs{ 0, 0 };

    /// Чтение матриц из файла
    std::ifstream inStream( common::defs::inFile, std::ios_base::in );
    if ( !inStream.is_open() )
    {
        std::cerr << "Cannot open file " << common::defs::inFile << '\n';
        return 1;
    }

    if ( !common::Matrix::read( lhs, inStream ) )
    {
        std::cerr << "Bad data for lhs matrix\n";
        return 1;
    }
    if ( !common::Matrix::read( rhs, inStream ) )
    {
        std::cerr << "Bad data for rhs matrix\n";
        return 1;
    }

    /// Отправка матриц второму процессу
    ipc::UnixSocket server{ ipc::defs::inCalcProcsSocket };
    if ( !server.setupServer() )
    {
        std::cerr << "Unable to setup server for process 1\n";
        return 1;
    }

    ipc::Socket slave = server.acceptConnection();
    if ( !slave.isValid() )
    {
        std::cerr << "Unable to connect to process 2\n";
        return 1;
    }

    if ( !common::Matrix::write( lhs, slave ) )
    {
        std::cerr << "Could not send lhs matrix\n";
        return 1;
    }
    if ( !common::Matrix::write( rhs, slave ) )
    {
        std::cerr << "Could not send rhs matrix\n";
        return 1;
    }

    return 0;
}