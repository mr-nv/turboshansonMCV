#include "main.h"
#include "hooks.h"

void* client;
IVEngineClient* engine;
CGlobalVarsBase* globals;
IClientEntityList* entitylist;
void* clientmode;
ICVar* cvar;
IVDebugOverlay* debugoverlay;
ISurface* surface;
IPanel* panel;
IEngineTrace* enginetrace;

CBaseEntity* me;

#if defined _DEBUG or defined DEBUGMODE
void Print( const char* funcname, const char* text, ... )
{
	char converted[ 1024 ];
	va_list list;
	va_start( list, text );
	vsnprintf( converted, sizeof( converted ), text, list );
	va_end( list );

	printf( "[%s] %s\n", funcname, converted );
}
#endif

void Start( )
{
	while( !GetModuleHandleA( XOR( "serverbrowser.dll" ) ) )
		Sleep( 1000 );

#if defined _DEBUG or defined DEBUGMODE
	util::CreateConsole( XOR( "turboshanson debug" ) );
#endif

	engine = util::GetInterface< IVEngineClient* >( XOR( "engine.dll" ), XOR( "VEngineClient" ) );
	client = util::GetInterface< void* >( XOR( "client.dll" ), XOR( "VClient" ) );
	surface = util::GetInterface< ISurface* >( XOR( "vguimatsurface.dll" ), XOR( "VGUI_Surface031" ), false );
	entitylist = util::GetInterface< IClientEntityList* >( XOR( "client.dll" ), XOR( "VClientEntityList" ) );
	cvar = util::GetInterface< ICVar* >( XOR( "vstdlib.dll" ), XOR( "VEngineCvar" ) );
	panel = util::GetInterface< IPanel* >( XOR( "vgui2.dll" ), XOR( "VGUI_Panel" ) );
	enginetrace = util::GetInterface< IEngineTrace* >( XOR( "engine.dll" ), XOR( "EngineTraceClient" ) );
	debugoverlay = util::GetInterface< IVDebugOverlay* >( XOR( "engine.dll" ), XOR( "VDebugOverlay" ) );

	netvars::Start( );

	// globals
	globals = **( CGlobalVarsBase*** )( util::FindPattern( XOR( "client.dll" ), XOR( "\xA1????\xF3\x0F\x10\x40?\x0F\x5A\xC0\xF2\x0F\x11\x04\x24" ) ) + 1 );
	if( !globals )
	{
		speak( XOR( "No globals" ) );
		return;
	}

	speak( XOR( "globals -> 0x%X" ), ( DWORD )globals );

	clientmode = **( void*** )( util::FindPattern( XOR( "client.dll" ), XOR( "\x8B\x0D????\x81\xEC????\x8B\x01\x56\xFF\x75\x08" ) ) + 2 );
	if( !clientmode )
	{
		speak( XOR( "No clientmode" ) );
		return;
	}

	speak( XOR( "clientmode -> 0x%X" ), ( DWORD )clientmode );

	vars = new Vars_s( );

	clientmodehook = new CVMTHookManager( clientmode );
	clientmodehook->Hook( CreateMove, Indexes::CREATEMOVE );

	clienthook = new CVMTHookManager( client );
	clienthook->Hook( FrameStageNotify, Indexes::FRAMESTAGENOTIFY );

	panelhook = new CVMTHookManager( panel );
	panelhook->Hook( PaintTraverse, Indexes::PAINTTRAVERSE );

	speak( XOR( "Done" ) );
}

BOOL WINAPI DllMain( HINSTANCE mod, DWORD reason, void* reserved )
{
	if( reason == DLL_PROCESS_ATTACH )
	{
		auto thread = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )Start, 0, 0, 0 );
		if( thread )
		{
			CloseHandle( ( HANDLE )thread );
			return TRUE;
		}
	}

	return FALSE;
}