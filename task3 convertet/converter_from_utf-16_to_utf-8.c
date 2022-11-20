#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MY_LITTLE_ENDIAN 0xFFFE
#define MY_BIG_ENDIAN 0xFEFF
#define BIG 1
#define LITTLE 0

int main(int argc, char *argv[]) {

    FILE *in, *out;
    char utf8s1, utf8s2, utf8s3;  // three bytes in utf8 for three symbols
    int number_of_bytes;  // number of written bytes in utf16
    unsigned short utf16s;  // symbol in utf16
    int bom;

    if (argc >= 5) {
        in = fopen(argv[2], "r"), out = fopen(argv[4], "w");
    } else if (argc == 3) {
        in = fopen(argv[2], "r"), out = stdout;
    } else if (argc == 1) {
        in = stdin, out = stdout;
    } else {
        return EXIT_FAILURE;
    }

    number_of_bytes = fread(&utf16s, sizeof(char), 2, in);
    if (number_of_bytes == 2) {
        if (utf16s == MY_BIG_ENDIAN) {
            bom = LITTLE;
        } else if (utf16s == MY_LITTLE_ENDIAN) {
            bom = BIG;
        } else if (argc > 5) {
            if (argc == 6) {
                if (strcmp(argv[5], "-le") == 0) {
                    bom = LITTLE;
                    fseek(in, 0, SEEK_SET);
                } else if (strcmp(argv[5], "-be") == 0) {
                    bom = BIG;
                    fseek(in, 0, SEEK_SET);
                }
            } else {
                if (((strcmp(argv[5], "-le") == 0) && (strcmp(argv[6], "-be") == 0)) ||
                    ((strcmp(argv[5], "-be") == 0) && (strcmp(argv[6], "-le") == 0))) {
                    fprintf(stderr, "Choose only one (-be or -le) decoding way\n");
                    return EXIT_FAILURE;
                }
            }
        } else {
            bom = LITTLE;
            fprintf(stderr, "Bom is not announced, -le is set by default\n");
            fseek(in, 0, SEEK_SET);
        }
    } else {
        bom = LITTLE;
        fprintf(stderr, "Number of bytes is less than 2\n");
        fseek(in, 0, SEEK_SET);
    }

    number_of_bytes = fread(&utf16s, sizeof(char), 2, in);
    while (number_of_bytes == 2) {
        if (bom == BIG) {  // reorder bytes
            utf8s1 = (char)(utf16s >> 8); // first byte
            utf16s <<= 8;
            utf16s += utf8s1;
        }
        if (utf16s < 128) {
            utf8s1 = (char) utf16s;
            fwrite(&utf8s1, 1, 1, out);
        } else if (utf16s < 2048) {
            utf8s2 = (char)(utf16s & 0x3F);  // Ox3F = 63_10 = 00...0111111
            utf8s2 += 128;
            utf16s >>= 6;
            utf8s1 = (char)(utf16s);
            utf8s1 += 128 + 64;
            fwrite(&utf8s1, 1, 1, out);
            fwrite(&utf8s2, 1, 1, out);
        } else {
            utf8s3 = (char)(utf16s & 0x3F);  // Ox3F = 63_10 = 00...0111111
            utf8s3 += 128;
            utf16s >>= 6;
            utf8s2 = (char)(utf16s & 0x3F);  // Ox3F = 63_10 = 00...0111111
            utf8s2 += 128;
            utf16s >>= 6;
            utf8s1 = (char)(utf16s);
            utf8s1 += 128 + 64 + 32;
            fwrite(&utf8s1, 1, 1, out);
            fwrite(&utf8s2, 1, 1, out);
            fwrite(&utf8s3, 1, 1, out);
        }
        number_of_bytes = fread(&utf16s, sizeof(char), 2, in);
    }
    if (number_of_bytes == 1) {
        fprintf(stderr, "Incorrect number of bytes, pairs can't be matched\n");
    }
    fclose(in);
    fclose(out);
    return 0;
}