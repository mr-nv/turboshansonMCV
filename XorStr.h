#pragma once
#include "main.h"

// todo - dex; check arg type for macros?

// hash type.
using hash32_t = uint32_t;

// compile-time hash.
#define HASH( str )                                     \
    []() {                                              \
        constexpr hash32_t out{ FNV1a::get( str ) };    \
                                                        \
        return out;                                     \
    }()

// compile-time const hash.
#define CONST_HASH( str ) FNV1a::get( str )

class FNV1a
{
private:
    // fnv1a constants.
    enum : uint32_t
    {
        PRIME = 0x1000193u,
        BASIS = 0x811C9DC5u
    };

    // compile-time strlen.
    // todo - dex; move this?
    static __forceinline constexpr size_t ct_strlen( const char* str, bool include_nullchar = false )
    {
        size_t out{};

        // count number of characters.
        while( str[ ++out ] != '\0' );

        // add one for null character if desired.
        if( include_nullchar )
            ++out;

        return out;
    }

public:
    // hash data ( compile-time or run-time ).
    static __forceinline constexpr hash32_t get( const uint8_t* data, const size_t len )
    {
        hash32_t out{ BASIS };

        for( size_t i{}; i < len; ++i )
            out = ( out ^ data[ i ] ) * PRIME;

        return out;
    }

    // hash c-style string ( compile-time or run-time ).
    static __forceinline constexpr hash32_t get( const char* str )
    {
        hash32_t out{ BASIS };
        size_t   len{ ct_strlen( str ) };

        for( size_t i{}; i < len; ++i )
            out = ( out ^ str[ i ] ) * PRIME;

        return out;
    }

    // hash c++-style string ( run-time only ).
    static __forceinline hash32_t get( const std::string& str )
    {
        return get( ( uint8_t* )str.c_str( ), str.size( ) );
    }
};

// warning C4172: returning address of local variable or temporary
// potential pointer dangling? seems fine for now.
#pragma warning( disable : 4172 )  

// generate 'pseudo-random' xor key based on file, date and line.
#define GET_XOR_KEYUI8  ( ( FNV1a::get( __FILE__ __TIMESTAMP__ ) + __LINE__ ) % UINT8_MAX )
#define GET_XOR_KEYUI16 ( ( FNV1a::get( __FILE__ __TIMESTAMP__ ) + __LINE__ ) % UINT16_MAX )
#define GET_XOR_KEYUI32 ( ( FNV1a::get( __FILE__ __TIMESTAMP__ ) + __LINE__ ) % UINT32_MAX )

namespace xorn
{
	// actual xor implementation.
	template< class t, const size_t len, const t key >
	class Gen
	{
	private:
		std::array< t, len > m_buffer;

	private:
		// encrypt single character.
		constexpr t enc( const t c ) const noexcept
		{
			return c ^ key;
		}

		// decrypt single character.
		__forceinline t dec( const t c ) const noexcept
		{
			return c ^ key;
		}

	public:
		// iterate each byte and decrypt.
		__forceinline auto data( ) noexcept
		{
			for( size_t i{ 0u }; i < len; ++i )
				m_buffer[ i ] = dec( m_buffer[ i ] );

			return m_buffer.data( );
		}

		// ctor.
		template< size_t... seq >
		constexpr __forceinline Gen( const t( &s )[ len ], std::index_sequence< seq... > ) noexcept : m_buffer{ enc( s[ seq ] )... } { }
	};
}

// todo - dex;  the issue with this code is that the xor key will be static since it's in the actual function call
//              the macro needs to pass a key instead using __COUNTER__ or __LINE__ as the first template arg, the rest will be handled by the compiler (like size).
//              sadly, this increasess compile time by a fuck ton.

template< class t, const size_t len >
constexpr __forceinline auto XorStr( const t( &s )[ len ] )
{
	return xorn::Gen< t, len, GET_XOR_KEYUI8 >( s, std::make_index_sequence< len >( ) ).data( );
}

#ifdef _DEBUG
#define XOR( s ) ( s )
#else
#define XOR( s ) ( XorStr( s ) )
#endif