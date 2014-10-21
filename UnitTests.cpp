#include "Observable.hpp"
#include "MessageBus.hpp"

#include <cassert>
#include <iostream>

struct IntObserver
{
    int& m_value;

    IntObserver(int& value) : m_value(value)
    {
    }

    void operator()(const int& value)
    {
        m_value = value;
        std::cout<<"IntObserver: "<<value<<std::endl;
    }
};

void Test_ObserverGetsNotified()
{
    int value = 0;
    BaseObservable<int> observable;

    Connection connection = observable.AddObserver(IntObserver(value));

    observable(1);
    assert(value == 1);
}

void Test_ConnectionClose()
{
    int value = 0;
    BaseObservable<int> observable;

    Connection connection = observable.AddObserver(IntObserver(value));

    observable(1);
    assert(value == 1);

    connection.Close();
    observable(2);
    assert(value == 1);

    connection = observable.AddObserver(IntObserver(value));
    observable(3);
    assert(value == 3);
}

void Test_ObservableDeleted()
{
    Connection connection;
    assert(connection.IsDetached());

    {
        BaseObservable<int> observable;
        connection = observable.AddObserver([](const int&){});

        assert(!connection.IsDetached());
    }

    assert(connection.IsDetached());
}

MessageBus message_buss;

struct MyEvent
{
    int myProperty;
};

class MyListener
{
    public:
        MyListener()
            : myConnection(message_buss.RegisterObserver(this, &MyListener::OnMyEvent))
        {
        }

    private:
        void OnMyEvent(const MyEvent& event)
        {
            std::cout<<"MyEvent: "<<event.myProperty<<std::endl;
        }

    private:
        Connection myConnection;
};

void Test_MessageBus()
{
    {
        MyListener listener;

        message_buss.PostMessage(MyEvent{1});
        message_buss.PostMessage(MyEvent{2});
        message_buss.PostMessage(MyEvent{3});
    }

    message_buss.PostMessage(MyEvent{4});
}

int main(int argc, const char * argv[])
{
    Test_ObserverGetsNotified();
    Test_ConnectionClose();
    Test_ObservableDeleted();
    Test_MessageBus();
}