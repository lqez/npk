#include <sys/types.h>
#include <stddef.h>

#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (k[(p&3)^e] ^ z)))

#ifdef NPK_DEV
void xxtea_encode(unsigned char* c, int* k)
{
    unsigned int n = 2;
    unsigned int v[2];
    unsigned int y, z, sum;
    unsigned int p, rounds, e;

    v[0] = c[0] | c[1] << 8 | c[2] << 16 | c[3] << 24;
    v[1] = c[4] | c[5] << 8 | c[6] << 16 | c[7] << 24;

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

    c[0] = v[0]       & 0xFF;
    c[1] = v[0] >>  8 & 0xFF;
    c[2] = v[0] >> 16 & 0xFF;
    c[3] = v[0] >> 24 & 0xFF;
    c[4] = v[1]       & 0xFF;
    c[5] = v[1] >>  8 & 0xFF;
    c[6] = v[1] >> 16 & 0xFF;
    c[7] = v[1] >> 24 & 0xFF;
}

void xxtea_encode_byte(unsigned char* v, int* k, off_t p)
{
    unsigned char y[] = "XtEaNpK";
    *v = *v^y[p]^(unsigned char)(k[p%4]%0xFF);
}

void xxtea_encode_buffer(unsigned char* in_buffer, off_t in_size, int* key, int cipherRemains)
{
    unsigned char *p;
    off_t remain = in_size % 8;
    off_t align_size = in_size - remain;
    for (p = in_buffer; p < in_buffer + align_size; p += 8)
        xxtea_encode(p, key);
    if( remain > 0 && cipherRemains )
        for (p = in_buffer + align_size; p < in_buffer + in_size; p += 1)
            xxtea_encode_byte( p, key, --remain );
}
#endif

void xxtea_decode(unsigned char* c,int* k)
{
    unsigned int n = 2;
    unsigned int v[2];
    unsigned int y, z, sum;
    unsigned int p, rounds, e;

    v[0] = c[0] | c[1] << 8 | c[2] << 16 | c[3] << 24;
    v[1] = c[4] | c[5] << 8 | c[6] << 16 | c[7] << 24;

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

    c[0] = v[0]       & 0xFF;
    c[1] = v[0] >>  8 & 0xFF;
    c[2] = v[0] >> 16 & 0xFF;
    c[3] = v[0] >> 24 & 0xFF;
    c[4] = v[1]       & 0xFF;
    c[5] = v[1] >>  8 & 0xFF;
    c[6] = v[1] >> 16 & 0xFF;
    c[7] = v[1] >> 24 & 0xFF;
}

void xxtea_decode_byte(unsigned char* v, int* k, off_t p)
{
    unsigned char y[] = "XtEaNpK";
    *v = *v^(unsigned char)(k[p%4]%0xFF)^y[p];
}

void xxtea_decode_buffer(unsigned char* in_buffer, off_t in_size, int* key, int cipherRemains)
{
    unsigned char *p;
    off_t remain = in_size % 8;
    off_t align_size = in_size - remain;
    for (p = in_buffer; p < in_buffer + align_size; p += 8)
        xxtea_decode(p, key);
    if( remain > 0 && cipherRemains )
        for (p = in_buffer + align_size; p < in_buffer + in_size; p += 1)
            xxtea_decode_byte( p, key, --remain );
}
