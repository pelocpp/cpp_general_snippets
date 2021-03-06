// =====================================================================================
// Lambda and Closure Classes
// =====================================================================================

#include <iostream>

namespace LambdaAndClosure {

    void test_01() 
    {
        const int n{ 10 };
        auto lambda = [n](int a) {return n + a; };
        const auto m = lambda(20); // m is now 30
        std::cout << "m: " << m << std::endl;
    }

    void test_02() 
    {
        class ClosureClass
        {
        public:
            ClosureClass(int n) : m_n(n) {}

            // if lambda is 'mutable' remove const-ness
            int operator()(int a) const
            {
                return m_n + a;
            }

        private:
            int m_n;
        };

        const int n{ 30 };
        auto lambda = ClosureClass(n);
        const auto m = lambda(20); // m is now 50
        std::cout << "m: " << m << std::endl;
    }
}

void main_lambda_and_closure()
{
    using namespace LambdaAndClosure;
    test_01();
    test_02();
}

// =====================================================================================
// End-of-File
// =====================================================================================
