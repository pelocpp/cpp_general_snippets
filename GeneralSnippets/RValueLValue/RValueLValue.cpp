// =====================================================================================
// LValue / RValue
// =====================================================================================

#include <iostream>
#include <cctype>

namespace LValueRValue {

    // lvalue reference
    void sayHello(std::string& message) {
        std::cout << "sayHello [std::string&]:  " << message << std::endl;
    }

    // rvalue reference
    void sayHello(std::string&& message) {
        std::cout << "sayHello [std::string&&]: " << message << std::endl;
    }

    void helper(std::string&& message) {

        // sayHello(message);
        sayHello(std::move(message));    // cast an lvalue to an rvalue
    }

    void test01() {

        std::string a = "Hello";
        std::string b = " World";

        sayHello(a);
        sayHello("ABC");
        sayHello(a + b);
    }

    void test02() {

        std::string a = "Hello";

        // put first 'sayHello' overload into comment: won't  compile
        // "cannot convert argument 1 from 'std::string' to 'std::string &&'" -
        // "an rvalue reference cannot be bound to an lvalue"

        // sayHello(a);

        // but:
        sayHello(std::move(a));  // casts an lvalue to an rvalue
    }

    void test03() {

        helper("Ohooo");
    }

    void test04() {

        //int& i = 2;       // invalid: reference to a constant

        //int a = 2;
        //int b = 3;

        //int& k = a + b;     // invalid: reference to a temporary

        int&& i = 2;     // works: reference to a constant

        int a = 2;
        int b = 3;

        int&& k = a + b;     // works: reference to a temporary
    }
}



int main_rvalue_lvalue() {

    using namespace LValueRValue;

    test01();
    test02();
    test03();
    test04();

    return 0;
}

// =====================================================================================
// End-of-File
// =====================================================================================
