#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <array>

#pragma warning(disable : 4244 4018)

//#define DEBUGMODE

#if defined _DEBUG or defined DEBUGMODE
extern void Print( const char* funcname, const char* text, ... );
#define speak( ... ) Print( __FUNCTION__, __VA_ARGS__ )
#else
#define speak( ... )
#endif

extern void* client;

#include "md5.h"
#include "dt_recv.h"
#include "sdk.h"
#include "vmt.h"
#include "XorStr.h"
#include "netvars.h"
#include "entity.h"
#include "util.h"

template <typename T = void*>
constexpr T GetVFuncAddress( void* thisptr, std::size_t index )
{
	return ( *static_cast< T** >( thisptr ) )[ index ];
}

extern CBaseEntity* me;

#include "menu.h"

#define PI 3.14159265358979323846f
#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PI ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PI ) ) )
#define square( x ) ( x * x )
#define CHECK_VALID( _v ) 0
#define Assert( _exp ) ((void)0)

#define	add(dst, flag)	((dst) |= (flag))
#define	del(dst, flag)	((dst) &=~(flag))
#define	not(dst, flag)	((dst) & ~(flag))
#define	chk(dst, flag)	((dst) &  (flag))

#include "draw.h"
#include "esp.h"