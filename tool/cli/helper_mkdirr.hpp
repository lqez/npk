// http://nion.modprobe.de/blog/archives/357-Recursive-directory-creation.html
#ifdef NPK_PLATFORM_WINDOWS
#include <direct.h>
#endif
static void mkdirr(const char *dir) {
	char tmp[256];
	char old;
	char *p = NULL;
	size_t len;

#ifdef NPK_PLATFORM_WINDOWS
	_snprintf(tmp, sizeof(tmp),"%s",dir);
#else
	snprintf(tmp, sizeof(tmp),"%s",dir);
#endif
	len = strlen(tmp);
	if(tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	else if(tmp[len - 1] == '\\')
		tmp[len - 1] = 0;

	for(p = tmp + 1; *p; p++)
		if( (*p == '/') || (*p == '\\') ){
			old = *p;
			*p = 0;
#ifdef NPK_PLATFORM_WINDOWS
			_mkdir(tmp);
#else
			mkdir(tmp, S_IRWXU);
#endif
			*p = old;
		}
#ifdef NPK_PLATFORM_WINDOWS
	_mkdir(tmp);
#else
	mkdir(tmp, S_IRWXU);
#endif
}
