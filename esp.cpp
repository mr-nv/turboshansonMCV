#include "esp.h"

namespace esp
{
	struct Box
	{
		int x, y, w, h;
	};

	bool Valid( CBaseEntity* entity )
	{
		return
			entity &&
			//!entity->IsDormant( ) &&
			entity->m_lifeState( ) == 0; // LIFE_ALIVE
	}

	void Draw( )
	{
		if( !vars->esp_box && !vars->esp_health && !vars->esp_name ) return;

		int screenwidth, screenheight;
		engine->GetScreenSize( screenwidth, screenheight );

		for( int i = 1; i <= globals->maxClients; i++ )
		{
			auto entity = entitylist->GetClientEntity( i );
			if( !entity || entity == me || !Valid( entity ) ) continue;

			player_info_t info;
			if( !engine->GetPlayerInfo( i, &info ) ) continue;
			if( !vars->esp_team && entity->m_iTeamNum( ) == me->m_iTeamNum( ) ) continue;

			auto origin = entity->GetAbsOrigin( ), mins = entity->m_vecMins( ), maxs = entity->m_vecMaxs( );
			Vector unused;
			if( !draw::WorldToScreen( origin, unused ) ) continue;

			mins += origin;
			maxs += origin;

			int alpha = 255;
			if( entity->IsDormant( ) )
				alpha = 75;

			std::array< Vector, 8 > corners =
			{
				Vector( mins.x, mins.y, mins.z ),
				Vector( mins.x, mins.y, maxs.z ),
				Vector( mins.x, maxs.y, mins.z ),
				Vector( mins.x, maxs.y, maxs.z ),
				Vector( maxs.x, mins.y, mins.z ),
				Vector( maxs.x, mins.y, maxs.z ),
				Vector( maxs.x, maxs.y, mins.z ),
				Vector( maxs.x, maxs.y, maxs.z )
			};

			Box box;
			box.x = screenwidth * 2;
			box.y = screenheight * 2;
			box.w = box.h = -1;

			for( const auto& corner : corners )
			{
				Vector screen;
				if( !draw::WorldToScreen( corner, screen ) ) continue;

				box.x = roundf( min( box.x, screen.x ) );
				box.y = roundf( min( box.y, screen.y ) );
				box.w = roundf( max( box.w, screen.x ) );
				box.h = roundf( max( box.h, screen.y ) );
			}

			box.w -= box.x;
			box.h -= box.y;

			if( vars->esp_box )
			{
				draw::Rect( box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color( 0, 0, 0, alpha ) );
				draw::Rect( box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color( 0, 0, 0, alpha ) );
				draw::Rect( box.x, box.y, box.w, box.h, Color( 255, 255, 255, alpha ) );
			}

			if( vars->esp_name )
			{
				std::string name = info.name;
				if( name.length( ) > 14 )
				{
					name = name.substr( 0, 14 );
					name = name.append( XOR( "..." ) );
				}

				int fonth, fontw;
				wchar_t wstr[ 1024 ] = { '\0' };
				MultiByteToWideChar( CP_UTF8, 0, name.c_str( ), 1024, wstr, 1024 );
				surface->GetTextSize( draw::espfont, wstr, fontw, fonth );

				auto midx = ( box.x + box.w ) - ( box.w / 2 );
				auto y = box.y - fonth - 2;

				surface->DrawSetTextColor( 255, 255, 255, alpha );
				surface->DrawSetTextFont( draw::espfont );
				surface->DrawSetTextPos( midx - ( fontw / 2 ), y );
				surface->DrawPrintText( wstr, wcslen( wstr ), 0 );
			}

			if( vars->esp_health )
			{
				auto health = entity->m_iHealth( );
				auto health_x = box.x - 4;
				auto health_y = box.y;
				auto health_h = std::clamp( health * box.h / 100, 0, box.h );

				// outline
				surface->DrawSetColor( 0, 0, 0, 255 );
				surface->DrawFilledRect( health_x - 1, ( health_y + box.h - health_h ) - 1, health_x + 2, ( ( health_y + box.h - health_h ) + health_h ) + 1 );
				surface->DrawFilledRect( health_x + 1, ( health_y + box.h - health_h ) + 1, health_x - 1, ( ( health_y + box.h - health_h ) + health_h ) - 1 );

				surface->DrawSetColor( 255 - ( int )( health * 2.55f ), ( int )( health * 2.55f ), 0, 255 );
				surface->DrawFilledRect( health_x, health_y + box.h - health_h, health_x + 1, ( health_y + box.h - health_h ) + health_h );

				// text
				if( health < 100 )
				{
					char fmt[ 10 ];
					sprintf_s( fmt, "%d", health );

					draw::Text( draw::espfont, health_x + 1, ( health_y + box.h - health_h ), Color( 255, 255, 255, alpha ), 1, fmt );
				}
			}

			// dbg
			/*for( int j = 0; j < 12; j++ )
			{
				auto gay = entity->GetBonePosition( j, globals->curtime );
				Vector w2s;
				if( draw::WorldToScreen( gay, w2s ) )
				{
					char z[ 10 ];
					sprintf_s( z, "%d", j );
					draw::Text( draw::espfont, w2s.x, w2s.y, Color::White( ), 1, z );
				}
			}*/
		}
	}
}