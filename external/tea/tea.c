#include <stddef.h>

#ifdef NPK_DEV
void tea_encode(long* v, long* k)
{
	unsigned long y=v[0],z=v[1], sum=0,   /* set up */
		delta=0x9e3779b9, n=32 ;             /* a key schedule constant */
	while (n-->0) {                       /* basic cycle start */
		sum += delta ;
		y += (z<<4)+k[0] ^ z+sum ^ (z>>5)+k[1] ;
		z += (y<<4)+k[2] ^ y+sum ^ (y>>5)+k[3] ;   /* end cycle */
	}
	v[0]=y ; v[1]=z ;
}

void tea_encode_buffer(char* in_buffer, size_t in_size, long* key)
{
	char *p;
	size_t remain = in_size % 8;
	size_t align_size = in_size - remain;
	for (p = in_buffer; p < in_buffer + align_size; p += 8)
		tea_encode( (long*)p, key);
}
#endif

void tea_decode(long* v,long* k)
{
	unsigned long n=32, sum, y=v[0], z=v[1],
		delta=0x9e3779b9 ;
	sum=delta<<5 ;
	/* start cycle */
	while (n-->0) {
		z-= (y<<4)+k[2] ^ y+sum ^ (y>>5)+k[3] ;
		y-= (z<<4)+k[0] ^ z+sum ^ (z>>5)+k[1] ;
		sum-=delta ;  }
	/* end cycle */
	v[0]=y ; v[1]=z ;
}

void tea_decode_buffer(char* in_buffer, size_t in_size, long* key)
{
	char *p;
	size_t remain = in_size % 8;
	size_t align_size = in_size - remain;
	for (p = in_buffer; p < in_buffer + align_size; p += 8)
		tea_decode( (long*)p, key);
}
