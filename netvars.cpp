#define _CRT_SECURE_NO_WARNINGS
#include "netvars.h"

class CClientClass
{
public:
	BYTE Pad[ 8 ];
	char* szNetworkName;
	RecvTable* pRecvTable;
	CClientClass* pNextClass;
	int iClassID;
};

//#define DUMPNETVARS

namespace netvars
{
	std::vector< Data > data;

#ifdef DUMPNETVARS
	FILE* file = nullptr;
#endif

	void Start( )
	{
		typedef CClientClass*( __thiscall* GetAllClassesFn )( void* );
		auto classes = GetVFunc< GetAllClassesFn >( client, 8 )( client );

		if( !classes )
		{
			speak( "Failed [client->GetAllClasses() is NULL]" );
			return;
		}

#ifdef DUMPNETVARS
		file = fopen( "netvars.log", "a" );
#endif

		for( auto _class = classes; _class != nullptr; _class = _class->pNextClass )
		{
			auto table = _class->pRecvTable;
			if( table )
			{
#ifdef DUMPNETVARS
				char text[ 128 ];
				sprintf_s( text, "[%s]\n", table->m_pNetTableName );
				fwrite( text, strlen( text ), 1, file );
#endif

				Populate( FNV1a::get( table->m_pNetTableName ), table );
			}
		}

#ifdef DUMPNETVARS
		fclose( file );
#endif
	}

	void Populate( uint32_t tablehash, RecvTable* table, int add )
	{
		Data _data;
		_data.table = tablehash;

		for( int i = 0; i < table->m_nProps; i++ )
		{
			auto prop = &table->m_pProps[ i ];
			if( !prop ) continue;

			if( prop->m_RecvType == DPT_DataTable )
				Populate( tablehash, prop->GetDataTable( ), add + prop->GetOffset( ) );
			else
			{
				/*if( strstr( prop->m_pVarName, "Team" ) )
					speak( "%s -> %s -> 0x%X", table->m_pNetTableName, prop->m_pVarName, add + prop->GetOffset( ) );*/
					/*if( strstr( prop->m_pVarName, "m_hOwnerEntity" ) )
						speak( "%s -> %s -> 0x%X", table->m_pNetTableName, prop->m_pVarName, add + prop->GetOffset( ) );*/
						/*if( strstr( prop->m_pVarName, "spawn" ) )
							speak( "%s -> %s -> 0x%X", table->m_pNetTableName, prop->m_pVarName, add + prop->GetOffset( ) );
						if( strstr( table->m_pNetTableName, "Player" ) )
							speak( "%s -> %s -> 0x%X", table->m_pNetTableName, prop->m_pVarName, add + prop->GetOffset( ) );*/
							/*if( strstr( prop->m_pVarName, "munity" ) || strstr( prop->m_pVarName, "Time" ) || strstr( prop->m_pVarName, "time" ) )
								speak( "%s -> %s -> 0x%X", table->m_pNetTableName, prop->m_pVarName, add + prop->GetOffset( ) );*/

								//if( !strcmp( prop->m_pVarName, "m_nPlayerCond" ) )
								//	speak( "cond %s -> %s -> 0x%X", table->m_pNetTableName, prop->m_pVarName, add + prop->GetOffset( ) );

								////if( add + prop->GetOffset( ) == 6204 )
								//	//speak("offset %s -> %s -> 0x%X", table->m_pNetTableName, prop->m_pVarName, add + prop->GetOffset( ) );

								//if( !strcmp( prop->m_pVarName, "m_Shared" ) )
								//	speak( "%s -> %s -> 0x%X", table->m_pNetTableName, prop->m_pVarName, add + prop->GetOffset( ) );

#ifdef DUMPNETVARS
				if( prop->m_pVarName[ 0 ] != '0' )
				{
					char text2[ 1024 ];
					sprintf_s( text2, "> %s (0x%X)\n", prop->m_pVarName, add + prop->GetOffset( ) );
					fwrite( text2, strlen( text2 ), 1, file );
				}
				//printf( "> %s (0x%X)\n", prop->m_pVarName, add + prop->GetOffset( ) );
#endif

				_data.prop = FNV1a::get( prop->m_pVarName );
				_data.recvprop = prop;
				_data.proxy = prop->GetProxyFn( );
				_data.offset = add + prop->GetOffset( );
				data.emplace_back( _data );
			}
		}
	}

	int Get( uint32_t tablename, uint32_t propname )
	{
		int ret = 0;

		for( int i = 0; i < data.size( ); i++ )
		{
			if( !&data[ i ] ) continue;

			const auto& _data = data[ i ];

			if( _data.table == tablename && _data.prop == propname )
			{
				ret = _data.offset;
				break;
			}
		}

		if( ret == 0 )
			speak( "Can't find netvar [%i -> %i]", tablename, propname );

		return ret;
	}

	/*int GetDataMap( datamap_t* map, hash32_t name, bool recursive )
	{
		int ret = 0;

		for( auto _map = map; _map != nullptr; _map = _map->baseMap )
		{
			for( int i = 0; i < _map->dataNumFields; i++ )
			{
				auto desc = _map->dataDesc[ i ];
				if( !desc.fieldName ) continue;

				if( FNV1a::get( desc.fieldName ) == name )
				{
					ret = desc.fieldOffset[ TD_OFFSET_NORMAL ];
					break;
				}

				if( desc.fieldType == FIELD_EMBEDDED && desc.td )
				{
					auto offset = GetDataMap( desc.td, name, true );
					if( offset )
					{
						ret = offset;
						break;
					}
				}
			}
		}

		return ret;
	}*/
}