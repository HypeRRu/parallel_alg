#ifndef IPC_UNIX_SOCKET_H
#define IPC_UNIX_SOCKET_H

#include "ipc/socket.h"

#include <string>
#include <functional>

#include <sys/un.h>

namespace ipc
{

class UnixSocket final: public Socket
{
public:
    explicit UnixSocket( const std::string& sockPath ) noexcept;

    inline bool isBound() const { return isBound_; }
    inline bool isServer() const { return isServer_; }

    bool setupServer();
    Socket acceptConnection();

    bool setupClient();

    virtual void close() override;

public:
    constexpr static size_t maxConnections_ = 1;    ///< Обрабатывается одно соединение за раз.

private:
    struct sockaddr_un sockaddr_;                   ///< Структура адреса.
    bool isBound_      = false;                     ///< Был ли запущен setupClient/setupServer.
    bool isServer_     = false;                     ///< Является ли сокет серверным.

}; // class UnixSocket

} // namespace ipc

#endif // IPC_UNIX_SOCKET_H
