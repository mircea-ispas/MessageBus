#pragma once

#include "Connection.hpp"

#include <list>
#include <functional>

template<typename EventType>
class BaseObservable
{
    public:
        BaseObservable() = default;
        BaseObservable(BaseObservable&&);
        BaseObservable(const BaseObservable&) = delete;
        ~BaseObservable();

        BaseObservable&         operator=(BaseObservable&&);
        BaseObservable&         operator=(const BaseObservable&) = delete;

        Connection              AddObserver(std::function<void(const EventType&)> callback);
        template<typename ObserverType>
        Connection              AddObserver(ObserverType* object, void (ObserverType::*callback)(const EventType&));

        void                    operator()(const EventType& event);

    private:
        using Link = std::pair<std::function<void(const EventType&)>, Connection*>;

    private:
        std::list<Link>         m_links;
};

template<typename EventType, typename Notifier>
class Observable : private BaseObservable<EventType>
{
    public:
        using BaseObservable<EventType>::AddObserver;

    private:
        using BaseObservable<EventType>::operator();

    friend Notifier;
};

template<typename EventType>
BaseObservable<EventType>::BaseObservable(BaseObservable&& rhs)
{
    std::swap(m_links, rhs.m_links);
}

template<typename EventType>
BaseObservable<EventType>& BaseObservable<EventType>::operator=(BaseObservable&& rhs)
{
    std::swap(m_links, rhs.m_links);

    return *this;
}

template<typename EventType>
Connection BaseObservable<EventType>::AddObserver(std::function<void(const EventType&)> callback)
{
    m_links.emplace_back(callback, nullptr);
    auto iterator = std::prev(m_links.end());

    return Connection
    {
        [this, iterator]()
        {
            m_links.erase(iterator);
        },
        [this, iterator](Connection* connection)
        {
            iterator->second = connection;
        }
    };
}

template<typename EventType>
template<typename ObserverType>
Connection BaseObservable<EventType>::AddObserver(ObserverType* object, void (ObserverType::*callback)(const EventType&))
{
    return AddObserver([object, callback](const EventType& event)
    {
        (object->*callback)(event);
    });
}

template<typename EventType>
void BaseObservable<EventType>::operator()(const EventType& event)
{
    for(auto& link : m_links)
    {
        link.first(event);
    }
}

template<typename EventType>
BaseObservable<EventType>::~BaseObservable()
{
    for(auto& link : m_links)
    {
        link.second->m_closeConnection = nullptr;
        link.second->m_updateConnection = nullptr;
    }
}