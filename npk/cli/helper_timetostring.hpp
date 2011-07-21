// http://mwultong.blogspot.com/2006/12/c-time-to-string-function-vc.html
char* timeToString(const time_t& tt) {
    static char s[20];

#ifdef NPACK_PLATFORM_WINDOWS
    struct tm t;
    if( localtime_s(&t, &tt) == 0 )
    {
        sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
                  t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
                  t.tm_hour, t.tm_min, t.tm_sec
              );
    }
    else
    {
        sprintf(s, "can't convert time." );
    }
#else
    struct tm* t;
    t = localtime( &tt );
    if( t != NULL )
    {
        sprintf(s, "%04d-%02d-%02d %02d:%02d:%02d",
                  t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                  t->tm_hour, t->tm_min, t->tm_sec
              );
    }
    else
    {
        sprintf(s, "can't convert time." );
    }
#endif

    return s;
}

