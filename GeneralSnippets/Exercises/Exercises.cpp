// =====================================================================================
// Exercices
// =====================================================================================

#include <iostream>
#include <string>
#include <algorithm>
#include <typeinfo>
#include <type_traits>
#include <vector>
#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <optional>
#include <tuple>

namespace Exercise_01 {

    template <typename T>
    inline T minimum(const T& t) { return t; }

    template <typename T, typename ...P>
    inline typename std::common_type<T, P...>::type
    minimum(const T& t, const P& ...p)
    {
        typedef typename std::common_type<T, P...>::type res_type;
        return std::min(res_type(t), res_type(minimum(p...)));
    }

    // oder etwas knapper

    //template <typename T>
    //inline T minimum(const T& t) { return t; }

    //template <typename T, typename ...P>
    //inline auto minimum(const T& t, const P& ...p)
    //{
    //    using res_type = std::common_type_t<T, P...>;
    //    return std::min(res_type(t), res_type(minimum(p...)));
    //}

    void testExercise() {
        auto min1 = minimum(-7, 3.7f, 9u, -2.6);
        std::cout << "min1: " << min1 << ", type: " << typeid(min1).name() << std::endl;

        auto min2 = minimum(-7, 3.7f, 9u, -42.6);
        std::cout << "min2: " << min2 << ", type: " << typeid(min2).name() << std::endl;

        auto min3 = minimum(123, (short)456, (long)789);
        std::cout << "min3: " << min3 << ", type: " << typeid(min3).name() << std::endl;
    }
}

namespace Exercise_02 {

#define SOLUTION 

    class HugeArray {
    private:
        size_t m_len;
        int* m_data;

    public:
        HugeArray();        // default c'tor
        HugeArray(size_t);  // user-defined c'tor
        ~HugeArray();       // d'tor

        // copy semantics
        HugeArray(const HugeArray&);  // copy c'tor
        HugeArray& operator=(const HugeArray&);  // copy assignment

#if defined (SOLUTION)
        // move semantics
        HugeArray(HugeArray&&) noexcept;  // move c'tor
        HugeArray& operator= (HugeArray&&) noexcept; // move assignment
#endif
    };

    HugeArray::HugeArray() : m_len(0), m_data(nullptr) {
        std::cout << "default c'tor" << std::endl;
    }

    HugeArray::HugeArray(size_t len) : m_len(len), m_data(new int[len]) {
        std::cout << "c'tor (size_t):  " << len << " allocated" << std::endl;
    }

    HugeArray::~HugeArray() {
        std::cout << "d'tor:           " << m_len << " relased" << std::endl;
        delete[] m_data;
    }

    // copy semantics
    HugeArray::HugeArray(const HugeArray& other) {
        std::cout << "COPY c'tor:      " << other.m_len << " allocated" << std::endl;
        m_len = other.m_len;
        m_data = new int[other.m_len];
        std::copy(other.m_data, other.m_data + m_len, m_data);
    }

    HugeArray& HugeArray::operator=(const HugeArray& other) {
        std::cout << "COPY assignment: " << other.m_len << " assigned" << std::endl;
        if (this != &other) {
            delete[] m_data;
            m_len = other.m_len;
            m_data = new int[m_len];
            std::copy(other.m_data, other.m_data + m_len, m_data);
        }
        return *this;
    }

#if defined (SOLUTION)
    // move semantics
    HugeArray::HugeArray(HugeArray&& other) noexcept {  // move c'tor
        std::cout << "MOVE c'tor:      " << other.m_len << " allocated" << std::endl;
        m_data = other.m_data;   // shallow copy
        m_len = other.m_len;
        other.m_data = nullptr;  // reset source object, ownership has been moved
        other.m_len = 0;
    }

    HugeArray& HugeArray::operator= (HugeArray&& other) noexcept { // move-assignment
        std::cout << "MOVE assignment: " << other.m_len << " assigned" << std::endl;
        if (this != &other) {
            delete[] m_data;         // release left side
            m_data = other.m_data;   // shallow copy
            m_len = other.m_len;
            other.m_data = nullptr;  // reset source object, ownership has been moved
            other.m_len = 0;
        }
        return *this;
    }
#endif

    void testExercise() {
        std::cout << "Start:" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<HugeArray> myVec;
        HugeArray bArray(10000000);
        HugeArray bArray2(bArray);
        myVec.push_back(bArray);   // <== std::move
        bArray = HugeArray(20000000);
        myVec.push_back(HugeArray(30000000));
        auto end = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
        std::cout << "Done [" << diff.count() << " msecs]" << std::endl;
    }
}

namespace Exercise_03 {

    inline void even(const int val) {
        if (!(val % 2)) {
            std::cout << val << std::endl;
        }
    }

    struct Even {
        void operator()(const int val) {
            if (!(val % 2)) {
                std::cout << val << std::endl;
            }
        }
    };

    void testExercise01() {
        std::vector<int> values(20);

        std::generate(
            std::begin(values),
            std::end(values),
            [count = 1]() mutable { return count++; }
        );

        // function
        std::for_each(std::begin(values), std::end(values), even);

        // lambda
        std::for_each(std::begin(values), std::end(values), [](int val) {
            if (!(val % 2)) {
                std::cout << val << std::endl;
            }   
        });

        // functor
        std::for_each(std::begin(values), std::end(values), Even());
    }

    void testExercise02() {
        std::vector<int> values(20);

        std::generate(
            std::begin(values),
            std::end(values),
            [count = 1]() mutable { return count++; }
        );

        // 'divisor' defined within capture clause 
        std::for_each(std::begin(values), std::end(values), [divisor = 3](int val) {
            if (!(val % divisor)) {
                std::cout << val << std::endl;
            }
        });
        std::cout << std::endl;

        // or 'divisor' defined in outer context (scope) 
        int divisor = 5;

        // capture context by value (reference & would work also)
        std::for_each(std::begin(values), std::end(values), [=](int val) {
            if (!(val % divisor)) {
                std::cout << val << std::endl;
            }
        });
    }
}

namespace Exercise_04 {

    std::map<char, std::function<double(double, double)>> createCalculatorEx() {

        std::map<char, std::function<double(double, double)>> map;
        map.insert(std::make_pair('+', [](double a, double b) { return a + b; }));
        map.insert(std::make_pair('-', [](double a, double b) { return a - b; }));
        map.insert(std::make_pair('*', [](double a, double b) { return a * b; }));
        map.insert(std::make_pair('/', [](double a, double b) { return a / b; }));
        return map;
    }

    // or more compact

    std::map<char, std::function<double(double, double)>> createCalculator() {

        std::map<char, std::function<double(double, double)>> map;
        map['+'] = [](double a, double b) { return a + b; };
        map['-'] = [](double a, double b) { return a - b; };
        map['*'] = [](double a, double b) { return a * b; };
        map['/'] = [](double a, double b) { return a / b; };
        return map;
    }

    void testExercise01() {

        std::map<char, std::function<double(double, double)>> calculator = createCalculator();
        double op1, op2;
        char op;

        std::cout << "Enter first Operand: ";
        std::cin >> op1;
        std::cout << "Enter second Operand: ";
        std::cin >> op2;
        std::cout << "Enter operation (+, -, *, /): ";
        std::cin >> op;

        // do the math
        double result = calculator[op](op1, op2);
        std::cout << "Result: " << op1 << ' ' << op << ' ' << op1 << " = " << result << '.' << std::endl;
    };

    void testExercise02() {

        std::map<char, std::function<double(double, double)>> calculator = createCalculator();

        // do some math operations
        std::cout << "1.5 + 2.7 = " << calculator['+'](1.5, 2.7) << std::endl;
        std::cout << "1.5 - 2.7 = " << calculator['-'](1.5, 2.7) << std::endl;
        std::cout << "1.5 * 2.7 = " << calculator['*'](1.5, 2.7) << std::endl;
        std::cout << "1.5 / 2.7 = " << calculator['/'](1.5, 2.7) << std::endl;

        // add a new operation
        calculator.insert(std::make_pair('^', [](double a, double b) { return std::pow(a, b); }));
        std::cout << "1.5 ^ 2.5 = " << calculator['^'](1.5, 2.5) << std::endl;
    };
}

namespace Exercise_05 {

    std::optional<int> toInt(std::string s) {
        std::optional<int> result;
        try
        {
            int i = std::stoi(s);

            // want input string to be consumed entirely (there are several ways
            // to accomplish this each with advantages and disadvantages)
            std::string tmp = std::to_string(i);
            if (tmp.length() != s.length())
                throw std::invalid_argument("input string illegal");

            result.emplace(i);
        }
        catch (std::invalid_argument const&)
        {
            std::cerr << "Bad input: std::invalid_argument" << std::endl;
        }
        catch (std::out_of_range const&)
        {
            std::cerr << "Integer overflow: std::out_of_range" << std::endl;
        }

        return result;
    }

    template <typename T>
    std::optional<T> toNumber(std::string s) {
        std::optional<T> result;
        try
        {
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, short>) {
                int i = std::stoi(s);
                std::string tmp = std::to_string(i);
                if (tmp.length() != s.length())
                    throw std::invalid_argument("input string illegal");

                result.emplace(i);
            }
            if constexpr (std::is_same_v<T, long>) {
                long l = std::stol(s);
                std::string tmp = std::to_string(l);
                if (tmp.length() != s.length())
                    throw std::invalid_argument("input string illegal");

                result.emplace(l);
            }
            if constexpr (std::is_same_v<T, long long>) {
                long long ll = std::stoll(s);
                std::string tmp = std::to_string(ll);
                if (tmp.length() != s.length())
                    throw std::invalid_argument("input string illegal");

                result.emplace(ll);
            }
        }
        catch (std::invalid_argument const&)
        {
            std::cerr << "Bad input: std::invalid_argument" << std::endl;
        }
        catch (std::out_of_range const&)
        {
            std::cerr << "Integer overflow: std::out_of_range" << std::endl;
        }

        return result;
    }

    void testExercise01() {
        std::optional<int> i1 = toInt("123");
        if (i1.has_value()) {
            std::cerr << i1.value() << std::endl;
        }

        std::optional<int> i2 = toInt("-987654321");
        if (i2.has_value()) {
            std::cerr << i2.value() << std::endl;
        }

        std::optional<int> i3 = toInt("123ABC");
        if (i3.has_value()) {
            std::cerr << i3.value() << std::endl;
        }

        std::optional<int> i4 = toInt("ABC123");
        if (i4.has_value()) {
            std::cerr << i4.value() << std::endl;
        }
    }

    void testExercise02() {

        std::optional<short> i1 = toNumber<short>("32767");
        if (i1.has_value()) {
            std::cerr << i1.value() << std::endl;
        }

        std::optional<int> i2 = toNumber<int>("2147483647");
        if (i2.has_value()) {
            std::cerr << i2.value() << std::endl;
        }

        std::optional<long int> i3 = toNumber<long int>("2147483647");
        if (i3.has_value()) {
            std::cerr << i3.value() << std::endl;
        }

        std::optional<long long> i4 = toNumber<long long>("9223372036854775807");
        if (i4.has_value()) {
            std::cerr << i4.value() << std::endl;
        }
    }
}

namespace Exercise_06 {

    template<typename Tuple, std::size_t N>
    struct ShowTupleImpl {
        static void print(const Tuple& t) {
            ShowTupleImpl<Tuple, N - 1>::print(t);
            std::cout << ", " << std::get<N - 1>(t);
        }
    };

    template<typename Tuple>
    struct ShowTupleImpl<Tuple, 1> {
        static void print(const Tuple& t) {
            std::cout << std::get<0>(t);
        }
    };

    template<typename ... args>
    void printTuple(const std::tuple<args...>& t) {
        using tuple_type = const std::tuple<args...>&;
        static const int tuple_size = sizeof...(args);
        std::cout << "[";
        ShowTupleImpl<const std::tuple<args...>&, tuple_size>::print(t);
        std::cout << "]" << std::endl;
    }

    template<typename ... args>
    void printTupleEx(const std::tuple<args...>& t) {
        using tuple_type = const std::tuple<args...>&;
        static const int tuple_size = sizeof...(args);
        std::cout << "[";
        ShowTupleImpl<tuple_type, tuple_size>::print(t);
        std::cout << "]" << std::endl;
    }

    void testExercise() {
        auto tuple1 = std::make_tuple(1, std::string("Modern C++"), false, 3.14159);
        auto tuple2 = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto tuple3 = std::make_tuple(12345);

        printTuple(tuple1);
        printTuple(tuple2);
        printTuple(tuple3);

        printTupleEx(tuple1);
        printTupleEx(tuple2);
        printTupleEx(tuple3);
    }
}

namespace Exercise_07 {

    template<long long n>
    struct FibImpl {
        static constexpr long long value =
            FibImpl<n - 1>::value + FibImpl<n - 2>::value;
    };

    template<>
    struct FibImpl<1> {
        static constexpr long long value = 1;
    };

    template<>
    struct FibImpl<0> {
        static constexpr long long value = 0;
    };

    template<long long n>
    struct Fibonacci {
        static_assert(n >= 0, "Error: Fibonacci can't be called with a negative integer");
        static constexpr long long value = FibImpl<n>::value;
    };

    void testExercise() {
        std::cout << 1 << ":  " << Fibonacci<1>::value  << std::endl;
        std::cout << 2 << ":  " << Fibonacci<2>::value  << std::endl;
        std::cout << 10 << ": " << Fibonacci<10>::value << std::endl;
        std::cout << 20 << ": " << Fibonacci<20>::value << std::endl;
        std::cout << 30 << ": " << Fibonacci<30>::value << std::endl;
        std::cout << 40 << ": " << Fibonacci<40>::value << std::endl;
        std::cout << 50 << ": " << Fibonacci<50>::value << std::endl;
        std::cout << 60 << ": " << Fibonacci<60>::value << std::endl;
        std::cout << 70 << ": " << Fibonacci<70>::value << std::endl;
        std::cout << 80 << ": " << Fibonacci<80>::value << std::endl;
        std::cout << 90 << ": " << Fibonacci<90>::value << std::endl;
        std::cout << 92 << ": " << Fibonacci<92>::value << std::endl;
    }
}


int main()
// int main_exercices()
{
    //using namespace Exercise_01;
    //testExercise();

    //using namespace Exercise_02;
    //testExercise();

    //using namespace Exercise_03;
    //testExercise01();
    //testExercise02();

  //  using namespace Exercise_04;
    //testExercise01();
 //   testExercise02();

 //   using namespace Exercise_05;
   // testExercise01();
  //  testExercise02();

    // using namespace Exercise_06;
    //testExercise();

    using namespace Exercise_07;
    testExercise();

    return 1;
}

// =====================================================================================
// End-of-File
// =====================================================================================