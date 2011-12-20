#include <stddef.h>

#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (k[(p&3)^e] ^ z)))

#ifdef NPK_DEV
void xxtea_encode(int* v, int* k)
{
    unsigned int n = 2;
    unsigned int y, z, sum;
    unsigned int p, rounds, e;
    rounds = 6 + 52/n;
    sum = 0;
    z = v[n-1];
    do {
        sum += DELTA;
        e = (sum >> 2) & 3;
        for (p=0; p<n-1; p++) {
            y = v[p+1]; 
            z = v[p] += MX;
        }
        y = v[0];
        z = v[n-1] += MX;
    } while (--rounds);
}

void xxtea_encode_byte(char* v, int* k, int p)
{
    char y[] = "XtEaNpK";
    *v = *v^y[p]^(char)(k[p%4]%0xFF);
}

void xxtea_encode_buffer(char* in_buffer, unsigned int in_size, int* key, int cipherRemains)
{
    char *p;
    unsigned int remain = in_size % 8;
    unsigned int align_size = in_size - remain;
    for (p = in_buffer; p < in_buffer + align_size; p += 8)
        xxtea_encode( (int*)p, key);
    if( remain > 0 && cipherRemains )
        for (p = in_buffer + align_size; p < in_buffer + in_size; p += 1)
            xxtea_encode_byte( p, key, --remain );
}
#endif

void xxtea_decode(int* v,int* k)
{
    unsigned int n = 2;
    unsigned int y, z, sum;
    unsigned int p, rounds, e;
    rounds = 6 + 52/n;
    sum = rounds*DELTA;
    y = v[0];
    while (sum != 0) {
        e = (sum >> 2) & 3;
        for (p=n-1; p>0; p--) {
            z = v[p-1];
            y = v[p] -= MX;
        }
        z = v[n-1];
        y = v[0] -= MX;
        sum -= DELTA;
    }
}

void xxtea_decode_byte(char* v, int* k, int p)
{
    char y[] = "XtEaNpK";
    *v = *v^(char)(k[p%4]%0xFF)^y[p];
}

void xxtea_decode_buffer(char* in_buffer, unsigned int in_size, int* key, int cipherRemains)
{
    char *p;
    unsigned int remain = in_size % 8;
    unsigned int align_size = in_size - remain;
    for (p = in_buffer; p < in_buffer + align_size; p += 8)
        xxtea_decode( (int*)p, key);
    if( remain > 0 && cipherRemains )
        for (p = in_buffer + align_size; p < in_buffer + in_size; p += 1)
            xxtea_decode_byte( p, key, --remain );
}
