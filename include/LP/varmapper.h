#ifndef VARMAPPER_H
#define VARMAPPER_H

#include <types.h>
#include <stdint.h>
#include <unordered_map>
#include <string>

using namespace std;

class VarMapperBase 
{
	private:
		unordered_map<uint64_t, unsigned int> map;
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
		unordered_map<unsigned int, string> get_translation_table() const;

		// debugging support
		string var2str(unsigned int var) const;

		// should be overridden by children
		virtual string key2str(uint64_t key, unsigned int var) const;
};

#endif
