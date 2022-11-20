#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MY_LITTLE_ENDIAN 0xFFFE
#define MY_BIG_ENDIAN 0xFEFF
#define BIG 1
#define LITTLE 0

int main(int argc, char *argv[]) {

    FILE *in, *out;
    unsigned char utf8s, correct_input = 0;
    unsigned short utf16s;  // symbol in utf16
    int bom = MY_BIG_ENDIAN;

    if (argc >= 5) {
        in = fopen(argv[2], "r"), out = fopen(argv[4], "w");
    } else if (argc == 3) {
        in = fopen(argv[2], "r"), out = stdout;
    } else if (argc == 1) {
        in = stdin, out = stdout;
    } else {
        return EXIT_FAILURE;
    }

    if (argc > 5) {
        if (argc == 7) {
            if ((strcmp(argv[5], "-be") == 0) && (strcmp(argv[6], "-le") == 0) ||
                (strcmp(argv[5], "-le") == 0) && (strcmp(argv[6], "-be") == 0)) {
                fprintf(stderr, "Choose only one (-be or -le) decoding way\n");
                return EXIT_FAILURE;
            }
        } else if (argc == 6) {
            if (strcmp(argv[5], "-be") == 0) {
                bom = MY_BIG_ENDIAN;
            } else if (strcmp(argv[5], "-le") == 0) {
                bom = MY_LITTLE_ENDIAN;
            }
        } else {
            bom = MY_LITTLE_ENDIAN;
        }
    }
    // Check if the BOM is in file
    unsigned char byte1 = 0, byte2 = 0, byte3 = 0;
    fread(&byte1, sizeof(char), 1, in);
    fread(&byte2, sizeof(char), 1, in);
    fread(&byte3, sizeof(char), 1, in);
    if ((!((byte1 == 0xEF) && (byte2 == 0xBF) && (byte3 == 0xBE))) ||
        (!((byte1 == 0xEF) && (byte2 == 0xBB) && (byte3 == 0xBF)))) {
        fseek(in, 0, SEEK_SET);  // return to the beginning of the file
    }

    fwrite(&bom, sizeof(unsigned short), 1, out);  // bom is written
    fread(&utf8s, sizeof(unsigned char), 1, in);

    while (!feof(in)) {
        utf16s = 0;
        if ((utf8s >> 7) == 0) {  // 1 byte is a symbol
            utf16s += utf8s;
            correct_input = 1;
        } else if ((utf8s >> 5) == 0x6) {  // 2 bytes are a symbol, 0x6 = 00000110
            utf8s &= 0x1F;  // 0x1F = 00011111
            utf16s += utf8s;
            utf16s <<= 6;  // leave 6 pos for second byte
            fread(&utf8s, sizeof(unsigned char), 1, in);  // read second byte
            if ((utf8s >> 6) == 0x2) {
                utf8s &= 0x3F;  //0x3F = 00111111
                utf16s += utf8s;
                correct_input = 1;
            } else {  // incorrect second byte in a pair of bytes
                fprintf(stderr, "The second byte is incorrect in a pair of 2 bytes, "
                                "value: %hhx, position: %ld\n", utf8s, ftell(in));
            }
        } else if ((utf8s >> 4) == 0xE) {  // 3 bytes are a symbol, 0xE = 00001110
            utf8s &= 0xF;  // 0xF = 00001111
            utf16s += utf8s;
            utf16s <<= 6;  // leave 6 pos for second byte
            fread(&utf8s, sizeof(unsigned char), 1, in);  // read second byte
            if ((utf8s >> 6) == 0x2) {
                utf8s &= 0x3F;  //0x3F = 00111111
                utf16s += utf8s;
                utf16s <<= 6;  // leave pos bits for third byte
                fread(&utf8s, sizeof(unsigned char), 1, in);  // read second byte
                if ((utf8s >> 6) == 0x2) {
                    utf8s &= 0x3F;  //0x3F = 00111111
                    utf16s += utf8s;
                    correct_input = 1;
                } else {
                    fprintf(stderr, "The third byte is incorrect in a triplet of 3 bytes, "
                                    "value: %hhx, position: %ld\n", utf8s, ftell(in));
                }
            } else {
                fprintf(stderr, "The second byte is incorrect in a triplet of 3 bytes, "
                                "value: %hhx, position: %ld\n", utf8s, ftell(in));
            }
        } else {
            fprintf(stderr, "The first byte is incorrect in a triplet of 3 bytes, "
                            "value: %hhx, position: %ld\n", utf8s, ftell(in));
        }

        if (correct_input) {
            if (bom == MY_LITTLE_ENDIAN) {
                utf8s = (unsigned char) (utf16s >> 8);  // left part
                fwrite(&utf8s, sizeof(unsigned char), 1, out);
                utf8s = (unsigned char) (utf16s);  // right part
                fwrite(&utf8s, sizeof(unsigned char), 1, out);
            } else {
                fwrite(&utf16s, sizeof(unsigned short), 1, out);
            }
            correct_input = 0;
        }
        fread(&utf8s, sizeof(unsigned  char), 1, in);
    }
    fclose(in);
    fclose(out);
    return 0;
}