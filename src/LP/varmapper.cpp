#include "varmapper.h"

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

hashmap<unsigned int, string> VarMapperBase::get_translation_table() const
{
	hashmap<unsigned int, string> table;

	foreach(map, kv)
	{
		table[kv->second] = key2str(kv->first, kv->second);
	}

	return table;
}




////////////////////VarMapper////////////////////
static uint64_t VarMapper::encode_request(uint64_t task_id, uint64_t res_id, uint64_t req_id, uint64_t blocking_type)
{
	assert(task_id < (1 << 32));
	assert(res_id < (1 << 10));
	assert(req_id < (1 << 20));
	assert(blocking_type < (1 << 2));

	return (blocking_type << 62) | (task_id << 30) | (req_id << 10) | res_id;
}

static uint64_t VarMapper::get_task(uint64_t var)
{
	return (var >> 30) & (uint64_t) 0xffffffff; //32 bits
}

static uint64_t VarMapper::get_type(uint64_t var)
{
	return (var >> 62) & (uint64_t) 0xf; //2 bits
}

static uint64_t VarMapper::get_req_id(uint64_t var)
{
	return (var >> 10) & (uint64_t) 0xfffff; //20 bits
}

static uint64_t VarMapper::get_res_id(uint64_t var)
{
	return var & (uint64_t) 0x3ff; //10 bits
}

VarMapper::VarMapper(unsigned int start_var): VarMapperBase(start_var) {}

unsigned int VarMapper::lookup(unsigned int task_id, unsigned int res_id, unsigned int req_id, blocking_type type)
{
	uint64_t key = encode_request(task_id, res_id, req_id, type);
	return var_for_key(key);
}

string VarMapper::key2str(uint64_t key, unsigned int var) const
{
	std::ostringstream buf;

	switch (get_type(key))
	{
		case BLOCKING_DIRECT:
			buf << "Xd[";
			break;
		case BLOCKING_INDIRECT:
			buf << "Xi[";
			break;
		case BLOCKING_PREEMPT:
			buf << "Xp[";
			break;
		case BLOCKING_OTHER:
			buf << "Xo[";
			break;
		default:
			buf << "X?[";
	}

	buf << get_task(key) << ", "
		<< get_res_id(key) << ", "
		<< get_req_id(key) << "]";

	return buf.str();
}


