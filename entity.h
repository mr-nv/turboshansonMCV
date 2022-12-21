#pragma once
#include "main.h"

#define DEFINESTR( x ) #x

#if defined _DEBUG or defined DEBUGMODE
#define NETVAR( type, table, prop ) type& prop( ) \
	{ \
		static auto offset = GetOffset( HASH( DEFINESTR( table ) ), HASH( DEFINESTR( prop ) ) ); \
		if( !offset ) \
		{ \
			char text[ 1024 ]; \
			sprintf_s( text, XOR( "Failed to find netvar %s / %s" ), DEFINESTR( table ), DEFINESTR( prop ) ); \
			MessageBoxA( 0, text, XOR( "netvars" ), MB_OK ); \
		} \
		return *( type* )( ( DWORD )this + offset ); \
	}
#else
#define NETVAR( type, table, prop ) type& prop( ) \
	{ \
		static auto offset = GetOffset( HASH( DEFINESTR( table ) ), HASH( DEFINESTR( prop ) ) ); \
		return *( type* )( ( DWORD )this + offset ); \
	}
#endif

class CBaseEntity
{
public:
	VFUNC( 10, Vector&, GetAbsOrigin, ( this ) );
	VFUNC( 11, Vector&, GetAbsAngles, ( this ) );

	DWORD GetOffset( hash32_t table, hash32_t prop );

	void* GetRenderable( )
	{
		return ( void* )( ( DWORD )this + 0x4 );
	}

	void* GetNetworkable( )
	{
		return ( void* )( ( DWORD )this + 0x8 );
	}

	bool IsDormant( )
	{
		auto network = GetNetworkable( );
		if( !network ) return false;

		return GetVFunc<bool( __thiscall* )( void* )>( network, 9 )( network );
	}

	bool SetupBones( matrix3x4_t* bone, int max, int mask, float curtime )
	{
		auto renderable = ( void* )( ( DWORD )this + 0x4 );
		if( !renderable ) return false;

		return GetVFunc<bool( __thiscall* )( void*, matrix3x4_t*, int, int, float )>( renderable, 13 )( renderable, bone, max, mask, curtime );
	}

	Vector GetBonePosition( int bone, float curtime )
	{
		static Vector nullvec( 0.f, 0.f, 0.f );
		matrix3x4_t matrix[ 128 ];

		if( !SetupBones( matrix, 128, 0x100, curtime ) ) return nullvec; // 0x100 - BONE_USED_BY_HITBOX

		return Vector( matrix[ bone ][ 0 ][ 3 ], matrix[ bone ][ 1 ][ 3 ], matrix[ bone ][ 2 ][ 3 ] );
	}

	NETVAR( unsigned char, DT_Vietnam_Player, m_lifeState );
	NETVAR( int, DT_Vietnam_Player, m_iHealth );
	NETVAR( int, DT_Vietnam_Player, m_fFlags );
	NETVAR( int, DT_Vietnam_Player, m_iTeamNum );
	NETVAR( Vector, DT_Vietnam_Player, m_vecOrigin );
	NETVAR( Vector, DT_Vietnam_Player, m_vecMins );
	NETVAR( Vector, DT_Vietnam_Player, m_vecMaxs );

	Vector GetEyePosition( )
	{
		auto origin = m_vecOrigin( );

		static auto m_vecViewOffset = GetOffset( HASH( "DT_Vietnam_Player" ), HASH( "m_vecViewOffset[0]" ) );
		auto viewoffset = *( Vector* )( this + m_vecViewOffset );

		return origin + viewoffset;
	}

	CBaseEntity* m_hObserverTarget( )
	{
		static auto offset = GetOffset( HASH( "DT_Vietnam_Player" ), HASH( "m_hObserverTarget" ) );

		void* shit = *( void** )( this + offset );
		if( !shit || shit == INVALID_HANDLE_VALUE )
			return nullptr;

		return entitylist->GetClientEntityFromHandle( shit );
	}
};