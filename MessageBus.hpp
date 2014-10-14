#pragma once

#include "Observable.hpp"

class MessageBus
{
    public:
        template<typename MessageType>
        void                                    PostMessage(const MessageType& message);
        template<typename MessageType>
        Connection                              RegisterObserver(std::function<void(const MessageType&)> callback);
        template<typename ObserverType, typename MessageType>
        Connection                              RegisterObserver(ObserverType* object, void (ObserverType::*callback)(const MessageType&));

    private:
        template<typename MessageType>
        Observable<MessageType, MessageBus>&    GetObservables();
};

template<typename MessageType>
void MessageBus::PostMessage(const MessageType& message)
{
    GetObservables<MessageType>()(message);
}

template<typename MessageType>
Connection MessageBus::RegisterObserver(std::function<void(const MessageType&)> callback)
{
    return GetObservables<MessageType>().AddObserver(callback);
}


template<typename ObserverType, typename MessageType>
Connection MessageBus::RegisterObserver(ObserverType* object, void (ObserverType::*callback)(const MessageType&))
{
    return GetObservables<MessageType>().AddObserver(object, callback);
}

template<typename MessageType>
Observable<MessageType, MessageBus>& MessageBus::GetObservables()
{
    static Observable<MessageType, MessageBus> s_observables;
    return s_observables;
}