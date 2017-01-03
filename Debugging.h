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
  L_DEBUG,
}LOG_LEVEL;

struct Global{
	int 	PathInitialized;
	char* 	_BasePath;
}Gbl ={0};

/*
 * MACROS
 */
#ifndef _STDARG_H
#include <stdarg.h>
#endif

#ifndef _STDIO_H
#include <stdio.h>
#endif

#ifndef _STDLIB_H
#include <stdlib.h>
#endif

#ifndef _UNISTD_H
#include <unistd.h>
#endif

#ifndef _TIME_H
#include <time.h>
#endif

/*
 * Functions Definitions
 */
int 	 DEBUG 					( LOG_LEVEL Level , const char *String , ...);
int		 FileExists				(											);
int		 InitFileLog			(											);
int		 PrintCommandLine		( const char *String , va_list arg 			);
int		 PrintLogFile			( const char *String , va_list arg 			);
int		 DebugError				( const char *String , va_list arg 			);
int		 DebugWarning			( const char *String , va_list arg 			);
int		 DefineDebugStatus		( int CreateDebug							);
int		 InitializeGlobals		(											);

char*	ConCat					( char *StringOne, char *StringTwo 	);
char*	CreateDateString		(									);
char*	IntToStr				( int Num , char *Buffer 			);

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

#define INT_TO_STR( _NUM_ , _BUF_ ) IntToStr( _NUM_ , _BUF_ )

static int _DEBUG_IS_ACTIVE_ = 0;

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

	/*
	 * IF NULL IS PASSED TO LEVEL, THE DEFAULT IS TO DEBU9N FILE AND ON COMMAND LINE.
	 */

	if (!Level)
		Level = L_DEFAULT;

	/*
	 * DEBUG WILL ALWAYS BE INACTIVE: " _DEBUG_IS_ACTIVE_ = 0 "
	 * THIS MEANS THAT _DEBUG_IS_ACTIVE_ WILL ALWAYS CHANGE LEVEL TO PRINT IN COMMAND LINE ONLY
	 * IF THIS MACRO CHANGED TO 1 THE PROCEES WILL BE AS NORMAL.
	 * THIS VALUE CAN BE CHANGED WITH THE FUNCTION:
	 * 			DefineDebugStatus(...)
	 * THIS FUNCTION HAS ONLY ONE CONFIG VARIABLE BUT CAN BE USED TO ADD MORE
	 */

	switch (Level){
		case L_DEFAULT:
			PrintCommandLine( String , arg );
			if (_DEBUG_IS_ACTIVE_){
				va_start( arg , String );
				PrintLogFile( String , arg );
				va_end(arg);
			}
			break;
		case L_INFO:
			DebugWarning	( String , arg );
			break;
		case L_ERROR:
			DebugError		( String , arg );
			break;
		case L_DEBUG:
			if (_DEBUG_IS_ACTIVE_){
				PrintLogFile( String , arg );
			}
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
FileExists( char *BasePath ){
	 FILE *FilePointer;
	 char *cwd;
	 cwd = (char *)malloc((size_t)MP);

	 CCWD    ( cwd );

	 FilePointer = fopen( ConCat ( cwd , BasePath ) , "a+");
	 
	 if (FilePointer){
		 fclose(FilePointer);
		 return 1;
	 }
	 
	 return 0;
}

int
InitFileLog( char *BasePath ){
    FILE *FilePointer;
    char *cwd;
    cwd = (char *)malloc((size_t)MP);

    CCWD    ( cwd );
    MAKEDIR ( cwd );
	
    FilePointer = fopen( ConCat ( cwd , BasePath ) , "w");
	
    fclose(FilePointer);

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
PrintLogFile	( const char *String ,
				  va_list arg	     )
{
	FILE *FilePointer;
	char *cwd;
	cwd = (char *)malloc((size_t)MP);

	CCWD	( cwd );

	InitializeGlobals();
	
	if ( ! FileExists( Gbl._BasePath) ){
		InitFileLog( Gbl._BasePath );
	}
	
	FilePointer = fopen( ConCat ( cwd , Gbl._BasePath ) , "a+");
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
	vprintf( "_END_ERROR_ \n\n" , arg );

	if (_DEBUG_IS_ACTIVE_){
		PrintLogFile("_ERROR_: \n" , NULL);
		PrintLogFile( String , arg );
		PrintLogFile("_END_ERROR_ \n\n" , NULL);
	}

	return 0;
}

int
DebugWarning	( const char *String ,
				  va_list arg		 )
{
	vprintf( "_WARNING_: \n" , arg );
	PrintCommandLine( String , arg );
	vprintf( "_END_WARNING_\n\n" , arg );

	if (_DEBUG_IS_ACTIVE_){
		PrintLogFile("_WARNING_: \n" , NULL);
		PrintLogFile( String , arg );
		PrintLogFile("_END_WARNING_\n\n" , NULL);
	}

	return 0;
}

int
DefineDebugStatus( int CreateDebug ){
		_DEBUG_IS_ACTIVE_ = CreateDebug;
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

char*
CreateDateString(){
	char *DateString = "";
	char *Buffer = "";
	int Year, Month, Day, Hour, Min, Sec;

	time_t CurrentTime;
	struct tm *Time;

	time( &CurrentTime );
	Time = localtime(&CurrentTime);

	Year 	= Time->tm_year + 1900  ;
	Month 	= Time->tm_mon	+ 1		;
	Day		= Time->tm_mday	;
	Hour	= Time->tm_hour	;
	Min		= Time->tm_min	;
	Sec		= Time->tm_sec	;

    /*
     * Name For the Log File.
     * ==========================================================================
     * | Year | - | Month | - | Day | _ | Hour | - | Minute | - | Second | .log |
     * |======|===|=======|===|=====|===|======|===|========|============|======|
     * |   4  | 1 |   2   | 1 |  2  | 1 |  2   | 1 |   2    | 1 |   2    |   4  |
     * ==========================================================================
     * 23 Characters
     * Need to allocate 24.
     */

	DateString  = (char *) malloc(		24		);
	Buffer 		= (char *) malloc( sizeof(char) );
	*Buffer		= '\0';
	*DateString	= '\0';

	INT_TO_STR( Year, Buffer );
	DateString = ConCat( DateString , Buffer );
	DateString = ConCat( DateString , "-" );

	INT_TO_STR( Month, Buffer );
	DateString = ConCat( DateString , Buffer );
	DateString = ConCat( DateString , "-" );

	INT_TO_STR( Day, Buffer );
	DateString = ConCat( DateString , Buffer );
	DateString = ConCat( DateString , "_" );

	INT_TO_STR( Hour, Buffer );
	DateString = ConCat( DateString , Buffer );
	DateString = ConCat( DateString , "-" );

	INT_TO_STR( Min, Buffer );
	DateString = ConCat( DateString , Buffer );
	DateString = ConCat( DateString , "-" );

	INT_TO_STR( Sec, Buffer );
	DateString = ConCat( DateString , Buffer );
	DateString = ConCat( DateString , ".log" );
	
	return DateString;
}

int
InitializeGlobals(){
	if ( !Gbl.PathInitialized ){
		Gbl._BasePath 		= ConCat( "/SYSTEMLOGS/" , CreateDateString() );
		Gbl.PathInitialized = 1;
	}
	return 0;
}

char *
IntToStr( int Num , char *Buffer ){
	int  i , x , y;
	int  TempNum = Num;
	char TempChar;

	*Buffer		= '\0';
	i = 0;

	if ( Num == 0 ){
		Buffer[i]   = '0';
		Buffer[i+1] = '\0';
		return Buffer;
	}

	while ( TempNum != 0 ){
		int C = TempNum % 10;
		Buffer[i++] = C + '0';
		TempNum = TempNum / 10;
	}

	if( Num < 10){
		Buffer[i++]   = '0';
	}

	Buffer[i] = '\0';

	for (x = 0, y = i - 1; x < y ; x++, y--){
		TempChar = *( Buffer + x );
		*(Buffer + x ) = *(Buffer + y );
		*(Buffer + y ) = TempChar;
	}

	return Buffer;
}
#endif
