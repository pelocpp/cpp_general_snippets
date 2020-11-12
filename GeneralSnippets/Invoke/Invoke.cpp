// =====================================================================================
// std::invoke
// =====================================================================================

#include <iostream>

namespace StdInvoke {

    // helper functions / classes
    int add(int a, int b)
    {
        return a + b;
    }

    class Incrementer
    {
    public:
        int m_x;

    public:
        Incrementer() : m_x{} {}
        Incrementer(int x) : m_x{ x } {}

        void incrementBy(int n) { m_x += n; }
        void operator()() { std::cout << "m_x: " << m_x << std::endl; }
    };

    class TimesThree
    {
    public:
        int operator()(int n) { return 3 * n; }
    };

    // testing scenarions for std::invoke
    void test_01() {
        int result;

        // free function:
        result = std::invoke(add, 1, 2);
        std::cout << "result: " << result << std::endl;
        // => 3

        // free function through pointer to function:
        result = std::invoke(&add, 3, 4);
        std::cout << "result: " << result << std::endl;
        // => 7

        int(*fadd) (int const, int const) = &add;
        result = std::invoke(fadd, 5, 6);
        std::cout << "result: " << result << std::endl;
        // => 11

        // member function through pointer to member function:
        Incrementer inc;
        std::invoke(&Incrementer::incrementBy, inc, 5);
        inc();
        // => 5

        // invoke (access) a (public) data member (!):
        result = std::invoke(&Incrementer::m_x, inc);
        std::cout << "result: " << result << std::endl;

        // function objects:
        Incrementer inc2(10);
        result = std::invoke(std::plus<>(), std::invoke(&Incrementer::m_x, inc2), 3);
        std::cout << "result: " << result << std::endl;
        // => 13

        result = std::invoke(TimesThree(), 5);
        std::cout << "result: " << result << std::endl;
        // => 15

        // lambda expression:
        auto lambda = [](auto a, auto b) {return a + b; };
        auto a = std::invoke(lambda, 11, 12);
        std::cout << "a: " << a << std::endl;
        // => 23
    }
}

void main_invoke()
{
    using namespace StdInvoke;
    test_01();
}

// =====================================================================================
// End-of-File
// =====================================================================================