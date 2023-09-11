#include "ipc/unix_socket.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>


ipc::UnixSocket::UnixSocket( const std::string& sockPath ) noexcept
    : Socket( ::socket( AF_UNIX, SOCK_STREAM, 0 ) )
{
    if ( !isValid() )
    {
        return;
    }

    std::memset( &sockaddr_, 0, sizeof( sockaddr_ ) );
    sockaddr_.sun_family = AF_UNIX;
    std::strncpy( sockaddr_.sun_path, sockPath.c_str(), sizeof( sockaddr_.sun_path ) - 1 );
} // UnixSocket


bool ipc::UnixSocket::setupServer()
{
    if ( !isValid() )
    {
        return false;
    }

    if ( isBound() )
    {
        errMsg_ = "Socket already bound";
        return false;
    }

    isServer_ = true;
    ::unlink( sockaddr_.sun_path );
    if ( -1 == ::bind( fd_, reinterpret_cast< struct sockaddr* >( &sockaddr_ ), sizeof( sockaddr_ ) ) )
    {
        isValid_ = false;
        errMsg_ = "Cannot bind socket";
        return false;
    }
    if ( -1 == ::listen( fd_, maxConnections_ ) )
    {
        isValid_ = false;
        errMsg_ = "Cannot listen socket";
        return false;
    }
    isBound_ = true;

    return true;
} // setupServer


ipc::Socket ipc::UnixSocket::acceptConnection()
{
    if ( !isValid() )
    {
        return ipc::Socket{ -1 };
    }

    if ( !isBound() || !isServer() )
    {
        errMsg_ = "Socket does not listening";
        return ipc::Socket{ -1 };
    }

    int slave_fd = accept( fd_, nullptr, nullptr );
    return ipc::Socket{ slave_fd };
} // acceptConnection


bool ipc::UnixSocket::setupClient()
{
    if ( !isValid() )
    {
        return false;
    }

    if ( isBound() )
    {
        errMsg_ = "Socket already bound";
        return false;
    }
    
    isServer_ = false;
    if ( -1 == ::connect( fd_, reinterpret_cast< struct sockaddr* >( &sockaddr_ ), sizeof( sockaddr_ ) ) )
    {
        isValid_ = false;
        errMsg_ = "Cannot listen socket";
        return false;
    }
    isBound_ = true;

    return true;
} // setupClient


void ipc::UnixSocket::close()
{
    Socket::close();
    if ( isServer() )
    {
        ::unlink( sockaddr_.sun_path );
    }
} // close
