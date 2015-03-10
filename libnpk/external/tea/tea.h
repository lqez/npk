/* TEA code from http://www.ftp.cl.cam.ac.uk/ftp/papers/djw-rmn/djw-rmn-tea.html */

void tea_encode(unsigned char* v, int* k);
void tea_decode(unsigned char* v, int* k);

void tea_encode_buffer(unsigned char* in_buffer, off_t in_size, int* key, int cipherRemains);
void tea_decode_buffer(unsigned char* in_buffer, off_t in_size, int* key, int cipherRemains);
