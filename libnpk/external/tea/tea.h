/* TEA code from http://www.ftp.cl.cam.ac.uk/ftp/papers/djw-rmn/djw-rmn-tea.html */

void tea_encode(int* v, int* k);
void tea_decode(int* v, int* k);

void tea_encode_buffer(char* in_buffer, unsigned int in_size, int* key, int cipherRemains);
void tea_decode_buffer(char* in_buffer, unsigned int in_size, int* key, int cipherRemains);
