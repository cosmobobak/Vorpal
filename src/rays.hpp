template <unsigned int n>
struct factorial {
    enum { value = n * factorial<n - 1>::value };
};

template <>
struct factorial<0> {
    enum { value = 1 };
};

template <unsigned int n>
struct mask {
    enum { value = 1ULL << n };
};

template <unsigned int n>
struct mask {
    enum { value = 1ULL << n };
};