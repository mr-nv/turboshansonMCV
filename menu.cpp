#include "menu.h"

unsigned long menu::smallfont;
unsigned long menu::font;
std::vector<menu::Element*> menu::Element::elements;
int menu::Element::input_currentelement = 0;
menu::Element* menu::body;
menu::Input* menu::input;
Vars_s* vars;

void menu::AddCheckbox( const char* name, bool* toggle )
{
	auto checkbox = new menu::Checkbox( );
	checkbox->SetName( name );
	checkbox->SetToggle( toggle );
	checkbox->Add( body );
}

void menu::AddSlider( const char* name, int* variable, int min, int max, int add )
{
	auto slider = new menu::Slider( );
	slider->SetName( name );
	slider->SetVariable( variable );
	slider->SetMinMax( min, max );
	slider->SetAdd( add );
	slider->Add( body );
}

void menu::AddKey( const char* name, int* variable, int defaultkey )
{
	auto key = new menu::Key( );
	key->SetName( name );
	key->SetVariable( variable );
	key->SetKey( defaultkey );
	key->Add( body );
}

void menu::AddButton( const char* name, std::function<void( )> func )
{
	auto button = new menu::Button( );
	button->SetName( name );
	button->SetFunction( func );
	button->Add( body );
}

void menu::Init( )
{
	body = new menu::Element( );
	body->SetPos( 200, 150 );

	// --- ELEMENTS --- //
	AddCheckbox( "ESP box", &vars->esp_box );
	AddCheckbox( "ESP health", &vars->esp_health );
	AddCheckbox( "ESP team", &vars->esp_team );
	/*
	bool aim = true;
	bool aim_team = false;
	int aim_smooth = 1;
	*/
	AddCheckbox( "Aimbot", &vars->aim );
	AddSlider( "Aim FOV", &vars->aim_fov, 1, 180 );
	AddSlider( "Aim smooth", &vars->aim_smooth, 0, 100 );
	AddCheckbox( "Aim on team", &vars->aim_team );
	AddCheckbox( "Draw aim FOV", &vars->aim_drawfov );
	AddCheckbox( "Bunny hop", &vars->bunnyhop );
	AddCheckbox( "Spectator list", &vars->spectators );
	/*AddCheckbox( "Backtrack", &vars->backtrack );
	AddSlider( "Ticks removal", &vars->backtrack_ticks, 2, 10 );
	AddCheckbox( "Draw backtrack", &vars->backtrack_draw );
	AddSlider( "      Red", &vars->backtrack_draw_r, 0, 255, 5 );
	AddSlider( "      Green", &vars->backtrack_draw_g, 0, 255, 5 );
	AddSlider( "      Blue", &vars->backtrack_draw_b, 0, 255, 5 );
	AddCheckbox( "Backtrack visible only", &vars->backtrack_visible );
	AddCheckbox( "Antiscreen", &vars->antiscreen );
	AddCheckbox( "Bunny hop", &vars->bunnyhop );
	AddButton( "Config save", [ ]( ) { config->Save( ); } );
	AddButton( "Config load", [ ]( ) { config->Load( ); } );*/
	//AddKey("test", &vars->test, 32);

	input = new menu::Input( );
}

void menu::Draw( )
{
	if( !menu::font )
	{
		font = surface->CreateFont_( );
		surface->SetFontGlyphSet( font, XOR( "Verdana" ), 15, 0, 0, 0, /*FontFlags_t::FONTFLAG_OUTLINE | */FontFlags_t::FONTFLAG_ANTIALIAS | FontFlags_t::FONTFLAG_DROPSHADOW );
	}

	if( !menu::smallfont )
	{
		smallfont = surface->CreateFont_( );
		surface->SetFontGlyphSet( smallfont, XOR( "Verdana" ), 12, 0, 0, 0, /*FontFlags_t::FONTFLAG_OUTLINE | */FontFlags_t::FONTFLAG_ANTIALIAS | FontFlags_t::FONTFLAG_DROPSHADOW );
	}

	static bool draw = false;
	static DWORD tk = 0;
	if( IsKeyDown( VK_INSERT ) && ( GetTickCount( ) > tk ) )
	{
		draw = !draw;
		tk = GetTickCount( ) + 220;
	}

	if( !draw ) return;

	input->Think( );

	for( int i = 0; i < body->elements.size( ); i++ )
	{
		body->elements[ i ]->Think( );
		body->elements[ i ]->Draw( );
	}
}

void menu::Rect( int x0, int y0, int x1, int y1, int r, int g, int b, int a )
{
	surface->DrawSetColor( r, g, b, a );
	surface->DrawOutlinedRect( x0, y0, x1, y1 );
}

void menu::FilledRect( int x0, int y0, int x1, int y1, int r, int g, int b, int a )
{
	surface->DrawSetColor( r, g, b, a );
	surface->DrawFilledRect( x0, y0, x1, y1 );
}

void menu::Text( int x, int y, int r, int g, int b, int a, int alignment, bool lowersize, const char* msg, ... )
{
	unsigned long curfont = 0;

	va_list va_alist;
	char buf[ 1024 ];
	va_start( va_alist, msg );
	vsnprintf_s( buf, sizeof( buf ), msg, va_alist );
	va_end( va_alist );
	wchar_t wbuf[ 1024 ];
	MultiByteToWideChar( CP_UTF8, 0, buf, 1024, wbuf, 1024 );

	if( lowersize ) curfont = ( strlen( msg ) >= 7 ) ? smallfont : font;
	else curfont = font;

	int width, height;
	surface->GetTextSize( curfont, wbuf, width, height );

	if( alignment == 1 )
		x -= width / 2;

	surface->DrawSetTextFont( curfont );
	surface->DrawSetTextColor( r, g, b, a );
	surface->DrawSetTextPos( x, y - height / 2 );
	surface->DrawPrintText( wbuf, wcslen( wbuf ), 0 );
}

bool menu::IsKeyDown( int key )
{
	static HWND valvewindow = 0;
	while( !valvewindow )
		valvewindow = FindWindowA( XOR( "Valve001" ), 0 );

	if( GetActiveWindow( ) != valvewindow ) return false;

	return GetAsyncKeyState( key );
}

bool menu::WasKeyPressed( int key )
{
	static HWND valvewindow = 0;
	while( !valvewindow )
		valvewindow = FindWindowA( XOR( "Valve001" ), 0 );

	if( GetActiveWindow( ) != valvewindow ) return false;

	return keystate[ key ] && !oldkeystate[ key ];
}