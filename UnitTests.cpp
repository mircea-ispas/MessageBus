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
    assert(connection.IsEmpty());

    {
        BaseObservable<int> observable;
        connection = observable.AddObserver([](const int&){});

        assert(!connection.IsEmpty());
    }

    assert(connection.IsEmpty());
}

int main(int argc, const char * argv[])
{
    Test_ObserverGetsNotified();
    Test_ConnectionClose();
    Test_ObservableDeleted();
}
