/* created by ez.amiryo@gmail.com */
// msdos-like wildcard comparator
bool b_strcmp( const char* s, const char* d, bool casesensitive = true )
{
    bool asterisk = false;
    int sPos = 0, dPos = 0;

    if( !s || !d ) return false;

    while(1)
    {
        if( s[sPos] == '*' )
        {
            ++sPos;
            asterisk = true;
        }

        if( ( s[sPos] != d[dPos] ) && ( s[sPos] != '?' ) && !asterisk )
        {
            // big and small letter check
            if( !casesensitive )
            {
                if( s[sPos] >= 'a' && s[sPos] <= 'z' && d[dPos] >= 'A' && d[dPos] <= 'Z' )
                {
                    if( (s[sPos] - 32) != d[dPos] )
                        return false;
                }
                else if( s[sPos] >= 'A' && s[sPos] <= 'Z' && d[dPos] >= 'a' && d[dPos] <= 'z' )
                {
                    if( (s[sPos] + 32) != d[dPos] )
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else if( s[sPos] =='\0' )
        {
            if( d[dPos] != '\0' && !asterisk )
                return false;
            else
                break;
        }
        else
        {
            if( d[dPos] == '\0' )
                return false;
        }

        if( asterisk )
        {
            if( d[dPos] == s[sPos] )
            {
                asterisk = false;
                ++sPos;
            }
        }
        else
            ++sPos;
        ++dPos;
    }
    return true;
}
 
bool b_strncmp( const char* s, const char* d, size_t dlength, bool casesensitive = true )
{
    bool asterisk = false;
    int sPos = 0, dPos = 0;

    if( !s || !d ) return false;

    for(;;)
    {
        if( s[sPos] == '*' )
        {
            ++sPos;
            asterisk = true;
        }

        char dChar = '\0';
        if ( (unsigned int)( dPos ) < dlength )
        {
            dChar = d[dPos];
        }

        if( ( s[sPos] != dChar ) && ( s[sPos] != '?' ) && !asterisk )
        {
            // big and small letter check
            if( !casesensitive )
            {
                if( s[sPos] >= 'a' && s[sPos] <= 'z' && dChar >= 'A' && dChar <= 'Z' )
                {
                    if( (s[sPos] - 32) != dChar )
                        return false;
                }
                else if( s[sPos] >= 'A' && s[sPos] <= 'Z' && dChar >= 'a' && dChar <= 'z' )
                {
                    if( (s[sPos] + 32) != dChar )
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else if( s[sPos] =='\0' )
        {
            if( dChar != '\0' && !asterisk )
                return false;
            else
                break;
        }
        else
        {
            if( dChar == '\0' )
                return false;
        }

        if( asterisk )
        {
            if( d[dPos] == s[sPos] )
            {
                asterisk = false;
                ++sPos;
            }
        }
        else
            ++sPos;
        ++dPos;
    }
    return true;
}

bool b_strcmp_path_delimited( const char* s, const char* d, char delimiter, bool casesensitive = true )
{
    if( !s || !d ) return false;

    const char* begin = d;
    const char* end = d;

    for ( ; ; )
    {
        for( ; *end != '\0' && *end != delimiter ; ++end ) {}
        if ( b_strncmp( s, begin, end - begin, casesensitive ) )
        {
            return true;
        }

        if ( *end == '\0' )
        {
            break;
        }

        ++end;
        begin = end;
    }

    return false;
}

