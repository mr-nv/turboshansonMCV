#pragma once
#include "sdk.h"

// greatest vmt hooking class to ever exist
// never let humanity down

// this class might be older than some tf2/csgo players
class CVMTHookManager
{
public:
	CVMTHookManager( void )
	{
		memset( this, 0, sizeof( CVMTHookManager ) );
	}

	CVMTHookManager( void* base )
	{
		Initialize( ( PDWORD* )base );
	}

	CVMTHookManager( PDWORD* ppdwClassBase )
	{
		Initialize( ppdwClassBase );
	}

	~CVMTHookManager( void )
	{
		Unhook( );
	}

	bool Initialize( PDWORD* ppdwClassBase )
	{
		m_ppdwClassBase = ppdwClassBase;
		m_pdwOldVMT = *ppdwClassBase;
		m_dwVMTSize = dwGetVMTCount( *ppdwClassBase );
		m_pdwNewVMT = new DWORD[ m_dwVMTSize ];
		memcpy( m_pdwNewVMT, m_pdwOldVMT, sizeof( DWORD ) * m_dwVMTSize );
		*ppdwClassBase = m_pdwNewVMT;

		return true;
	}

	bool Initialize( PDWORD** pppdwClassBase )
	{
		return Initialize( *pppdwClassBase );
	}

	void Unhook( void )
	{
		if( m_ppdwClassBase )
		{
			*m_ppdwClassBase = m_pdwOldVMT;
		}
	}

	void Rehook( void )
	{
		if( m_ppdwClassBase )
		{
			*m_ppdwClassBase = m_pdwNewVMT;
		}
	}

	int GetSize( void )
	{
		return ( int )m_dwVMTSize;
	}

	DWORD GetMethodAddress( int Index )
	{
		if( Index >= 0 && Index <= ( int )m_dwVMTSize && m_pdwOldVMT != NULL )
		{
			return m_pdwOldVMT[ Index ];
		}
		return NULL;
	}

	PDWORD GetOldVMT( void )
	{
		return m_pdwOldVMT;
	}

	DWORD Hook( void* dwNewFunc, unsigned int iIndex )
	{
		if( m_pdwNewVMT && m_pdwOldVMT && iIndex <= m_dwVMTSize && iIndex >= 0 )
		{
			m_pdwNewVMT[ iIndex ] = ( DWORD )dwNewFunc;
			return m_pdwOldVMT[ iIndex ];
		}
		return NULL;
	}

private:
	DWORD dwGetVMTCount( PDWORD pdwVMT )
	{
		DWORD dwIndex = 0;

		for( dwIndex = 0; pdwVMT[ dwIndex ]; dwIndex++ )
		{
			if( IsBadCodePtr( ( FARPROC )pdwVMT[ dwIndex ] ) )
			{
				break;
			}
		}
		return dwIndex;
	}
	PDWORD* m_ppdwClassBase;
	PDWORD	m_pdwNewVMT, m_pdwOldVMT;
	DWORD	m_dwVMTSize;
};