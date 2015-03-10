/* Corrected Block TEA code from http://en.wikipedia.org/wiki/XXTEA */

void xxtea_encode(unsigned char* v, int* k);
void xxtea_decode(unsigned char* v, int* k);

void xxtea_encode_buffer(unsigned char* in_buffer, off_t in_size, int* key, int cipherRemains);
void xxtea_decode_buffer(unsigned char* in_buffer, off_t in_size, int* key, int cipherRemains);
