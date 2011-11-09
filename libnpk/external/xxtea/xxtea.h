/* Corrected Block TEA code from http://en.wikipedia.org/wiki/XXTEA */

void xxtea_encode(int* v, int* k);
void xxtea_decode(int* v, int* k);

void xxtea_encode_buffer(char* in_buffer, unsigned int in_size, int* key, int cipherRemains);
void xxtea_decode_buffer(char* in_buffer, unsigned int in_size, int* key, int cipherRemains);
