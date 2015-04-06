namespace sse {

    template <typename T>
    __m128i load(T ptr) {
        
        return _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr));
    }

    __m128i mask_lower_bytes(size_t n) {

        // assert(n < 16)

        static const uint8_t mask[32] = {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        };

        return load(mask + 16 - n);
    }

    __m128i mask_higher_bytes(size_t n) {

        // assert(n < 16)

        static const uint8_t mask[32] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
        };

        return load(mask + 16 - n);
    }


    union proxy {
        __m128i  vec;
        uint8_t  u8[16];
        uint16_t u16[8];
    };


    namespace dump {

        void epu16(const __m128i vec) {
            
            proxy p;
            p.vec = vec;

            for (int i=0; i < 8; i++) {
                printf("%04x ", p.u16[i]);
            }

            putchar('\n');
        }

        void epu8(const __m128i vec) {
            
            proxy p;
            p.vec = vec;

            putchar('\'');
            for (int i=0; i < 16; i++) {
                printf("%02x ", p.u8[i]);
            }

            putchar('\'');
            putchar('\n');
        }

    } // namespace dump

} // namespace sse
