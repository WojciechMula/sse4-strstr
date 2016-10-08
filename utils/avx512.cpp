namespace avx512 {

    union proxy {
        __m512i  vec;
        uint8_t  u8[64];
        uint16_t u16[32];
    };


    namespace dump {

        void epu16(const __m512i vec) {
            
            proxy p;
            p.vec = vec;

            for (int i=0; i < 32; i++) {
                printf("%04x ", p.u16[i]);
            }

            putchar('\n');
        }

        void epu8(const __m512i vec) {
            
            proxy p;
            p.vec = vec;

            putchar('\'');
            for (int i=0; i < 64; i++) {
                printf("%02x ", p.u8[i]);
            }

            putchar('\'');
            putchar('\n');
        }

    } // namespace dump

} // namespace sse
