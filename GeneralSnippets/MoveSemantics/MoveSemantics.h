namespace MoveSemantics {

    class BigData
    {
    private:
        // private member data
        size_t m_size;  // current number of elements
        int* m_data;    // array of elements

    public:
        // c'tors and d'tor
        BigData();
        BigData(size_t, int);
        ~BigData();

        // copy semantics
        BigData(const BigData&); // copy c'tor
        BigData& operator= (const BigData&);  // copy assignment

        // move semantics
        BigData(BigData&&) noexcept;  // move c'tor
        BigData& operator= (BigData&&) noexcept; // move assignment

    private:
        // private helper methods
        void cleanup() noexcept;
        void moveFrom(BigData&) noexcept;
        void swap(BigData&, BigData&) noexcept;  // 'swap idiom'

    public:
        // getter
        size_t size() const;
        bool isEmpty() const;

        // output operator
        friend std::ostream& operator<< (std::ostream&, const BigData&);
    };
}