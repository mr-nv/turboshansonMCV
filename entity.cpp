#include "entity.h"

DWORD CBaseEntity::GetOffset( hash32_t table, hash32_t prop )
{
	return netvars::Get( table, prop );
}