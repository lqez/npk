/*

    npk - General-Purpose File Packing Library
    Copyright (c) 2009-2011 Park Hyun woo(ez.amiryo@gmail.com)

    npk command-line tool
    
    See README for copyright and license information.

*/

#include <iostream>
#include <time.h>
#include <sys/stat.h>
#include <errno.h>
#include <list>
#include <string>
#include <stdio.h>
#include <algorithm>
#include <npk.h>
#include <npk_dev.h>

#include "helper_commify.hpp"
#include "helper_timetostring.hpp"
#include "helper_bstrcmp.h"
#include "helper_mkdirr.hpp"

#ifdef NPK_PLATFORM_WINDOWS
    #include <conio.h>
    #include "helper_dirent.h"
    #define PATH_SEPARATOR '\\'
    #pragma warning( disable : 4996 )
#else
    #include <dirent.h>
    #include <string.h>
    #define PATH_SEPARATOR '/'
    #define strnicmp strncasecmp
#endif

#if defined( NPK_CASESENSITIVE )
    #define CASE_SENSITIVE true
#else
    #define CASE_SENSITIVE false 
#endif

using namespace std;

/* -------------------------------------------------------------------------------------- */
/* TODO: Refactoring - remove duplicated codes in traversal functions and sync functions. */
/* -------------------------------------------------------------------------------------- */

typedef list<NPK_ENTITY> ENTITYLIST;
typedef list<NPK_ENTITY>::iterator ELI;
typedef list<string> STRLIST;
typedef list<string>::iterator SLI;

#define toolversion "1.76"
#define V(x,y) (strcmp(v[x],y) == 0)

char                baseversion[16];
const char          _HR_[] = "-------------------------------------------------------------------------------\n";
const int           readable_size_count = 3;
const unsigned int  readable_size[readable_size_count] = { 1024*1024*1024, 1024*1024, 1024 };
const char*         readable_unit[readable_size_count] = { "GB", "MB", "KB" };
unsigned int        sync_result[4] = { 0, 0, 0, 0 };    // pass, delete, update, add

int                 n = 0;  // current argc number
int                 c;      // argc
char                **v;    // argv

// options
char* output = NULL;
bool verbose = false;
bool modified = false;
bool forceoverwrite = false;
bool norecursive = false;
bool human_readable = false;
bool syncadd = false;
bool syncdelete = false;
bool forceupdate = false;
bool justcreate = false;

NPK_FLAG currentflag;
STRLIST withlist;
STRLIST ignorelist;

NPK_TEAKEY k[4] = { 0, 0, 0, 0 };
NPK_PACKAGE package = NULL;
NPK_ENTITY entity = NULL;

typedef bool( *LOCAL_TFP )( NPK_CSTR fullpath, NPK_CSTR filename );
typedef bool( *PACKAGE_TFP )( NPK_ENTITY entity );

// non-command function
void title();
void version();
void info();
void basic_help();
void bad_syntax();
void help();
void error_n_exit();

// command function
void add();
void create();
void del();
void diff();
void expt();
void flag();
void sort();
void listinfo();
void sync_package();
void sync_and_add( bool sd, bool force, const char* basepath, const char* path );
void sync_only_in_package( bool sd, bool force, const char* path );

// option function
bool has_wildcard_pattern( const char* str );
bool find_option( const char* option, int* pos = NULL );
void get_key();
void get_verbose();
void get_output();
void get_gluetime();
void get_forceoverwrite();
void get_norecursive();
void get_withlist();
void get_ignorelist();
void get_human_readable();
void get_just_create();
void get_sync_options();

// traversal function
bool valid_name( NPK_CSTR name, NPK_CSTR pattern = NULL );
int traverse_local( const char* basepath, const char* path, LOCAL_TFP fp, NPK_CSTR pattern = NULL );
int traverse_package( PACKAGE_TFP fp, NPK_CSTR pattern = NULL );

int main( int _c, char* _v[] )
{
#ifdef NPK_PLATFORM_WINDOWS
    _tzset();
#else
    tzset();
#endif

    sprintf( baseversion, "v%d", NPK_VERSION_CURRENT );

    c = _c;
    v = _v;

    if( c < 2 )
    {
        basic_help();
        exit(1);
    }
    else
    {
        if(V(1,"-help"))
            help();
        else if(V(1,"-version"))
            version();
        else
        {
            if( c < 3 )
                bad_syntax();

            output = v[1];
            get_verbose();
            get_key();
            get_output();
            get_gluetime();
            get_forceoverwrite();
            get_norecursive();
            get_withlist();
            get_ignorelist();
            get_human_readable();
            get_just_create();
            get_sync_options();

            n = 2;

            if( V(2,"-create") )
            {
                create();
            }
            else
            {
                if( ( package = npk_package_open( v[1], k ) ) == NULL )
                {
                    if( justcreate )
                    {
                        create();
                        --n;
                    }
                    else
                        error_n_exit();
                }
            }

            while( n < c )
            {
                if(V(n,"-add")||V(n,"-insert"))
                    add();
                else if(V(n,"-create"))
                {
                    cout << "-create command must be in the first place of commands.\n";
                    error_n_exit();
                }
                else if(V(n,"-delete")||V(n,"-remove"))
                    del();
                else if(V(n,"-diff"))
                    diff();
                else if(V(n,"-export"))
                    expt();
                else if(V(n,"-flag"))
                    flag();
                else if(V(n,"-sort"))
                    sort();
                else if(V(n,"-list")||V(n,"-info"))
                    listinfo();
                else if(V(n,"-sync")||V(n,"-update"))
                    sync_package();
                else
                    ++n;
            }

            if( modified )
            {
                if( verbose )
                    cout << "saving package... ";

                /* For convenient using */
                if( v[1] == output )
                    forceoverwrite = true;
                
                if( npk_package_save( package, output, forceoverwrite ) != NPK_SUCCESS )
                    error_n_exit();

                if( verbose )
                    cout << "finished.\n";
            }
            if( npk_package_close( package ) != NPK_SUCCESS )
                error_n_exit();
        }
    }
    return 0;
}

void basic_help()
{
    cout << "Type 'npk -help' for usage.\n";
}

void bad_syntax()
{
    cout << "Bad syntax error. Please type 'npk -help' for usage.\n";
    exit(-1);
}

void title()
{
    cout << "npk command-line client, version " << toolversion << ".\n";
}

void version()
{
    cout << "npk command-line client, version " << toolversion << ".\n"
        << "Based on libnpk, npk format " << baseversion << ".\n\n"
        << "Copyright (C) 2009-2011 Park Hyun woo(ez.amiryo@gmail.com)\n";
}

void info()
{
    cout << "Checkout the latest version of npk from:\n"
        "    http://npk.googlecode.com/svn/trunk\n";
}

void help()
{
    if( c <= 2 )
    {
        title();
        cout << "usage: npk <package> <command> [args]\n"
            << "Type 'npk -help <command>' for help on a specific command.\n"
            << "Type 'npk -version' to see the program version.\n"
            << "\n"
            << "Available commands:\n"
            << "    -add\n"
            << "    -create\n"
            << "    -delete\n"
            << "    -diff\n"
            << "    -export\n"
            << "    -flag\n"
            << "    -sort\n"
            << "    -list\n"
            << "    -sync\n"
            << "\n"
            << "You can also use multiple commands at once.\n";
        info();
    }
    else
    {
        if(V(2,"add") || V(2,"-add") || V(2,"insert") || V(2,"-insert") )
        {
            cout << "add/insert: Add files into the package as entities.\n"
                << "usage: npk <package> -add <FILE1[@ENTITY]> [FILE2] ...\n"
                << "       You can use wildcard pattern on filename.\n"
                << "\n"
                << "example:\n"
                << "    npk foo.npk -add *.jpg\n"
                << "    npk foo.npk -add bar.jpg@null.jpg --v\n"
                << "    npk foo.npk -add bar.jpg qoo.jpg --k 1:2:3:4\n"
                << "\n"
                << "options:\n"
                << "    --k [--teakey] ARG   : Use ARG as key for TEA/XXTEA. default key is 0:0:0:0.\n"
                << "    --v [--verbose]      : Print working information.\n"
                << "    --g [--gluetime] ARG : Use ARG as gluetime. ARG is UNIX timestamp(epoch).\n"
                << "    --o [--output] ARG   : Save package as new file named ARG.\n"
                << "    --f [--force]        : Force overwrite old package file.\n"
                << "    --jc [--justcreate]  : Create new package automatically if not exist.\n";
        }
        else if(V(2,"create") || V(2,"-create") )
        {
            cout << "create: Create a new npk package.\n"
                << "usage: npk <package> -create\n"
                << "\n"
                << "example:\n"
                << "    npk foo.npk -create\n"
                << "\n"
                << "options:\n"
                << "    --v [--verbose] : Print working information.\n"
                << "    --f [--force]   : Force overwrite old package file.\n";
        }
        else if(V(2,"delete") || V(2,"-delete") || V(2,"remove") || V(2,"-remove") )
        {
            cout << "delete/remove: Delete entities from the package.\n"
                << "usage: npk <package> -delete <ENTITY1> [ENTITY2] ...\n"
                << "       You can use wildcard pattern on ENTITY name.\n"
                << "\n"
                << "example:\n"
                << "    npk foo.npk -delete *.jpg\n"
                << "    npk foo.npk -delete bar.jpg qoo.jpg --k 1:2:3:4\n"
                << "\n"
                << "options:\n"
                << "    --k [--teakey] ARG : Use ARG as key for TEA/XXTEA. default key is 0:0:0:0.\n"
                << "    --v [--verbose]    : Print working information.\n"
                << "    --o [--output] ARG : Save package as new file named ARG.\n"
                << "    --f [--force]      : Force overwrite old package file.\n";
        }
        else if(V(2,"diff") || V(2,"-diff") )
        {
            cout << "diff: Show differences between the package and local path.\n"
                << "usage: npk <package> -diff <PATH>\n"
                << "\n"
                << "example:\n"
                << "    npk foo.npk -diff /home/users/lqez/bar\n"
                << "    npk foo.npk -diff d:\\foo --k 1:2:3:4\n"
                << "\n"
                << "options:\n"
                << "    --k [--teakey] ARG : Use ARG as key for TEA/XXTEA. default key is 0:0:0:0.\n"
                << "    --v [--verbose]    : Print working information.\n";
        }
        else if(V(2,"export") || V(2,"-export") )
        {
            cout << "export: Export entities into local files from the package.\n"
                << "usage: npk <package> -export <ENTITY1[@FILE]> [ENTITY2] ...\n"
                << "       You can use wildcard pattern on ENTITY name.\n"
                << "note: If entity name contains directory, \n"
                << "      npk will create subdirectories automatically.\n"
                << "\n"
                << "example:\n"
                << "    npk foo.npk -export *.jpg\n"
                << "    npk foo.npk -export bar.jpg@new_bar.jpg --v\n"
                << "    npk foo.npk -export bar.jpg qoo.jpg --k 1:2:3:4\n"
                << "\n"
                << "options:\n"
                << "    --k [--teakey] ARG : Use ARG as key for TEA/XXTEA. default key is 0:0:0:0.\n"
                << "    --v [--verbose]    : Print working information.\n"
                << "    --f [--force]      : Force overwrite old file.\n";
        }
        else if(V(2,"flag") || V(2,"-flag") )
        {
            cout << "flag: Set flag(property) of entity in the package.\n"
                << "usage: npk <package> -flag <ENTITY1[@FLAG1][@FLAG2]...> [ENTITY2] ...\n"
                << "\n"
                << "flags:\n"
                << "    COMPRESS [C] : Compress entity with zlib\n"
                << "    BZIP2    [B] : Compress entity with bzip2\n"
                << "    ENCRYPT  [E] : Encrypt entity with tea\n"
                << "    XXTEA    [X] : Encrypt entity with xxtea\n"
                << "\n"
                << "example:\n"
                << "    npk foo.npk -flag *.jpg@ENCRYPT\n"
                << "    npk foo.npk -flag bar.jpg@C@E --v\n"
                << "    npk foo.npk -flag bar.jpg --k 1:2:3:4   /* to remove flag */\n"
                << "\n"
                << "options:\n"
                << "    --k [--teakey] ARG : Use ARG as key for TEA/XXTEA. default key is 0:0:0:0.\n"
                << "    --v [--verbose]    : Print working information.\n"
                << "    --o [--output] ARG : Save package as new file named ARG.\n"
                << "    --f [--force]      : Force overwrite old package file.\n";
        }
        else if(V(2,"sort") || V(2,"-sort") )
        {
            cout << "sort: Sort entities by rules\n"
                << "usage: npk <package> -sort <ENTITY1[@ORDER]> [RULE2] ...\n"
                << "       You can use wildcard pattern on filename.\n"
                << "note: Rules are applied in order. Remain entities are shipped by no order\n"
                << "\n"
                << "order:\n"
                << "    ASC  [A] : Sort entities by ascending order\n"
                << "    DESC [D] : Sort entities by descending order\n"
                << "\n"
                << "example:\n"
                << "    npk foo.npk -sort *@A\n"
                << "    npk foo.npk -flag *.xml *.thumbnail.*@ASC *@ASC --v\n"
                << "\n"
                << "options:\n"
                << "    --k [--teakey] ARG : Use ARG as key for TEA/XXTEA. default key is 0:0:0:0.\n"
                << "    --v [--verbose]    : Print working information.\n"
                << "    --o [--output] ARG : Save package as new file named ARG.\n"
                << "    --f [--force]      : Force overwrite old package file.\n";
        }
        else if(V(2,"list") || V(2,"-list") || V(2,"info") || V(2,"-info") )
        {
            cout << "list/info: Show package information and entity list.\n"
                << "usage: npk <package> -list [PATTERN]\n"
                << "\n"
                << "example:\n"
                << "    npk foo.npk -list\n"
                << "    npk foo.npk -list *.jpg --k 1:2:3:4\n"
                << "\n"
                << "options:\n"
                << "    --k [--teakey] ARG     : Use ARG as key for TEA/XXTEA. default key is 0:0:0:0.\n"
                << "    --v [--verbose]        : Print working information.\n"
                << "    --h [--human-readable] : Support enhanced readability.\n";
        }
        else if( V(2,"sync") || V(2,"-sync") || V(2,"update") || V(2,"-update") )
        {
            cout << "sync/update: Synchronize the package with local files.\n"
                << "usage: npk <package> -sync <PATH>\n"
                << "\n"
                << "example:\n"
                << "    npk foo.npk -sync /home/users/lqez/bar --k 1:2:3:4\n"
                << "    npk foo.npk -sync c:\\product\\release --sa --sd\n"
                << "    npk foo.npk -sync c:\\product\\release --sa --ig *.tmp *.pdb\n"
                << "\n"
                << "options:\n"
                << "    --k [--teakey] ARG     : Use ARG as key for TEA/XXTEA. default key is 0:0:0:0.\n"
                << "    --v [--verbose]        : Print working information.\n"
                << "    --g [--gluetime] ARG   : Use ARG as gluetime. ARG is UNIX timestamp(epoch).\n"
                << "    --o [--output] ARG     : Save package as new file named ARG.\n"
                << "    --f [--force]          : Force overwrite old package file.\n"
                << "    --jc [--justcreate]    : Create new package automatically if not exist.\n"
                << "    --nr [--norecursive]   : Do not sync child directories.\n"
                << "    --fu [--forceupdate]   : Force update with older local files.\n"
                << "    --sa [--syncadd]       : Add new local files into the package.\n"
                << "    --sd [--syncdelete]    : Delete entities that not exist in local disk.\n"
                << "    --wo [--withonly] ARGS : Sync ARGS only(wildcard pattern).\n"
                << "    --ig [--ignore] ARGS   : Do not sync ARGS(wildcard pattern).\n"
                << "                             You can use --wo and --ig at same time,\n"
                << "                             but --wo ARGS will be ignored by --ig ARGS.\n";
        }
    }
}

void error_n_exit()
{
    int err = g_npkError;
    cout << npk_error_to_str(err) << "\n";
    exit(err);
}

bool has_wildcard_pattern( const char* str )
{
    if( !str )
        return false;

    size_t len = strlen(str);
    for( size_t i = 0; i < len; ++i )
    {
        if( str[i] == '?' || str[i] =='*' )
            return true;
    }
    return false;
}

bool find_option( const char* option, int* pos )
{
    char buf[512];
    sprintf( buf, "--%s", option );

    for( int i = 0; i < c; ++i )
    {
        if( strncmp( v[i], buf, strlen(buf) ) == 0 )
        {
            if( ( pos != NULL ) && ( i < c-1 ) )
                *pos = i + 1;;
            return true;
        }
    }
    return false;
}

void get_key()
{
    int pos = -1;
    if( find_option( "k", &pos ) || find_option( "teakey", &pos ) )
    {
        if( pos == -1 )
            bad_syntax();

        char buf[512];
        char* c;
        c = buf;
        strcpy( buf, v[pos] );

        for( int i = 0; i < 4; ++i )
        {
            char* colonpos = strchr( c, ':' );
            if( ( i != 3 ) && ( !colonpos ) )
                bad_syntax();
            k[i] = atoi( c );
            c = colonpos + 1;
        }

        if( verbose )
            cout << "using key " << v[pos] << "\n";

    }
}

void get_verbose()
{
    verbose = find_option( "v" ) || find_option( "verbose" );
}

void get_output()
{
    int pos = -1;
    if( find_option( "o", &pos ) || find_option( "output", &pos ) )
    {
        if( pos == -1 )
            bad_syntax();

        output = v[pos];

        if( verbose )
            cout << "using output target " << output << "\n";
    }
}

void get_gluetime()
{
    int pos = -1;
    if( find_option( "g", &pos ) || find_option( "gluetime", &pos ) )
    {
        if( pos == -1 )
            bad_syntax();


        NPK_TIME gluetime;
        gluetime = atoi( v[pos] );
        npk_enable_gluetime( gluetime );

        if( verbose )
            cout << "using gluetime " << timeToString(gluetime) << "\n";
    }
}

void get_forceoverwrite()
{
    forceoverwrite = find_option( "f" ) || find_option( "force" );
}

void get_norecursive()
{
    norecursive = find_option( "nr" ) || find_option( "norecursive" );
}

void get_human_readable()
{
    human_readable = find_option( "h" ) | find_option( "human-readable" );
}

void get_just_create()
{
    justcreate = find_option( "jc" ) | find_option( "justcreate" );
}

void get_sync_options()
{
    syncadd = find_option( "sa" ) | find_option( "syncadd" );
    syncdelete = find_option( "sd" ) | find_option( "syncdelete" );
    forceupdate = find_option( "fu" ) | find_option( "forceupdate" );
}

void get_withlist()
{
    withlist.clear();

    int pos = -1;
    if( find_option( "ow", &pos ) || find_option( "onlywith", &pos ) )
    {
        if( pos == -1 )
            bad_syntax();

        if( verbose )
            cout << "using withonly filter ";

        while( pos < c )
        {
            if( v[pos][0] == '-' )
                break;

            if( withlist.size() > 0 )
                cout << ", ";
            cout << v[pos];

            withlist.push_back( v[pos] );
            ++pos;
        }

        if( verbose )
        {
            cout << "\n";
            cout << _HR_;
        }
    }
}
void get_ignorelist()
{
    ignorelist.clear();

    int pos = -1;
    if( find_option( "ig", &pos ) || find_option( "ignore", &pos ) )
    {
        if( pos == -1 )
            bad_syntax();

        if( verbose )
            cout << "using ignore filter ";

        while( pos < c )
        {
            if( v[pos][0] == '-' )
                break;

            if( verbose )
            {
                if( ignorelist.size() > 0 )
                    cout << ", ";
                cout << v[pos];
            }
            ignorelist.push_back( v[pos] );
            ++pos;
        }

        if( verbose )
        {
            cout << "\n";
            cout << _HR_;
        }
    }
}

bool valid_name( NPK_CSTR name, NPK_CSTR pattern )
{
    bool with = true;

    if( pattern != NULL )
    {
        with = b_strcmp( pattern, name, CASE_SENSITIVE );
    }
    else
    {
        // check 'withonly' list
        if( withlist.size() > 0 )
        {
            SLI iter = withlist.begin();
            while( iter != withlist.end() )
            {
                if( b_strcmp( (*iter).c_str(), name, CASE_SENSITIVE ) )
                {
                    with = true;
                    break;
                }
                ++iter;
            }
        }
    }

    if( !with )
        return false;

    // check 'ignorable' list
    if( ignorelist.size() > 0 )
    {
        SLI iter = ignorelist.begin();
        while( iter != ignorelist.end() )
        {
            if( b_strcmp( (*iter).c_str(), name, CASE_SENSITIVE ) )
                return false;
            ++iter;
        }
    }
    return true;
}

bool add_tfp( NPK_CSTR fullpath, NPK_CSTR filename )
{
    if( verbose )
        cout << "    " << filename << "\n";

    if( npk_package_add_file( package, fullpath, filename, NULL ) != NPK_SUCCESS )
        error_n_exit();

    return true;
}

void add()
{
    int count = 0;

    while( n < c )
    {
        ++n;
        if( n >= c )
            break;
        if( v[n][0] == '-' )
            break;

        if( verbose )
            cout << "adding " << v[n] << "\n";

        if( has_wildcard_pattern( v[n] ) )
        {
            char fullpath[512];
            strcpy( fullpath, v[n] );
            char *pattern = v[n], *atpos = NULL;

            if( ( atpos = strchr( fullpath, PATH_SEPARATOR ) ) )
            {
                *atpos = '\0';
                pattern = atpos + 1;
            }
            else
            {
                sprintf( fullpath, "." );
            }
            count += traverse_local( fullpath, fullpath, add_tfp, pattern );
        }
        else
        {
            char filename[512];
            strcpy( filename, v[n] );
            char *entityname = NULL, *atpos = NULL;

            if( ( atpos = strchr( filename, '@' ) ) )
            {
                *atpos = '\0';
                entityname = atpos + 1;
            }

            if( npk_package_add_file( package, filename, entityname, NULL ) != NPK_SUCCESS )
                error_n_exit();
            ++count;
        }
    }

    if( verbose )
        cout << count << " file(s) added.\n";
    if( count > 0 )
        modified = true;
}


void create()
{
    if( npk_package_alloc( &package, k ) != NPK_SUCCESS )
        error_n_exit();
    if( npk_package_save( package, v[1], forceoverwrite ) != NPK_SUCCESS )
        error_n_exit();
    if( npk_package_close( package ) != NPK_SUCCESS )
        error_n_exit();

    if( verbose )
        cout << "package " << v[1] << " has been created successfully.\n";

    if( ( package = npk_package_open( v[1], k ) ) == NULL )
        error_n_exit();
    ++n;
}

bool del_tfp( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = (NPK_ENTITYBODY*)entity;

    if( verbose )
        cout << "    " << eb->name_ << "\n";
    if( npk_package_remove_entity( package, entity ) != NPK_SUCCESS )
        error_n_exit();

    return true;
}

void del()
{
    int count = 0;
    NPK_ENTITY entity = NULL;

    while( n < c )
    {
        ++n;
        if( n >= c )
            break;
        if( v[n][0] == '-' )
            break;

        if( verbose )
            cout << "removing " << v[n] << "\n";

        if( has_wildcard_pattern( v[n] ) )
        {
            count += traverse_package( del_tfp, v[n] );
        }
        else
        {
            entity = npk_package_get_entity( package, v[n] );
            if( entity == NULL )
                error_n_exit();
            if( npk_package_remove_entity( package, entity ) != NPK_SUCCESS )
                error_n_exit();
            ++count;
        }
    }

    if( verbose )
        cout << count << " entity(s) removed.\n";
    if( count > 0 )
        modified = true;
}

void diff()
{
    NPK_PACKAGEBODY* pb = (NPK_PACKAGEBODY*)package;
    NPK_ENTITYBODY* eb = pb->pEntityHead_;

    ++n;

    if( n >= c )
        bad_syntax();
    if( v[n][0] == '-' )
        bad_syntax();

    if( verbose )
    {
        cout << "Package      : " << output << "\n";
        cout << "Version      : " << pb->info_.version_ << "\n";
        cout << "Total entity : " << pb->info_.entityCount_ << "\n";
        cout << _HR_;
        cout << "     status       "
             << " name" << "\n";
        cout << _HR_;
    }


    char path[255], buf[512];
    unsigned int count[4] = { 0,0,0,0 };
    size_t len = strlen(path)-1;
    strcpy( path, v[n] );

    // remove last \ mark
    if( path[len] == PATH_SEPARATOR )
        path[len] = '\0';

    struct stat __sbuf;
    int result;
    int status = 0; // 0:not found, 1:same, 2:package is latest, 3:local is latest

    while( eb )
    {
        sprintf( buf, "%s%c%s", path, PATH_SEPARATOR, eb->name_ );
        result = stat( buf, &__sbuf );

        if( result != 0 )
        {
            switch( errno )
            {
            case ENOENT:
                status = 0;
                break;
            default:
                /* Should never be reached. */
                cout << "Unexpected error in _stat call.\n";
                exit(-1);
            }
        }
        else
        {
            if( __sbuf.st_mtime > eb->info_.modified_ )
                status = 3;
            else if( __sbuf.st_mtime < eb->info_.modified_ )
                status = 2;
            else
                status = 1;
        }
        ++count[status];

        if( verbose )
        {
            switch( status )
            {
            case 0: cout << "not found        ? "; break;
            case 1: cout << "same             = "; break;
            case 2: cout << "newer than local + "; break;
            case 3: cout << "older than local - "; break;
            }
        }
        else
        {
            switch( status )
            {
            case 0: cout << "? "; break;
            case 1: cout << "= "; break;
            case 2: cout << "+ "; break;
            case 3: cout << "- "; break;
            }
        }
        cout << eb->name_ << "\n";
        eb = eb->next_;
    }

    if( verbose )
    {
        cout << _HR_;
        if( count[1] > 0 )
            cout << count[1] << " entity(s) are same.\n";
        if( count[2] > 0 )
            cout << count[2] << " entity(s) are newer than local.\n";
        if( count[3] > 0 )
            cout << count[3] << " entity(s) are older than local.\n";
        if( count[0] > 0 )
            cout << count[0] << " entity(s) are not found in local.\n";
    }
}

NPK_RESULT expt_with_dir_check( NPK_ENTITY entity, const char* filename )
{
    // check directory
    char pathname[512];
    char* separator = 0;
    strcpy( pathname, filename );

    if( ( separator = strchr( pathname, '/' ) ) )
    {
        *separator = '\0';
        mkdirr( pathname );
    }
    else if( ( separator = strchr( pathname, '\\' ) ) )
    {
        *separator = '\0';
        mkdirr( pathname );
    }

    return( npk_entity_export( entity, filename, forceoverwrite ) );
}

bool expt_tfp( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = (NPK_ENTITYBODY*)entity;

    if( verbose )
        cout << "    " << eb->name_ << "\n";

    if( expt_with_dir_check( entity, eb->name_ ) != NPK_SUCCESS )
        error_n_exit();

    return true;
}

void expt()
{
    int count = 0;

    while( n < c )
    {
        ++n;
        if( n >= c )
            break;
        if( v[n][0] == '-' )
            break;

        if( has_wildcard_pattern( v[n] ) )
        {
            if( verbose )
                cout << "exporting " << v[n] << "\n";
            count += traverse_package( expt_tfp, v[n] );
        }
        else
        {
            char entityname[512];
            strcpy( entityname, v[n] );
            char *filename = NULL, *atpos = NULL;

            if( ( atpos = strchr( entityname, '@' ) ) )
            {
                *atpos = '\0';
                filename = atpos + 1;
            }
            else
                filename = entityname;

            if( verbose )
            {
                if( filename )
                    cout << "exporting " << entityname << " as " << filename << "\n";
                else
                    cout << "exporting " << v[n] << "\n";
            }

            if( !(entity = npk_package_get_entity( package, entityname ) ) )
                error_n_exit();

            if( expt_with_dir_check( entity, filename ) != NPK_SUCCESS )
                error_n_exit();

            ++count;
        }
    }

    if( verbose )
        cout << count << " file(s) exported.\n";
}

bool flag_tfp( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = (NPK_ENTITYBODY*)entity;

    if( eb->info_.flag_ != currentflag )
    {
        if( verbose )
            cout << "    " << eb->name_ << "\n";
        if( npk_entity_set_flag( entity, currentflag ) != NPK_SUCCESS )
            error_n_exit();
        return true;
    }
    return false;
}

void flag()
{
    int count = 0;

    while( n < c )
    {
        ++n;
        if( n >= c )
            break;
        if( v[n][0] == '-' )
            break;

        char entityname[512];
        strcpy( entityname, v[n] );
        char *flagchar = NULL;
        NPK_FLAG flag = NPK_ENTITY_NULL;
        if( NPK_VERSION_CURRENT >= NPK_VERSION_REFACTORING )
            flag = NPK_ENTITY_REVERSE;

        flagchar = entityname;

        while( ( flagchar = strchr( flagchar, '@' ) ) )
        {
            *flagchar = '\0';
            flagchar++;
            if( ( strnicmp( flagchar, "C", 1 ) == 0 )
             || ( strnicmp( flagchar, "COMPRESS", 8 ) == 0 ) )
                flag |= NPK_ENTITY_COMPRESS_ZLIB;
            else if( ( strnicmp( flagchar, "E", 1 ) == 0 )
             || ( strnicmp( flagchar, "ENCRYPT", 7 ) == 0 ) )
                flag |= NPK_ENTITY_ENCRYPT_TEA;
            else if( ( strnicmp( flagchar, "B", 1 ) == 0 )
             || ( strnicmp( flagchar, "BZIP", 4 ) == 0 ) )
                flag |= NPK_ENTITY_COMPRESS_BZIP2;
            else if( ( strnicmp( flagchar, "X", 1 ) == 0 )
             || ( strnicmp( flagchar, "XXTEA", 5 ) == 0 ) )
                flag |= NPK_ENTITY_ENCRYPT_XXTEA;
        }

        if( verbose )
        {
            cout << "changing " << entityname;
            if( flag & NPK_ENTITY_COMPRESS_ZLIB )
                cout << " -compress by zlib";
            if( flag & NPK_ENTITY_ENCRYPT_TEA )
                cout << " -encrypt by tea";
            if( flag & NPK_ENTITY_COMPRESS_BZIP2 )
                cout << " -compress by bzip2";
            if( flag & NPK_ENTITY_ENCRYPT_XXTEA )
                cout << " -encrypt by xxtea";
            cout << "\n";
        }

        if( has_wildcard_pattern( v[n] ) )
        {
            currentflag = flag;
            count += traverse_package( flag_tfp, entityname );
        }
        else
        {
            entity = npk_package_get_entity( package, entityname );
            if( !entity )
                error_n_exit();

            NPK_FLAG currentEntityFlag = NPK_ENTITY_NULL;

            if( npk_entity_get_current_flag( entity, &currentEntityFlag ) != NPK_SUCCESS )
                error_n_exit();

            if( currentEntityFlag != flag )
            {
                NPK_ENTITYBODY* eb = (NPK_ENTITYBODY*)entity;
                if( verbose )
                    cout << "    " << eb->name_ << "\n";
                if( npk_entity_set_flag( entity, flag ) != NPK_SUCCESS )
                    error_n_exit();
                ++count;
            }
        }
    }

    if( verbose )
        cout << count << " entity(s) changed.\n";
    if( count > 0 )
        modified = true;
}

bool listinfo_tfp( NPK_ENTITY entity )
{
    NPK_ENTITYBODY* eb = (NPK_ENTITYBODY*)entity;

    char buf1[80], buf2[80], size1[80], size2[80];
    bool readable_out;

    if( eb->info_.size_ == 0 )
    {
        printf( "    <not stored yet>    %c%c%c%c ---------- -------- %s\n",
            (eb->newflag_ & NPK_ENTITY_COMPRESS_ZLIB)?'C':' ',
            (eb->newflag_ & NPK_ENTITY_ENCRYPT_TEA)?'E':' ',
            (eb->newflag_ & NPK_ENTITY_COMPRESS_BZIP2)?'B':' ',
            (eb->newflag_ & NPK_ENTITY_ENCRYPT_XXTEA)?'X':' ',
            eb->name_ );
    }
    else
    {
        readable_out = false;

        if( human_readable )
        {
            for( int i = 0; i < readable_size_count; ++i )
            {
                if( readable_size[i] <= eb->info_.originalSize_ )
                {
                    commify( (double)eb->info_.size_ / readable_size[i], buf1, 1 ),
                    commify( (double)eb->info_.originalSize_ / readable_size[i], buf2, 1 ),
                    sprintf( size1, "%8s %2s", buf1, readable_unit[i] );
                    sprintf( size2, "%8s %2s", buf2, readable_unit[i] );

                    readable_out = true;
                    break;
                }
            }
        }

        if( !readable_out )
        {
            commify( eb->info_.size_, size1, 0 );
            commify( eb->info_.originalSize_, size2, 0 );
        }

        printf( "%11s %11s %c%c%c%c %17s %s\n",
            size1,
            size2,
            (eb->info_.flag_ & NPK_ENTITY_COMPRESS_ZLIB)?'C':' ',
            (eb->info_.flag_ & NPK_ENTITY_ENCRYPT_TEA)?'E':' ',
            (eb->info_.flag_ & NPK_ENTITY_COMPRESS_BZIP2)?'B':' ',
            (eb->info_.flag_ & NPK_ENTITY_ENCRYPT_XXTEA)?'X':' ',
            timeToString( eb->info_.modified_ ),
            eb->name_ );
    }
    return true;
}

void listinfo()
{
    NPK_PACKAGEBODY* pb = (NPK_PACKAGEBODY*)package;
    ++n;
    char* filter = NULL;

    if( n < c )
    {
        if( v[n][0] != '-' )
        {
            filter = v[n];
        }
    }

    cout << "Package      : " << output << "\n";
    cout << "Version      : " << pb->info_.version_ << "\n";
    cout << "Total entity : " << pb->info_.entityCount_ << "\n";
    if( pb->info_.version_ >= NPK_VERSION_PACKAGETIMESTAMP )
        cout << "Timestamp    : " << timeToString( pb->modified_) << "\n";
    if( filter )
        cout << "Using filter : " << filter << "\n";

    cout << _HR_;
    cout << "       size "
         << "   original "
         << "flag "
         << "   date    "
         << "  time   "
         << "name" << "\n";
    cout << _HR_;

    traverse_package( listinfo_tfp, filter );
    cout << _HR_;
}

void sync_package()
{
    NPK_PACKAGEBODY* pb = (NPK_PACKAGEBODY*)package;

    if( verbose )
    {
        cout << "Package      : " << output << "\n";
        cout << "Version      : " << pb->info_.version_ << "\n";
        cout << "Total entity : " << pb->info_.entityCount_ << "\n";
        cout << _HR_;
    }
    ++n;

    if( n >= c )
        bad_syntax();
    if( v[n][0] == '-' )
        bad_syntax();

    char path[255];
    strcpy( path, v[n] );
    size_t len = strlen(path)-1;

    // remove last \ mark
    if( path[len] == PATH_SEPARATOR )
        path[len] = '\0';

    // reset count
    for( int i = 0; i < 4; ++i )
        sync_result[i] = 0;

    sync_only_in_package( syncdelete, forceupdate, path );

    if( syncadd )
        sync_and_add( syncdelete, forceupdate, path, path );

    // calc count
    int total_count = 0;
    for( int i = 1; i < 4; ++i )
        total_count += sync_result[i];

    if( total_count > 0 )
        modified = true;
    
    if( verbose )
    {
        if( total_count == 0 )
        {
            cout << "Package is up-to-date. Nothing changed.\n";
        }
        else
        {
            cout << _HR_;
            if( sync_result[3] > 0 )
                cout << sync_result[3] << " entity(s) was added.\n";
            if( sync_result[2] > 0 )
                cout << sync_result[2] << " entity(s) was updated.\n";
            if( sync_result[1] > 0 )
                cout << sync_result[1] << " entity(s) was deleted.\n";
        }
    }
}

int traverse_local( const char* basepath, const char* path, LOCAL_TFP fp, NPK_CSTR pattern )
{
    int count = 0;
    DIR *pDIR;
    struct dirent *pDirEnt;

    /* Open the current directory */
    pDIR = opendir( path );
    if ( pDIR == NULL ) {
        fprintf( stderr, "%s %d: opendir() failed (%s)\n",
                 __FILE__, __LINE__, strerror( errno ));
        exit( -1 );
    }

    /* Get each directory entry from pDIR and print its name */
    char buf[512];
    char *rel_name = NULL;
    int result;
    size_t basepath_len = strlen(basepath)+1;

    pDirEnt = readdir( pDIR );
    struct stat __sbuf;
    while ( pDirEnt != NULL ) {
        if( ( strncmp( pDirEnt->d_name, ".", 1 ) != 0 )
         && ( strncmp( pDirEnt->d_name, "..", 2 ) != 0 ) )
        {
            sprintf( buf, "%s%c%s", path, PATH_SEPARATOR, pDirEnt->d_name );
            rel_name = &buf[basepath_len];

            result = stat( buf, &__sbuf );
            if( result == 0 )
            {
                if( valid_name( pDirEnt->d_name, pattern ) )
                {
                    if( __sbuf.st_mode & S_IFDIR )
                    {   // this is a directory
                        if( !norecursive )
                            count += traverse_local( basepath, buf, fp );
                    }
                    else
                    {   // this is a file
                        ++count;
                        fp( buf, rel_name );
                    }
                }
            }
        }
        pDirEnt = readdir( pDIR );
    }

    /* Release the open directory */
    closedir( pDIR );
    
    return count;
}

int traverse_package( PACKAGE_TFP fp, NPK_CSTR pattern )
{
    NPK_PACKAGEBODY* pb = (NPK_PACKAGEBODY*)package;
    NPK_ENTITYBODY* eb = pb->pEntityHead_;
    NPK_ENTITYBODY* eb_;

    int count = 0;
    while( eb )
    {
        eb_ = eb;
        eb = eb->next_;
        if( valid_name( eb_->name_, pattern ) )
        {
            if( fp( eb_ ) )
                ++count;
        }
    }
    return count;
}

void sync_and_add( bool sd, bool force, const char* basepath, const char* path )
{

    DIR *pDIR;
    struct dirent *pDirEnt;

    /* Open the current directory */

    pDIR = opendir( path );

    if ( pDIR == NULL ) {
        fprintf( stderr, "%s %d: opendir() failed (%s)\n",
                 __FILE__, __LINE__, strerror( errno ));
        exit( -1 );
    }

    /* Get each directory entry from pDIR and print its name */
    char buf[512];
    char *rel_name = NULL;
    int result;
    size_t basepath_len = strlen(basepath)+1;

    pDirEnt = readdir( pDIR );
    struct stat __sbuf;
    while ( pDirEnt != NULL ) {
        if( ( strncmp( pDirEnt->d_name, ".", 1 ) != 0 )
         && ( strncmp( pDirEnt->d_name, "..", 2 ) != 0 ) )
        {
            sprintf( buf, "%s%c%s", path, PATH_SEPARATOR, pDirEnt->d_name );
            rel_name = &buf[basepath_len];

            result = stat( buf, &__sbuf );
            if( result == 0 )
            {
                if( valid_name( pDirEnt->d_name ) )
                {
                    if( __sbuf.st_mode & S_IFDIR )
                    {   // this is a directory
                        if( !norecursive )
                        {
                            sync_and_add( sd, force, basepath, buf );
                        }
                    }
                    else
                    {
                        if( NULL == npk_package_get_entity( package, rel_name ) )
                        {
                            if( verbose )
                                cout << "adding ";
                            else
                                cout << "A ";

                            cout << rel_name << "\n";

                            if( NPK_SUCCESS != npk_package_add_file( package, buf, rel_name, NULL ) )
                                error_n_exit();

                            ++sync_result[3];
                        }
                    }
                }
            }
        }

        pDirEnt = readdir( pDIR );
    }

    /* Release the open directory */

    closedir( pDIR );
}

void sync_only_in_package( bool sd, bool force, const char* path )
{
    NPK_PACKAGEBODY* pb = (NPK_PACKAGEBODY*)package;
    NPK_ENTITYBODY* eb = pb->pEntityHead_;

    char buf[512];
    bool ignorable;
    struct stat __sbuf;
    int result;
    int status = 0; // 0:delete, 1:pass, 2:update
    ENTITYLIST el;

    while( eb )
    {
        status = 0;
        ignorable = false;
        SLI iter = ignorelist.begin();
        
        while( iter != ignorelist.end() )
        {
            if( b_strcmp_path_delimited( (*iter).c_str(), eb->name_, PATH_SEPARATOR, CASE_SENSITIVE ) )
            {
                ignorable = true;
                break;
            }
            ++iter;
        }

        if( norecursive )
        {
            if( strchr( eb->name_, PATH_SEPARATOR ) )
            {
                ignorable = true;
            }
        }

        if( ignorable )
        {
            eb = eb->next_;
            continue;
        }

        if ( status == 0 )
        {
            sprintf( buf, "%s%c%s", path, PATH_SEPARATOR, eb->name_ );
            result = stat( buf, &__sbuf );

            if( result != 0 )
            {
                switch( errno )
                {
                case ENOENT:
                    if( sd )
                    {
                        status = 1;
                    }
                    break;
                default:
                    /* Should never be reached. */
                    cout << "Unexpected error in _stat call.\n";
                    exit(-1);
                }
            }
            else
            {
                if( __sbuf.st_mtime > eb->info_.modified_ )
                {
                    status = 2;
                }
                else if( ( __sbuf.st_mtime < eb->info_.modified_ ) || ( force ) )
                {
                    status = 2;
                }
            }
        }

        ++sync_result[status];

        if( verbose )
        {
            switch( status )
            {
            case 1: cout << "deleting "; break;
            case 2: cout << "updating "; break;
            }
        }
        else
        {
            switch( status )
            {
            case 1: cout << "D "; break;
            case 2: cout << "U "; break;
            }
        }
        if( ( status == 2 ) || ( status == 1 ) )
            cout << eb->name_ << "\n";

        switch( status )
        {
        case 1:
            el.push_back( eb );
            break;
        case 2:
            npk_alloc_copy_string( &eb->localname_, buf );
            break;
        }
        eb = eb->next_;
    }

    ELI iter = el.begin();
    while( iter != el.end() )
    {
        npk_package_remove_entity( package, *iter );
        ++iter;
    }
}
