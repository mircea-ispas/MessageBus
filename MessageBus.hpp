#pragma once

#include "Observable.hpp"

#include <memory>
#include <functional>
#include <unordered_map>

template<typename T>
struct TypeID
{
    static size_t value()
    {
        return reinterpret_cast<size_t>(&TypeID<T>::value);
    }
};

class MessageBus
{
    public:
        template<typename MessageType>
        void                                        PostMessage(const MessageType& message);
        template<typename MessageType>
        Connection                                  RegisterObserver(std::function<void(const MessageType&)> callback);
        template<typename ObserverType, typename MessageType>
        Connection                                  RegisterObserver(ObserverType* object, void (ObserverType::*callback)(const MessageType&));

    private:
        using ObservablePtr = std::unique_ptr<void, void(*)(void*)>;

    private:
        template<typename MessageType>
        Observable<MessageType, MessageBus>*        GetObservable();

    private:
        std::unordered_map<size_t, ObservablePtr>   m_observables;
};

template<typename MessageType>
void MessageBus::PostMessage(const MessageType& message)
{
    GetObservable<MessageType>()->operator()(message);
}

template<typename MessageType>
Connection MessageBus::RegisterObserver(std::function<void(const MessageType&)> callback)
{
    return GetObservable<MessageType>()->AddObserver(callback);
}


template<typename ObserverType, typename MessageType>
Connection MessageBus::RegisterObserver(ObserverType* object, void (ObserverType::*callback)(const MessageType&))
{
    return GetObservable<MessageType>()->AddObserver(object, callback);
}

template<typename MessageType>
Observable<MessageType, MessageBus>* MessageBus::GetObservable()
{
    using ObservableType = Observable<MessageType, MessageBus>;
    auto it = m_observables.find(TypeID<MessageType>::value());

    if(it == m_observables.end())
    {
        m_observables.emplace(TypeID<MessageType>::value(), ObservablePtr(new ObservableType(), [](void* observable){delete static_cast<ObservableType*>(observable);}));
        it = m_observables.find(TypeID<MessageType>::value());
    }

    return static_cast<ObservableType*>(it->second.get());
}