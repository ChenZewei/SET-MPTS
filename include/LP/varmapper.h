#ifndef VARMAPPER_H
#define VARMAPPER_H

#include <stdint.h>
#include <hash_map>
#include <string>

using namespace std;

enum blocking_type
{
	BLOCKING_DIRECT,
	BLOCKING_INDIRECT,
	BLOCKING_PREEMPT,
	BLOCKING_OTHER
};

class VarMapperBase 
{
	private:
		hashmap<uint64_t, unsigned int> map;
		unsigned int next_var;
		bool sealed;

	protected:
		void insert(uint64_t key);
		bool exists(uint64_t key) const;
		unsigned int get(uint64_t key);
		unsigned int var_for_key(uint64_t key);
		bool search_key_for_var(unsigned int var, uint64_t &key) const;
	public:
		VarMapperBase(unsigned int start_var = 0);
		// stop new IDs from being generated
		void seal();
		unsigned int get_num_vars() const;
		unsigned int get_next_var() const;
		hashmap<unsigned int, string> get_translation_table() const;

		// debugging support
		string var2str(unsigned int var) const;

		// should be overridden by children
		virtual string key2str(uint64_t key, unsigned int var) const;
};

class VarMapper: public VarMapperBase
{
	private:
		static uint64_t encode_request(uint64_t task_id, uint64_t res_id, uint64_t req_id, uint64_t blocking_type);
		static uint64_t get_task(uint64_t var);
		static uint64_t get_type(uint64_t var);
		static uint64_t get_req_id(uint64_t var);
		static uint64_t get_res_id(uint64_t var);
	public:
		VarMapper(unsigned int start_var = 0);
		unsigned int lookup(unsigned int task_id, unsigned int res_id, unsigned int req_id, blocking_type type);
		string key2str(uint64_t key, unsigned int var) const;						
													
}

#endif
