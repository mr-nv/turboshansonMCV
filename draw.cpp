#include "draw.h"

namespace draw
{
	unsigned long espfont;
	unsigned long watermarkfont;
	unsigned long miscfont;
	unsigned long infofont;

	void BuildFonts( )
	{
		speak( XOR( "Building fonts" ) );

		espfont = surface->CreateFont_( );
		surface->SetFontGlyphSet( espfont, XOR( "Verdana" ), 14, 400, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS );

		watermarkfont = surface->CreateFont_( );
		surface->SetFontGlyphSet( watermarkfont, XOR( "Tahoma" ), 14, 400, 0, 0, FONTFLAG_DROPSHADOW );

		miscfont = surface->CreateFont_( );
		surface->SetFontGlyphSet( miscfont, XOR( "Verdana" ), 15, 400, 0, 0, FONTFLAG_DROPSHADOW );

		infofont = surface->CreateFont_( );
		surface->SetFontGlyphSet( infofont, XOR( "Verdana" ), 16, 600, 0, 0, FONTFLAG_OUTLINE );
	}

	void Rect( int x, int y, int w, int h, Color color )
	{
		surface->DrawSetColor( color.r( ), color.g( ), color.b( ), color.a( ) );
		surface->DrawOutlinedRect( x, y, x + w, y + h );
	}

	void FilledRect( int x, int y, int w, int h, Color color )
	{
		surface->DrawSetColor( color.r( ), color.g( ), color.b( ), color.a( ) );
		surface->DrawFilledRect( x, y, x + w, y + h );
	}

	void Line( int x1, int y1, int x2, int y2, Color color )
	{
		surface->DrawSetColor( color.r( ), color.g( ), color.b( ), color.a( ) );
		surface->DrawLine( x1, y1, x2, y2 );
	}

	void Circle( int x, int y, float radius, int segments, Color color )
	{
		surface->DrawSetColor( color.r( ), color.g( ), color.b( ), color.a( ) );

		const auto step = ( float )PI * 2.0f / segments;

		for( float a = 0; a < ( PI * 2.0f ); a += step )
		{
			const int x1 = radius * cosf( a ) + x;
			const int y1 = radius * sinf( a ) + y;
			const int x2 = radius * cosf( a + step ) + x;
			const int y2 = radius * sinf( a + step ) + y;

			surface->DrawLine( x1, y1, x2, y2 );
		}

		//surface->DrawOutlinedCircle( x, y, radius, segments );
	}

	// https://github.com/Lak3/Source-Engine-2013-base/blob/master/SourceEngine/SourceEngine/SDK/Main/DrawUtils/DrawUtils.h#L66
	bool WorldToScreen( const Vector in, Vector& out )
	{
		/*bool retval = ClipTransform( in, out );

		int w, h;
		engine->GetScreenSize( w, h );

		out.x = 0.5 * out.x * w;
		out.y = -0.5 * out.y * h;
		out.x += 0.5 * w;
		out.y += 0.5 * h;
		return retval;*/

		return debugoverlay->ScreenPosition( in, out ) != 1;
	}

	// alignment 1 - center, alignment 2 - right, 3 - center x/y
	void Text( unsigned long font, int x, int y, Color color, int alignment, const char* text )
	{
		int w, h;

		wchar_t wstr[ 1024 ] = { '\0' };
		wsprintfW( wstr, L"%hs", text );
		//auto wstr = MultiByteToWide( text );

		surface->GetTextSize( font, wstr, w, h );
		surface->DrawSetTextFont( font );
		surface->DrawSetTextColor( color.r( ), color.g( ), color.b( ), color.a( ) );

		if( alignment == 1 ) // center
			x -= ( w / 2 );
		else if( alignment == 2 )
			x -= w;
		else if( alignment == 3 )
		{
			x -= ( w / 2 );
			y -= ( h / 2 );
		}

		surface->DrawSetTextPos( x, y );
		surface->DrawPrintText( wstr, ( int )wcslen( wstr ), 0 );
	}
}