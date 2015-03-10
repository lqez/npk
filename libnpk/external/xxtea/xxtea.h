/* Corrected Block TEA code from http://en.wikipedia.org/wiki/XXTEA */

void xxtea_encode(char* v, int* k);
void xxtea_decode(char* v, int* k);

void xxtea_encode_buffer(char* in_buffer, off_t in_size, int* key, int cipherRemains);
void xxtea_decode_buffer(char* in_buffer, off_t in_size, int* key, int cipherRemains);
