#include <sof/extensions.hpp>


namespace sof{namespace impl{

	sof_extension_system::sof_extension_system():_counter(0)
	{
		memset(_buckets,0,sizeof(_buckets));
	}

	sof_extension_system::~sof_extension_system()
	{

	}

	void sof_extension_system::stop(sof_state S)
	{

		std::unique_lock<std::mutex> lock(_mutex);

		for(size_t i =0; i < buckets_capacity; ++ i)
		{
			if(_buckets[i].vtable.id != 0)
			{
				_buckets[i].vtable.stop(S,_buckets[i].userdata);
			}
		}
	}

	void sof_extension_system::new_extension(sof_state S,const sof_extension_vtable * vtable, void * userdata)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if(_counter != max_extensions)
		{
			for(size_t i = hash(vtable->id); i < buckets_capacity; ++ i)
			{
				if(_buckets[i].vtable.id == 0)
				{
					_buckets[i].vtable = *vtable;

					_buckets[i].userdata = userdata;

					++ _counter;

					return;
				}
			}
		}

		error_info errorCode;

		SOF_USER_ERROR(errorCode,SOF_RESOURCE_ERROR);

		errorCode.raise(S,"the extension hash map no empty bucket");
	}

	void * sof_extension_system::get_extension(sof_state S,sof_extension_t id)
	{

		for(size_t i = hash(id); i < buckets_capacity; ++ i)
		{
			if(_buckets[i].vtable.id == id)
			{
				return _buckets[i].userdata;
			}
		}

		error_info errorCode;

		SOF_USER_ERROR(errorCode,SOF_RESOURCE_ERROR);

		errorCode.raise(S);

		return nullptr;
	}

	size_t sof_extension_system::hash(sof_extension_t id)
	{
		size_t hashCode = (size_t)id;

#if SIZE_MAX > 0xffffffff

		hashCode = (~hashCode) + (hashCode << 18); // hashCode = (hashCode << 18) - hashCode - 1;

		hashCode = hashCode ^ (hashCode >> 31);

		hashCode = hashCode * 21; // hashCode = (hashCode + (hashCode << 2)) + (hashCode << 4);

		hashCode = hashCode ^ (hashCode >> 11);

		hashCode = hashCode + (hashCode << 6);

		hashCode = hashCode ^ (hashCode >> 22);

#else

		hashCode = ~hashCode + (hashCode << 15); // hashCode = (hashCode << 15) - hashCode - 1;

		hashCode = hashCode ^ (hashCode >> 12);

		hashCode = hashCode + (hashCode << 2);

		hashCode = hashCode ^ (hashCode >> 4);

		hashCode = hashCode * 2057; // hashCode = (hashCode + (hashCode << 3)) + (hashCode << 11);

		hashCode = hashCode ^ (hashCode >> 16);

#endif

		return (((hashCode) >> 3) ^ 0x7FFFFFFF) % max_extensions;
	}

}}
