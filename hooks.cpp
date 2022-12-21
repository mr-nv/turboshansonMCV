#include "hooks.h"

CVMTHookManager* clienthook;
CVMTHookManager* clientmodehook;
CVMTHookManager* panelhook;

void __fastcall FrameStageNotify( void* ecx, void* edx, ClientFrameStage_t stage )
{
	static auto original = ( decltype( &FrameStageNotify ) )clienthook->GetMethodAddress( Indexes::FRAMESTAGENOTIFY );

	original( ecx, edx, stage );

	switch( stage )
	{
	case ClientFrameStage_t::FRAME_NET_UPDATE_START:
		me = nullptr;
		break;
	case ClientFrameStage_t::FRAME_NET_UPDATE_END:
		me = entitylist->GetClientEntity( engine->GetLocalPlayer( ) );
		break;
	default:
		break;
	}
}

bool CheckAimbotEntity( CBaseEntity* entity )
{
	// 0 = LIFE_ALIVE
	if( !entity || entity->m_lifeState( ) != 0 )
		return false;

	if( !vars->aim_team && entity->m_iTeamNum( ) == me->m_iTeamNum( ) )
		return false;

	return
		!entity->IsDormant( );
}

Vector CalcAngle( const Vector& src, const Vector& dst )
{
	Vector angles = Vector( 0.0f, 0.0f, 0.0f );
	Vector delta = ( src - dst );
	float fHyp = sqrt( ( delta.x * delta.x ) + ( delta.y * delta.y ) );

	angles.x = ( atanf( delta.z / fHyp ) * 57.295779513082f );
	angles.y = ( atanf( delta.y / delta.x ) * 57.295779513082f );
	angles.z = 0.0f;

	if( delta.x >= 0.0f )
		angles.y += 180.0f;

	return angles;
}

float NormalizeAngle( const float ang )
{
	if( !std::isfinite( ang ) )
		return 0.0f;

	return std::remainder( ang, 360.0f );
}

void MakeVector( Vector angles, Vector& out )
{
	auto pitch = float( angles[ 0 ] * PI / 180.f );
	auto yaw = float( angles[ 1 ] * PI / 180.f );
	auto tmp = float( cos( pitch ) );
	out[ 0 ] = float( -tmp * -cos( yaw ) );
	out[ 1 ] = float( sin( yaw ) * tmp );
	out[ 2 ] = float( -sin( pitch ) );
}

float GetFOV( const Vector angles, const Vector src, const Vector dst )
{
	Vector ang, aim;
	double fov;

	ang = CalcAngle( src, dst );
	MakeVector( angles, aim );
	MakeVector( ang, ang );

	auto mag_s = sqrt( ( aim[ 0 ] * aim[ 0 ] ) + ( aim[ 1 ] * aim[ 1 ] ) + ( aim[ 2 ] * aim[ 2 ] ) );
	auto mag_d = sqrt( ( aim[ 0 ] * aim[ 0 ] ) + ( aim[ 1 ] * aim[ 1 ] ) + ( aim[ 2 ] * aim[ 2 ] ) );
	auto u_dot_v = aim[ 0 ] * ang[ 0 ] + aim[ 1 ] * ang[ 1 ] + aim[ 2 ] * ang[ 2 ];

	fov = acos( u_dot_v / ( mag_s * mag_d ) ) * ( 180.f / PI );

	return ( float )fov;
}

void DrawFOV( )
{
	const auto radius = ( int )( ( float )vars->aim_fov * 10.7f );

	int screenwidth, screenheight;
	engine->GetScreenSize( screenwidth, screenheight );

	draw::Circle( screenwidth / 2, screenheight / 2, radius, 50, Color::White( ) );
}

bool IsVisible( Vector start, Vector end, CBaseEntity* entity )
{
	Ray_t ray;
	CGameTrace trace;
	CTraceFilter filter;
	filter.pSkip = me;

	ray.Init( start, end );
	enginetrace->TraceRay( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &trace );

	return trace.entity == entity || trace.fraction >= 0.97f;
}

void Aimbot( CUserCmd* cmd )
{
	static CBaseEntity* lasttarget = nullptr;

	if( lasttarget )
	{
		// if we have a target recheck it and if its good continue aiming on it
		bool found = false;

		// check if this entity still exists in the game
		for( int i = 1; i <= globals->maxClients; i++ )
		{
			auto entity = entitylist->GetClientEntity( i );
			if( entity == lasttarget )
			{
				found = true;
				break;
			}
		}

		if( !found )
			lasttarget = nullptr;
		else
		{
			if( CheckAimbotEntity( lasttarget ) && IsVisible( me->GetEyePosition( ), lasttarget->GetEyePosition( ), lasttarget ) )
			{
				// head hitbox is 6
				auto hitbox = lasttarget->GetBonePosition( 6, globals->curtime );
				auto eye = me->GetEyePosition( );

				auto calc = CalcAngle( eye, hitbox );

				calc.x = NormalizeAngle( calc.x );
				calc.y = NormalizeAngle( calc.y );
				calc.z = cmd->viewangles.z;

				Vector angle = calc;

				// smoothing
				if( vars->aim_smooth > 0 )
				{
					const float smoothvalue = ( float )vars->aim_smooth;

					Vector copy = calc - cmd->viewangles;

					// normalize
					copy.x = NormalizeAngle( copy.x );
					copy.y = NormalizeAngle( copy.y );
					copy.z = 0.f;

					copy.x = copy.x / smoothvalue + cmd->viewangles.x;
					copy.y = copy.y / smoothvalue + cmd->viewangles.y;

					// normalize again
					angle.x = NormalizeAngle( copy.x );
					angle.y = NormalizeAngle( copy.y );
					angle.z = cmd->viewangles.z;
				}

				// set angles
				cmd->viewangles = angle;
				engine->SetViewAngles( angle );
			}
			else
				lasttarget = nullptr;
		}
	}
	else
	{
		// we need to find a target
		float lastfov = FLT_MAX;
		
		for( int i = 1; i <= globals->maxClients; i++ )
		{
			auto entity = entitylist->GetClientEntity( i );
			if( !entity || entity == me || !CheckAimbotEntity( entity ) ) continue;

			// visibility check
			if( !IsVisible( me->GetEyePosition( ), entity->GetEyePosition( ), entity ) )
				continue;

			// sort out the closest entity to our crosshair
			const float fov = GetFOV( cmd->viewangles, me->GetEyePosition( ), entity->GetEyePosition( ) );
			if( fov < ( float )vars->aim_fov && fov < lastfov )
			{
				lastfov = fov;
				lasttarget = entity;
			}
		}

		// if we have found a target then the cheat will aim at it
		// the next time createmove gets called
	}
}

bool __fastcall CreateMove( void* ecx, void* edx, float frametime, CUserCmd* cmd )
{
	static auto original = ( decltype( &CreateMove ) )clientmodehook->GetMethodAddress( Indexes::CREATEMOVE );

	if( !cmd || !cmd->command_number )
		return original( ecx, edx, frametime, cmd );

	original( ecx, edx, frametime, cmd );

	if( me && me->m_lifeState( ) == 0 ) // 0 = LIFE_ALIVE
	{
		if( vars->bunnyhop )
		{
			if( chk( cmd->buttons, IN_JUMP ) && !chk( me->m_fFlags( ), FL_ONGROUND ) )
				del( cmd->buttons, IN_JUMP );
		}

		if( chk( cmd->buttons, IN_ATTACK ) || GetAsyncKeyState( VK_RBUTTON ) )
		{
			if( vars->aim )
				Aimbot( cmd );
		}
	}

	return false;
}

void __fastcall PaintTraverse( void* ecx, void* edx, unsigned int panelid, bool repaint, bool force )
{
	static auto original = ( decltype( &PaintTraverse ) )panelhook->GetMethodAddress( Indexes::PAINTTRAVERSE );

	original( ecx, edx, panelid, repaint, force );

	static unsigned int FocusOverlayPanel = 0;
	if( !FocusOverlayPanel )
	{
		auto panelname = panel->GetName( panelid );
		if( panelname )
		{
			if( CONST_HASH( panelname ) == HASH( "FocusOverlayPanel" ) )
			{
				speak( XOR( "Found FocusOverlayPanel" ) );
				FocusOverlayPanel = panelid;
			}
		}
	}

	if( panelid != FocusOverlayPanel )
		return;

	static bool flip = false;
	flip = !flip;

	if( flip ) return;

	static bool init = false;
	if( !init )
	{
		init = true;

		draw::BuildFonts( );
		menu::Init( );
	}

	static int sw, sh, sw2, sh2;
	engine->GetScreenSize( sw, sh );
	if( sw != sw2 || sh != sh2 )
	{
		menu::font = 0;
		menu::smallfont = 0;
		sw2 = sw;
		sh2 = sh;
	}

	// watermark
	draw::Text( draw::watermarkfont, 5, 5, Color::White( ), 0, XOR( "turboshanson for Military Conflict: Vietnam (" __DATE__ ")" ) );

	if( me && engine->IsInGame( ) && engine->IsConnected( ) )
	{
		esp::Draw( );

		if( vars->aim_drawfov )
			DrawFOV( );

		// spectator list
		if( vars->spectators )
		{
			static std::vector< std::string > spectators;
			spectators.clear( );

			for( int i = 1; i <= engine->GetMaxClients( ); i++ )
			{
				auto entity = entitylist->GetClientEntity( i );
				if( !entity || entity->IsDormant( ) || entity == me ) continue;

				if( entity->m_lifeState( ) != 0 ) // LIFE_ALIVE
				{
					player_info_t info;
					if( engine->GetPlayerInfo( i, &info ) )
					{
						if( entity->m_hObserverTarget( ) )
						{
							if( entity->m_hObserverTarget( ) == me )
								spectators.push_back( info.name );
						}
					}
				}
			}

			if( spectators.size( ) > 0 )
			{
				char fmt[ 128 ];
				sprintf_s( fmt, XOR( "SPECTATING YOU (%d)" ), spectators.size( ) );
				
				int screenwidth, screenheight;
				engine->GetScreenSize( screenwidth, screenheight );

				draw::Text( draw::espfont, screenwidth / 2, 75, Color( 255, 178, 0, 255 ), 1, fmt );

				int x = 95;

				for( auto name : spectators )
				{
					wchar_t wstr[ 1024 ] = { '\0' };
					MultiByteToWideChar( CP_UTF8, 0, name.c_str( ), 1024, wstr, 1024 );
					int textw, texth;
					surface->GetTextSize( draw::espfont, wstr, textw, texth );

					surface->DrawSetTextFont( draw::espfont );
					surface->DrawSetTextColor( 255, 255, 255, 255 );
					surface->DrawSetTextPos( screenwidth / 2 - ( textw / 2 ), x );
					surface->DrawPrintText( wstr, wcslen( wstr ), 0 );

					x += 18;
				}
			}
		}
	}

	menu::Draw( );
}