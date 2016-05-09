#include "resources.h"

/////////////////////////////Resource///////////////////////////////


Resource::Resource(uint id, uint locality, bool global_resource, bool processor_local_resource)
{
	this->resource_id = id;
	this->locality = locality;
	this->global_resource = global_resource;
	this->processor_local_resource = processor_local_resource;
}


/////////////////////////////ResourceSet///////////////////////////////

ResourceSet::ResourceSet(){}

void ResourceSet::add_resource()
{
	resources.push_back(Resource(resources.size()));
}

/////////////////////////////Others///////////////////////////////

void resource_gen(ResourceSet *resourceset, int num)
{
	for(int i = 0; i < num; i++)
		resourceset->add_resource();
}
