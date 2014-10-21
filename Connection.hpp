#pragma once

#include <functional>

class Connection
{
    public:
        Connection() = default;
        Connection(Connection&& rhs);
        Connection(const Connection&) = delete;
        ~Connection();

        Connection&                         operator=(Connection&& rhs);
        Connection&                         operator=(const Connection&) = delete;

        void                                Close();
        bool                                IsDetached();

    private:
        Connection(std::function<void()> close, std::function<void(Connection*)> update);

    private:
        std::function<void()>               m_closeConnection;
        std::function<void(Connection*)>    m_updateConnection;

    template<typename EventType>
    friend class BaseObservable;
};
