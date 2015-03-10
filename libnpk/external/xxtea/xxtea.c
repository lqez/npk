#include <sys/types.h>
#include <stddef.h>

#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (k[(p&3)^e] ^ z)))

#ifdef NPK_DEV
void xxtea_encode(char* c, int* k)
{
    unsigned int n = 2;
    unsigned int v[2];
    unsigned int y, z, sum;
    unsigned int p, rounds, e;

    v[0] = ((c[0]      ) & 0x000000FF)
         | ((c[1] <<  8) & 0x0000FF00)
         | ((c[2] << 16) & 0x00FF0000)
         | ((c[3] << 24) & 0xFF000000);
    v[1] = ((c[4]      ) & 0x000000FF)
         | ((c[5] <<  8) & 0x0000FF00)
         | ((c[6] << 16) & 0x00FF0000)
         | ((c[7] << 24) & 0xFF000000);

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

    c[0] = (char)((v[0] & 0x000000FF) & 0xFF);
    c[1] = (char)((v[0] & 0x0000FF00) >> 8 & 0xFF);
    c[2] = (char)((v[0] & 0x00FF0000) >> 16 & 0xFF);
    c[3] = (char)((v[0] & 0xFF000000) >> 24 & 0xFF);
    c[4] = (char)((v[1] & 0x000000FF) & 0xFF);
    c[5] = (char)((v[1] & 0x0000FF00) >> 8 & 0xFF);
    c[6] = (char)((v[1] & 0x00FF0000) >> 16 & 0xFF);
    c[7] = (char)((v[1] & 0xFF000000) >> 24 & 0xFF);
}

void xxtea_encode_byte(char* v, int* k, off_t p)
{
    char y[] = "XtEaNpK";
    *v = *v^y[p]^(char)(k[p%4]%0xFF);
}

void xxtea_encode_buffer(char* in_buffer, off_t in_size, int* key, int cipherRemains)
{
    char *p;
    off_t remain = in_size % 8;
    off_t align_size = in_size - remain;
    for (p = in_buffer; p < in_buffer + align_size; p += 8)
        xxtea_encode(p, key);
    if( remain > 0 && cipherRemains )
        for (p = in_buffer + align_size; p < in_buffer + in_size; p += 1)
            xxtea_encode_byte( p, key, --remain );
}
#endif

void xxtea_decode(char* c,int* k)
{
    unsigned int n = 2;
    unsigned int v[2];
    unsigned int y, z, sum;
    unsigned int p, rounds, e;

    v[0] = ((c[0]      ) & 0x000000FF)
         | ((c[1] <<  8) & 0x0000FF00)
         | ((c[2] << 16) & 0x00FF0000)
         | ((c[3] << 24) & 0xFF000000);
    v[1] = ((c[4]      ) & 0x000000FF)
         | ((c[5] <<  8) & 0x0000FF00)
         | ((c[6] << 16) & 0x00FF0000)
         | ((c[7] << 24) & 0xFF000000);

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

    c[0] = (char)((v[0] & 0x000000FF) & 0xFF);
    c[1] = (char)((v[0] & 0x0000FF00) >> 8 & 0xFF);
    c[2] = (char)((v[0] & 0x00FF0000) >> 16 & 0xFF);
    c[3] = (char)((v[0] & 0xFF000000) >> 24 & 0xFF);
    c[4] = (char)((v[1] & 0x000000FF) & 0xFF);
    c[5] = (char)((v[1] & 0x0000FF00) >> 8 & 0xFF);
    c[6] = (char)((v[1] & 0x00FF0000) >> 16 & 0xFF);
    c[7] = (char)((v[1] & 0xFF000000) >> 24 & 0xFF);
}

void xxtea_decode_byte(char* v, int* k, off_t p)
{
    char y[] = "XtEaNpK";
    *v = *v^(char)(k[p%4]%0xFF)^y[p];
}

void xxtea_decode_buffer(char* in_buffer, off_t in_size, int* key, int cipherRemains)
{
    char *p;
    off_t remain = in_size % 8;
    off_t align_size = in_size - remain;
    for (p = in_buffer; p < in_buffer + align_size; p += 8)
        xxtea_decode(p, key);
    if( remain > 0 && cipherRemains )
        for (p = in_buffer + align_size; p < in_buffer + in_size; p += 1)
            xxtea_decode_byte( p, key, --remain );
}
