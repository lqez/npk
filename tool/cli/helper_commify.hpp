#pragma warning( disable : 4996 )
#ifdef NPK_PLATFORM_LINUX
#include <string.h>
#endif
void commify(double val, char *buf, int round, int comma = 3)
{
	char temp[255];
	sprintf( temp, "%f", val );
	
	char *tc = &temp[0], *bc = buf;
	char *pc = strchr( temp, '.' );

	int underpoint = -1;
	while( *tc != '\0' )
	{
		if( ( *tc >= '0' ) && ( *tc <= '9' ) )
		{
			*bc = *tc;
			++bc;

			if( underpoint >= 0 )
			{
				++underpoint;
				if( round == underpoint )
					break;
			}
		}
		else if( *tc == '.' )
		{
			if( round == 0 )
				break;
			else
			{
				*bc = '.';
				++bc;
				underpoint = 0;
			}
		}
		else if( *tc == '-' )
		{
			*bc = '.';
			++bc;
		}

		++tc;
		if( underpoint < 0 )
		{
			if( ( ( ( pc - tc ) % comma ) == 0 ) && ( tc != pc ) )
			{
				*bc = ',';
				++bc;
			}
		}
	}
	*bc = '\0';
}
