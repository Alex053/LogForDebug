/*
 * Created on Nov 15, 2016
 *
 * @author: Jorge Alejandro Caloca Hernandez
 */

#ifndef _DEBUGGING_H
#define _DEBUGGING_H "Debugging.h"

/*
 * Types of Levels for Logs
 */
typedef enum
{
  L_DEFAULT,
  L_INFO,
  L_ERROR,
  L_DEBUG
}LOG_LEVEL;



/*
 * MACROS
 */
#ifndef _STDARG_H
#include <stdarg.h>
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

/*
 * Functions Definitions
 */
int 	 DEBUG 	( LOG_LEVEL Level , const char *String , ...);
int		 PrintCommandLine	( const char *String , va_list arg );
int		 PrintLogFile		( const char *String , va_list arg );
int		 DebugError			( const char *String , va_list arg );
int		 DebugWarning		( const char *String , va_list arg );
char 	*ConCat	( char *StringOne, char *StringTwo );

/*
 * MACROS Depending on Linux or Windows
 */
#ifdef __unix__

	#ifndef _LINUX_LIMITS_H
		#include <linux/limits.h>
	#endif
	#ifndef _SYS_TYPES_H
		#include <sys/types.h>
	#endif
	#ifndef _SYS_STAT_H
		#include <sys/stat.h>
	#endif

	#define MP PATH_MAX
	#define CCWD( buf ) getcwd( buf , MP)
	#define MAKEDIR( cwd ) mkdir( ConCat ( cwd , "/SYSTEMLOGS" ) , 0777);

#elif defined(_WIN32) || defined(WIN32)

	#include <windows.h>
	#define MP MAX_PATH
	#define CCWD( buf ) _getcwd( buf , MP )
	#define MAKEDIR( cwd ) mkdir( ConCat ( cwd , "/SYSTEMLOGS" ) );

#endif



/*
 * Functions
 */
int
DEBUG(	LOG_LEVEL Level 	,
		const char *String 	,
		...					)
{
	va_list arg;
	va_start( arg , String );

	if (!Level)
		Level = L_DEFAULT;

	switch (Level){
		case L_DEFAULT:
			PrintCommandLine( String , arg );
			va_start( arg , String );
			PrintLogFile	( String , arg );
			va_end(arg);
			break;
		case L_INFO:
			DebugWarning	( String , arg );
			break;
		case L_ERROR:
			DebugError		( String , arg );
			break;
		case L_DEBUG:
			PrintLogFile	( String , arg );
			break;
		default:
			PrintCommandLine( String , arg );
			break;
	}

	va_end(arg);
	fflush( stdout );
	return 0;
}

int
PrintCommandLine( const char *String ,
				  va_list arg 		 )
{
	vprintf( String , arg );
	return 0;
}

int
InitFileLog(){
    FILE *FilePointer;
    char *FileName = "/SYSTEMLOGS/Log.txt\0";
    char *cwd;
    cwd = (char *)malloc((size_t)MP);

    CCWD    ( cwd );
    MAKEDIR ( cwd );

    FilePointer = fopen( ConCat ( cwd , FileName ) , "w");

    fclose(FilePointer);

    return 0;	
}

int
PrintLogFile	( const char *String ,
				  va_list arg	     )
{
	FILE *FilePointer;
	char *FileName = "/SYSTEMLOGS/Log.txt\0";
	char *cwd;
	cwd = (char *)malloc((size_t)MP);

	CCWD	( cwd );
	//MAKEDIR ( cwd );

	FilePointer = fopen( ConCat ( cwd , FileName ) , "a+");

	vfprintf( FilePointer ,  String , arg );

	fclose(FilePointer);

	return 0;
}

int
DebugError		( const char *String ,
				  va_list arg		 )
{
	vprintf( "_ERROR_: \n" , arg );
	PrintCommandLine( String , arg );
	vprintf( "_END_ERROR_: \n" , arg );

	PrintLogFile("_ERROR_: \n" , NULL);
	PrintLogFile( String , arg );
	PrintLogFile("_END_ERROR_: \n" , NULL);

	return 0;
}

int
DebugWarning	( const char *String ,
				  va_list arg		 )
{
	vprintf( "_WARNING_: \n" , arg );
	PrintCommandLine( String , arg );
	vprintf( "_END_WARNING_\n" , arg );

	PrintLogFile("_WARNING_: \n" , NULL);
	PrintLogFile( String , arg );
	PrintLogFile("_END_WARNING_\n" , NULL);

	return 0;
}

char *
ConCat			( char *StringOne 	 ,
				  char *StringTwo 	 )
{
    char *result;

    result = malloc( strlen( StringOne ) + strlen( StringTwo ) + 1 );

    strcpy( result , StringOne  );
    strcat( result , StringTwo );

    return result;
}
#endif
