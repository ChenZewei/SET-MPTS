#include "param.h"

uint Param::get_method_num()
{
	return test_attributes.size();
}

int Param::get_test_method(uint index)
{
	return test_attributes[index].test_method;
}

int Param::get_test_type(uint index)
{
	return test_attributes[index].test_type;
}

