namespace avx2 {

    union proxy {
        __m256i  vec;
        uint8_t  u8[32];
        uint16_t u16[16];
    };


    namespace dump {

        void epu16(const __m256i vec) {
            
            proxy p;
            p.vec = vec;

            for (int i=0; i < 16; i++) {
                printf("%04x ", p.u16[i]);
            }

            putchar('\n');
        }

        void epu8(const __m256i vec) {
            
            proxy p;
            p.vec = vec;

            putchar('\'');
            for (int i=0; i < 32; i++) {
                printf("%02x ", p.u8[i]);
            }

            putchar('\'');
            putchar('\n');
        }

    } // namespace dump

} // namespace sse
