#include "varmapper.h"
#include <sstream>
#include <limits.h>
#include <iostream>

using namespace std;

////////////////////VarMapperBase////////////////////
void VarMapperBase::insert(uint64_t key)
{
	assert(next_var < UINT_MAX);
	assert(!sealed);
	unsigned int idx = next_var++;
	map[key] = idx;
}

bool VarMapperBase::exists(uint64_t key) const
{
	return map.count(key) > 0;
}

unsigned int VarMapperBase::get(uint64_t key)
{
	return map[key];
}

unsigned int VarMapperBase::var_for_key(uint64_t key)
{
	if (!exists(key))
		insert(key);
	return get(key);
}

bool VarMapperBase::search_key_for_var(unsigned int var, uint64_t &key) const
{
	foreach(map, it)
	{
		if (it->second == var)
		{
			key = it->first;
			return true;
		}
	}
	return false;
}

VarMapperBase::VarMapperBase(unsigned int start_var): next_var(start_var), sealed(false) {}


// stop new IDs from being generated
void VarMapperBase::seal()
{
	sealed = true;
}

unsigned int VarMapperBase::get_num_vars() const
{
	return map.size();
}

unsigned int VarMapperBase::get_next_var() const
{
	return next_var;
}

// debugging support
string VarMapperBase::var2str(unsigned int var) const
{
	uint64_t key;

	if (search_key_for_var(var, key))
	{
		return key2str(key, var);
	}
	else
		return "<?>";
}

// should be overridden by children
string VarMapperBase::key2str(uint64_t key, unsigned int var) const
{
	std::ostringstream buf;
	buf << "X" << var;
	return buf.str();
}

unordered_map<unsigned int, string> VarMapperBase::get_translation_table() const
{
	unordered_map<unsigned int, string> table;

	foreach(map, kv)
	{
		table[kv->second] = key2str(kv->first, kv->second);
	}

	return table;
}




