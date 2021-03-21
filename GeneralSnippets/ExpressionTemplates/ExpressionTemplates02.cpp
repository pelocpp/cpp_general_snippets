// =====================================================================================
// Expression Templates
// =====================================================================================

#include <iostream>
#include <string>
#include <vector>
#include <chrono> 

namespace ExpressionTemplates_02 {

    constexpr bool Verbose{ false };

    // default sizes
    constexpr size_t DefaultCols{ 3 };
    constexpr size_t DefaultRows{ 3 };

    // benchmark sizes
    constexpr int Iterations{ 10 };
    constexpr size_t BenchmarkRows { 1000 };
    constexpr size_t BenchmarkCols { 1000 };

    // actual sizes
    constexpr size_t Cols{ BenchmarkRows };
    constexpr size_t Rows{ BenchmarkRows };

    class Matrix {
    private:
        size_t m_cols;
        size_t m_rows;
        std::vector<double> m_values;

    public:
        // c'tor(s)
        Matrix() : Matrix(Cols, Rows) {}

        Matrix(size_t cols, size_t rows) : m_cols{ cols }, m_rows{ rows }
        {
            m_values.resize(cols*rows);
        }

        // getter
        size_t inline getCols() const { return m_cols; };
        size_t inline getRows() const { return m_rows; };

        // functor - representing index operator
        const double& operator()(size_t x, size_t y) const;
        double& operator()(size_t x, size_t y);

        // operator= --> classical definition
        Matrix& operator=(const Matrix& rhs);

        // operator= --> expression template approach (template member method)
        template <typename TEXPR>
        Matrix& operator=(const TEXPR& expression);
    };

    const double& Matrix::operator()(size_t x, size_t y) const {
        //if constexpr (Verbose) {
        //    std::cout << "Matrix::operator() => [" << x << ',' << y << ']' << std::endl;
        //}
        return m_values[y * getCols() + x];
    }

    double& Matrix::operator()(size_t x, size_t y) {
        //if constexpr (Verbose) {
        //    std::cout << "Matrix::operator() => [" << x << ',' << y << ']' << std::endl;
        //}
        return m_values[y * getCols() + x];
    }

    // classical operator+ definition
    Matrix operator+(const Matrix& lhs, const Matrix& rhs)
    {
        Matrix result{ lhs.getCols(), lhs.getRows() };

        for (size_t y{}; y != lhs.getRows(); ++y) {
            for (size_t x{}; x != lhs.getCols(); ++x) {

                if constexpr (Verbose) {
                    double l {lhs(x, y)};
                    double r {rhs(x, y)};
                    std::cout << "Matrix:: adding " << l << '+' << r << std::endl;
                    double tmp{ l + r };
                    std::cout << "Matrix:: assigning result " << tmp << std::endl;
                    result(x, y) = tmp;
                }
                else {
                    result(x, y) = lhs(x, y) + rhs(x, y);
                }
            }
        }
        return result;
    }

    // classical operator= implementation
    Matrix& Matrix::operator=(const Matrix& rhs) {

        // prevent self-assignment
        if (this != &rhs) {
            m_values = rhs.m_values;
        }

        return *this;
    }

    // expression template approach: operator=
    template <typename TEXPR>
    Matrix& Matrix::operator=(const TEXPR& expr) {
        for (size_t y{}; y != getRows(); ++y) {
            for (size_t x{}; x != getCols(); ++x) {

                if constexpr (Verbose) {
                    double sum{ expr(x, y) };
                    std::cout << "Matrix::    assigning expression result " << sum << std::endl;
                    m_values[y * getCols() + x] = sum;
                }
                else {
                    m_values[y * getCols() + x] = expr(x, y);
                }
            }
        }
        return *this;
    }

    // ========================================================================

    Matrix add3(const Matrix& a, const Matrix& b, const Matrix& c)
    {
        Matrix result{ a.getCols(), a.getRows() };
        for (size_t y = 0; y != a.getRows(); ++y) {
            for (size_t x = 0; x != a.getCols(); ++x) {
                result(x, y) = a(x, y) + b(x, y) + c(x, y);
            }
        }
        return result;
    }

    // ========================================================================

    template <typename LHS, typename RHS>
    class MatrixExpr
    {
    private:
        const LHS& m_lhs;
        const RHS& m_rhs;

    public:
        MatrixExpr(const LHS& lhs, const RHS& rhs) : m_rhs{ rhs }, m_lhs{ lhs } {}

        double operator() (size_t x, size_t y) const {

            if constexpr (Verbose) {
                double l { m_lhs(x, y) };
                double r { m_rhs(x, y) };
                std::cout << "MatrixExpr:: adding " << l << '+' << r << std::endl;
                double tmp{ l + r };
                return tmp;
            }
            else {
                return m_lhs(x, y) + m_rhs(x, y);
            }
        }
    };

    template <typename LHS, typename RHS>
    MatrixExpr<LHS, RHS> operator+(const LHS& lhs, const LHS& rhs) {
        return MatrixExpr<LHS, RHS>(lhs, rhs);
    }

    // ========================================================================

    void test_01()
    {
        std::cout << "Expression Template 01: Classical Approach" << std::endl;

        Matrix a{}, b{}, c{}, d{};

        // initialize matrices
        for (size_t y = 0; y != a.getRows(); ++y) {
            for (size_t x = 0; x != a.getCols(); ++x) {
                a(x, y) = 1.0;
                b(x, y) = 2.0;
                c(x, y) = 3.0;
                d(x, y) = 4.0;
            }
        }

        Matrix result = a + b + c + d;  // result(x, y) = 10 
    }

    void test_02()
    {
        std::cout << "Expression Template 02:" << std::endl;

        Matrix a{}, b{}, c{}, d{};
        Matrix result{};

        // initialize matrices
        for (size_t y = 0; y != a.getRows(); ++y) {
            for (size_t x = 0; x != a.getCols(); ++x) {
                a(x, y) = 1.0;
                b(x, y) = 2.0;
                c(x, y) = 3.0;
                d(x, y) = 4.0;
            }
        }

        // adding 2 matrices
        MatrixExpr<Matrix, Matrix> sumAB(a, b);
        for (size_t y = 0; y != a.getRows(); ++y) {
            for (size_t x = 0; x != a.getCols(); ++x) {
                result(x, y) = sumAB(x, y);
            }
        }

        // adding 3 matrices
        MatrixExpr<MatrixExpr<Matrix, Matrix>, Matrix> sumABC(sumAB, c);
        for (size_t y = 0; y != a.getRows(); ++y) {
            for (size_t x = 0; x != a.getCols(); ++x) {
                result(x, y) = sumABC(x, y);
            }
        }

        // adding 4 matrices
        MatrixExpr<MatrixExpr<MatrixExpr<Matrix, Matrix>, Matrix>, Matrix> sumABCD{ sumABC, d };
        for (size_t y = 0; y != a.getRows(); ++y) {
            for (size_t x = 0; x != a.getCols(); ++x) {
                result(x, y) = sumABCD(x, y);
            }
        }
    }

    void test_03()
    {
        std::cout << "Expression Template 03: Expression Templates Approach" << std::endl;

        Matrix a{}, b{}, c{}, d{};
        Matrix result{};

        // initialize matrices
        for (size_t y = 0; y != a.getRows(); ++y) {
            for (size_t x = 0; x != a.getCols(); ++x) {
                a(x, y) = 1.0;
                b(x, y) = 2.0;
                c(x, y) = 3.0;
                d(x, y) = 4.0;
            }
        }

        // adding 4 matrices using modified operator=
        MatrixExpr<Matrix, Matrix> sumAB{ a, b };
        MatrixExpr<MatrixExpr<Matrix, Matrix>, Matrix> sumABC{ sumAB, c };
        MatrixExpr<MatrixExpr<MatrixExpr<Matrix, Matrix>, Matrix>, Matrix> sumABCD{ sumABC, d };

        result = sumABCD;
    }

    // =====================================================================================

    void test_04a_benchmark(
        int iterations,
        Matrix& result,
        const Matrix& a1,
        const Matrix& a2,
        const Matrix& a3,
        const Matrix& a4,
        const Matrix& a5)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            result = a1 + a2 + a3 + a4 + a5;
        }
        auto end = std::chrono::high_resolution_clock::now();

        std::cout
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << " milliseconds." << std::endl;
    }

    void test_04b_benchmark(
        int iterations,
        Matrix& result,
        const Matrix& a1,
        const Matrix& a2,
        const Matrix& a3,
        const Matrix& a4,
        const Matrix& a5)
    {
        // adding 5 matrices with expression template approach
        MatrixExpr<Matrix, Matrix> sumAB{ a1, a2 };
        MatrixExpr<MatrixExpr<Matrix, Matrix>, Matrix> sumABC{ sumAB, a3 };
        MatrixExpr<MatrixExpr<MatrixExpr<Matrix, Matrix>, Matrix>, Matrix> sumABCD{ sumABC, a4 };
        MatrixExpr<MatrixExpr<MatrixExpr<MatrixExpr<Matrix, Matrix>, Matrix>, Matrix>, Matrix> sumABCDE{ sumABCD, a5 };

        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            result = sumABCDE;
        }
        auto end = std::chrono::high_resolution_clock::now();

        std::cout
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            << " milliseconds." << std::endl;
    }

    void test_04_benchmark()
    {
        std::cout << "Expression Templates 04 (Benchmark):" << std::endl;

        Matrix a{}, b{}, c{}, d{}, e{};
        Matrix result{};

        // initialize matrices
        for (size_t y = 0; y != a.getRows(); ++y) {
            for (size_t x = 0; x != a.getCols(); ++x) {
                a(x, y) = 1.0;
                b(x, y) = 2.0;
                c(x, y) = 3.0;
                d(x, y) = 4.0;
                e(x, y) = 5.0;
            }
        }

        std::cout << "Start:" << std::endl;
        test_04a_benchmark(Iterations, result, a, b, c, d, e);
        test_04b_benchmark(Iterations, result, a, b, c, d, e);
        std::cout << "Done." << std::endl;
    }
}

void main_expression_templates_02()
{
    using namespace ExpressionTemplates_02;
    test_01();            // <== classical approach
    test_02();            // <== expression templates approach
    test_03();            // <== expression templates approach using modified operator=
    test_04_benchmark();  // <== benchmark
}

// =====================================================================================
// End-of-File
// =====================================================================================
