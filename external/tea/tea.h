/* TEA code from http://www.ftp.cl.cam.ac.uk/ftp/papers/djw-rmn/djw-rmn-tea.html */

void tea_encode(long* v, long* k);
void tea_decode(long* v, long* k);

void tea_encode_buffer(char* in_buffer, size_t in_size, long* key);
void tea_decode_buffer(char* in_buffer, size_t in_size, long* key);
