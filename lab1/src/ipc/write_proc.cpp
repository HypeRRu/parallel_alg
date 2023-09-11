#include <fstream>
#include <iostream>

#include "common/matrix.h"
#include "ipc/defs.h"
#include "ipc/unix_socket.h"


int main()
{
    common::Matrix result{ 0, 0 };

    /// Чтение матриц из сокета от второго процесса
    ipc::UnixSocket client{ ipc::defs::calcOutProcsSocket };
    if ( !client.setupClient() )
    {
        std::cerr << "Unable to setup client for process 3\n";
        return 1;
    }

    if ( !common::Matrix::read( result, client ) )
    {
        std::cerr << "Bad data for result matrix\n";
        return 1;
    }

    /// Вывод результата в файл
    std::ofstream outStream( common::defs::outFile, std::ios_base::out | std::ios_base::trunc );
    if ( !outStream.is_open() )
    {
        std::cerr << "Cannot open file " << common::defs::outFile << '\n';
        return 1;
    }

    if ( !common::Matrix::write( result, outStream ) )
    {
        std::cerr << "Could not write result matrix\n";
        return 1;
    }

    return 0;
}