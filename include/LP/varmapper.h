#ifndef VARMAPPER_H
#define VARMAPPER_H

#include <string>

using namespace std;

enum blocking_type
{
	BLOCKING_DIRECT,
	BLOCKING_INDIRECT,
	BLOCKING_PREEMPT,
	BLOCKING_OTHER
};

class VarMapper
{
	private:
		static uint64_t encode_request(uint64_t task_id, uint64_t res_id, uint64_t req_id,
				                   uint64_t blocking_type)
		{
			assert(task_id < (1 << 32));
			assert(res_id < (1 << 10));
			assert(req_id < (1 << 20));
			assert(blocking_type < (1 << 2));

			return (blocking_type << 62) | (task_id << 30) | (req_id << 10) | res_id;
		}

		static uint64_t get_task(uint64_t var)
		{
			return (var >> 30) & (uint64_t) 0xffffffff; //32 bits
		}

		static uint64_t get_type(uint64_t var)
		{
			return (var >> 62) & (uint64_t) 0xf; //2 bits
		}

		static uint64_t get_req_id(uint64_t var)
		{
			return (var >> 10) & (uint64_t) 0xfffff; //20 bits
		}

		static uint64_t get_res_id(uint64_t var)
		{
			return var & (uint64_t) 0x3ff; //10 bits
		}
	public:
		VarMapper(unsigned int start_var = 0) {}

		unsigned int lookup(unsigned int task_id, unsigned int res_id, unsigned int req_id, blocking_type type)
		{
			uint64_t key = encode_request(task_id, res_id, req_id, type);
			return var_for_key(key);
		}

		string key2str(uint64_t key, unsigned int var) const
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
																							
}

#endif
