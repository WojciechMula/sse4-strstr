namespace neon {

    namespace dump {

        void epu8(const uint8x16_t vec) {
            
            uint8_t p[16];
            vst1q_u8(p, vec);

            putchar('\'');
            for (int i=0; i < 16; i++) {
                printf("%02x ", p[i]);
            }

            putchar('\'');
            putchar('\n');
        }

        void epu8(const uint8x8_t vec) {
            
            uint8_t p[8];
            vst1_u8(p, vec);

            putchar('\'');
            for (int i=0; i < 8; i++) {
                printf("%02x ", p[i]);
            }

            putchar('\'');
            putchar('\n');
        }

    } // namespace dump

} // namespace sse
