#define _CRT_SECURE_NO_WARNINGS
#include "util.h"

#if defined _DEBUG or defined DEBUGMODE
void util::CreateConsole( std::string title )
{
	AllocConsole( );
	freopen( "CONOUT$", "wt", stdout );
	SetConsoleTitleA( title.c_str( ) );
	SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED );
}
#endif

DWORD util::FindPattern( const char* dll, const char* pattern )
{
	return FindPattern( ( DWORD )( GetModuleHandleA( dll ) ), -1, pattern );
}

DWORD util::FindPattern( DWORD start, const char* pattern )
{
	return FindPattern( start, -1, pattern );
}

DWORD util::FindPattern( DWORD start, DWORD size, const char* pattern )
{
	for( DWORD i = 0; i < size; i++, start++ )
	{
		if( CompareData( ( const char* )start, pattern ) )
			return start;
	}

	return 0;
}

bool util::CompareData( const char* base, const char* pattern )
{
	for( ; *pattern; base++, pattern++ )
	{
		if( *pattern != '?' && *base != *pattern )
			return 0;
	}

	return *pattern == 0;
}