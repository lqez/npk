/* created by ez.amiryo@gmail.com */
// dos-style wildcard comparator
bool b_strcmp( const char* s, const char* d, bool casesensitive = true )
{
	bool asterisk = false;
	int sPos = 0, dPos = 0;

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

