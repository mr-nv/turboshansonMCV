#pragma once
#include "main.h"

#pragma warning( disable : 4018 )

namespace netvars
{
	struct Data
	{
		uint32_t table;
		uint32_t prop;
		RecvProp* recvprop;
		RecvVarProxyFn proxy;
		int offset;
	};

	extern std::vector< Data > data;

	extern void Start( );
	extern void Populate( uint32_t tablehash, RecvTable* table, int add = 0 );
	extern int Get( uint32_t tablehash, uint32_t prophash );
	//extern int GetDataMap( datamap_t* map, hash32_t name, bool recursive = false ); // dont touch the recursive argument
}