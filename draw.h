#pragma once
#include "main.h"

namespace draw
{
	// fonts
	extern unsigned long espfont;
	extern unsigned long watermarkfont;
	extern unsigned long miscfont;
	extern unsigned long infofont;

	wchar_t MultiByteToWide( const std::string& str );
	void BuildFonts( );

	void Rect( int x, int y, int w, int h, Color color );
	void FilledRect( int x, int y, int w, int h, Color color );
	void Line( int x1, int y1, int x2, int y2, Color color );
	void Circle( int x, int y, float radius, int segments, Color color );
	bool WorldToScreen( const Vector in, Vector& out );

	// alignment 1 - center, alignment 2 - right, 3 - center x/y
	void Text( unsigned long font, int x, int y, Color color, int alignment, const char* text );
}