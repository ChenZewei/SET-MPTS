#ifndef SCHED_TEST_MAPPER
#define SCHED_TEST_MAPPER

#include <types.h>
#include <stdint.h>
#include <unordered_map>
#include <string>

using namespace std;

class SchedTestMapper
{
	private:
		unordered_map<uint64_t, uint> map;
	public:
		SchedTestMapper();
		uint get_method_num() const;
		uint get_next_idx() const;

		unordered_map<uint, string> get_translation_table() const;

		void insert(uint64_t key);
		bool exists(uint64_t key) const;
		uint get(uint64_t key);
		uint get_methord_id(uint64_t key);
		string key2str(uint64_t key, uint var) const;
}

#endif
