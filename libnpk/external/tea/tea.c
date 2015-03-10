#include <sys/types.h>
#include <stddef.h>

#ifdef NPK_DEV
void tea_encode(char* c, int* k)
{
    unsigned int y, z, sum=0, delta=0x9e3779b9, n=32;
    y = ((c[0]      ) & 0x000000FF)
      | ((c[1] <<  8) & 0x0000FF00)
      | ((c[2] << 16) & 0x00FF0000)
      | ((c[3] << 24) & 0xFF000000);
    z = ((c[4]      ) & 0x000000FF)
      | ((c[5] <<  8) & 0x0000FF00)
      | ((c[6] << 16) & 0x00FF0000)
      | ((c[7] << 24) & 0xFF000000);

    while (n-->0) {
        sum += delta;
        y += (z<<4)+k[0] ^ z+sum ^ (z>>5)+k[1];
        z += (y<<4)+k[2] ^ y+sum ^ (y>>5)+k[3];
    }

    c[0] = (char)((y & 0x000000FF) & 0xFF);
    c[1] = (char)((y & 0x0000FF00) >> 8 & 0xFF);
    c[2] = (char)((y & 0x00FF0000) >> 16 & 0xFF);
    c[3] = (char)((y & 0xFF000000) >> 24 & 0xFF);
    c[4] = (char)((z & 0x000000FF) & 0xFF);
    c[5] = (char)((z & 0x0000FF00) >> 8 & 0xFF);
    c[6] = (char)((z & 0x00FF0000) >> 16 & 0xFF);
    c[7] = (char)((z & 0xFF000000) >> 24 & 0xFF);
}

void tea_encode_byte(char* v, int* k, off_t p)
{
    char y[] = "NpK!TeA";
    *v = *v^y[p]^(char)(k[p%4]%0xFF);
}

void tea_encode_buffer(char* in_buffer, off_t in_size, int* key, int cipherRemains)
{
    char *p;
    off_t remain = in_size % 8;
    off_t align_size = in_size - remain;
    for (p = in_buffer; p < in_buffer + align_size; p += 8)
        tea_encode(p, key);
    if( remain > 0 && cipherRemains )
        for (p = in_buffer + align_size; p < in_buffer + in_size; p += 1)
            tea_encode_byte( p, key, --remain );
}
#endif

void tea_decode(char* c,int* k)
{
    unsigned int n=32, sum, y, z, delta=0x9e3779b9;
    y = ((c[0]      ) & 0x000000FF)
      | ((c[1] <<  8) & 0x0000FF00)
      | ((c[2] << 16) & 0x00FF0000)
      | ((c[3] << 24) & 0xFF000000);
    z = ((c[4]      ) & 0x000000FF)
      | ((c[5] <<  8) & 0x0000FF00)
      | ((c[6] << 16) & 0x00FF0000)
      | ((c[7] << 24) & 0xFF000000);

    sum=delta<<5 ;
    /* start cycle */
    while (n-->0) {
        z-= (y<<4)+k[2] ^ y+sum ^ (y>>5)+k[3];
        y-= (z<<4)+k[0] ^ z+sum ^ (z>>5)+k[1];
        sum-=delta ;  }
    /* end cycle */

    c[0] = (char)((y & 0x000000FF) & 0xFF);
    c[1] = (char)((y & 0x0000FF00) >> 8 & 0xFF);
    c[2] = (char)((y & 0x00FF0000) >> 16 & 0xFF);
    c[3] = (char)((y & 0xFF000000) >> 24 & 0xFF);
    c[4] = (char)((z & 0x000000FF) & 0xFF);
    c[5] = (char)((z & 0x0000FF00) >> 8 & 0xFF);
    c[6] = (char)((z & 0x00FF0000) >> 16 & 0xFF);
    c[7] = (char)((z & 0xFF000000) >> 24 & 0xFF);
}

void tea_decode_byte(char* v, int* k, off_t p)
{
    char y[] = "NpK!TeA";
    *v = *v^(char)(k[p%4]%0xFF)^y[p];
}

void tea_decode_buffer(char* in_buffer, off_t in_size, int* key, int cipherRemains)
{
    char *p;
    off_t remain = in_size % 8;
    off_t align_size = in_size - remain;
    for (p = in_buffer; p < in_buffer + align_size; p += 8)
        tea_decode(p, key);
    if( remain > 0 && cipherRemains )
        for (p = in_buffer + align_size; p < in_buffer + in_size; p += 1)
            tea_decode_byte( p, key, --remain );
}
