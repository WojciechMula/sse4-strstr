
namespace bits {

    template <typename T>
    T clear_leftmost_set(const T value) {

        assert(value != 0);

        return value & (value - 1);
    }


    template <typename T>
    unsigned get_first_bit_set(const T value) {

        assert(value != 0);

        return __builtin_ctz(value);
    }


    template <>
    unsigned get_first_bit_set<uint64_t>(const uint64_t value) {

        assert(value != 0);

        return __builtin_ctzl(value);
    }

} // namespace bits
