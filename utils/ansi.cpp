namespace ansi {
    
    const int RED   = 31;
    const int GREEN = 32;
    const int WHITE = 37;

    std::string seq(const std::string& str, int color) {

        return "\033[" + std::to_string(color) + "m" + str + "\033[0m";
    }

} // namespace ansi

