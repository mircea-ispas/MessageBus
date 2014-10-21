#include "Connection.hpp"

Connection::Connection(Connection&& rhs)
    : m_closeConnection(rhs.m_closeConnection)
    , m_updateConnection(rhs.m_updateConnection)
{
    rhs.m_closeConnection = nullptr;
    rhs.m_updateConnection = nullptr;

    if(m_updateConnection)
    {
        m_updateConnection(this);
    }
}

Connection::Connection(std::function<void()> close, std::function<void(Connection*)> update)
    : m_closeConnection(close)
    , m_updateConnection(update)
{
    if(m_updateConnection)
    {
        m_updateConnection(this);
    }
}

Connection::~Connection()
{
    Close();
}

Connection& Connection::operator=(Connection&& rhs)
{
    Close();

    m_closeConnection = rhs.m_closeConnection;
    m_updateConnection = rhs.m_updateConnection;

    if(m_updateConnection)
    {
        m_updateConnection(this);
    }

    rhs.m_closeConnection = nullptr;
    rhs.m_updateConnection = nullptr;

    return *this;
}

void Connection::Close()
{
    if(m_closeConnection)
    {
        m_closeConnection();
        m_closeConnection = nullptr;
        m_updateConnection = nullptr;
    }
}

bool Connection::IsDetached()
{
    return m_closeConnection == nullptr;
}