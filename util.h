#pragma once
#include "main.h"

namespace util
{
	template< typename T >
	inline T GetInterface( const char* dll, const char* name, bool bruteforce = true )
	{
		const auto mod = GetModuleHandleA( dll );
		if( !mod )
		{
			char text[ 1024 ];
			sprintf_s( text, "Failed to find module %s", dll );
			MessageBoxA( 0, text, ( "turboshanson" ), MB_OK );
			return nullptr;
		}

		typedef void* ( *CreateInterfaceFn )( const char*, unsigned long );
		CreateInterfaceFn CreateInterface = ( CreateInterfaceFn )GetProcAddress( mod, ( "CreateInterface" ) );
		if( !CreateInterface )
		{
			char text[ 1024 ];
			sprintf_s( text, "Failed to find CreateInterface in module %s", dll );
			MessageBoxA( 0, text, ( "turboshanson" ), MB_OK );
			return nullptr;
		}

		if( bruteforce )
		{
			for( int i = 99; i > 0; i-- )
			{
				char chars[ 128 ];
				sprintf_s( chars, "%s%03i", name, i );

				const auto ret = CreateInterface( chars, 0 );
				if( ret )
				{
					speak( "Found %s%03i", name, i );
					return ( T )ret;
				}
			}
		}
		else
		{
			const auto ret = CreateInterface( name, 0 );
			if( ret )
				return ( T )ret;
		}

		char text[ 1024 ];
		sprintf_s( text, "Failed to find %s interface in module %s", name, dll );
		MessageBoxA( 0, text, ( "turboshanson" ), MB_OK );

		return nullptr;
	}

	inline DWORD FollowRelative( DWORD ptr )
	{
		return ptr + *( DWORD* )( ptr + 1 ) + 5;
	}

	inline int GetRandomInt( int min, int max )
	{
		typedef int( __cdecl* GetRandomIntFn )( int, int );
		static auto function = ( GetRandomIntFn )GetProcAddress( GetModuleHandleW( ( L"vstdlib.dll" ) ), ( "RandomInt" ) );

		return function( min, max );
	}

#if defined _DEBUG or defined DEBUGMODE
	void CreateConsole( std::string title );
#endif

	// thenk u nanocat
	DWORD FindPattern( const char* dll, const char* pattern );
	DWORD FindPattern( DWORD start, const char* pattern );
	DWORD FindPattern( DWORD start, DWORD size, const char* pattern );
	bool CompareData( const char* base, const char* pattern );
}