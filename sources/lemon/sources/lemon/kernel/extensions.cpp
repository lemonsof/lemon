#include <lemon/assembly.h>
#include <lemon/kernel/system.hpp>
#include <lemon/kernel/extensions.hpp>

namespace lemon{namespace kernel{

	lemon_extension_system::lemon_extension_system():_counter(0)
	{
		memset(_buckets,0,sizeof(_buckets));
	}

	lemon_extension_system::~lemon_extension_system()
	{

	}

	void lemon_extension_system::stop()
	{

		std::unique_lock<std::mutex> lock(_mutex);

		for(size_t i =0; i < buckets_capacity; ++ i)
		{
			if(_buckets[i].vtable.id != 0)
			{
				_buckets[i].vtable.stop(_buckets[i].userdata);
			}
		}
	}

	void lemon_extension_system::new_extension(lemon_t source,const lemon_extension_vtable * vtable, void * userdata)
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

		lemon_log_error(_system->trace_system(),source,"the max extension loaded");

		lemon_declare_errinfo(errorCode);

		lemon_user_errno(errorCode,LEMON_RESOURCE_ERROR);

		throw errorCode;
	}

	void * lemon_extension_system::get_extension(lemon_t source,lemon_extension_t id)
	{

		for(size_t i = hash(id); i < buckets_capacity; ++ i)
		{
			if(_buckets[i].vtable.id == id)
			{
				return _buckets[i].userdata;
			}
		}

		lemon_log_error(_system->trace_system(),source,"can't found extension id : %d",(int)id);

		lemon_declare_errinfo(errorCode);

		lemon_user_errno(errorCode,LEMON_EXTENSION_NOT_FOUND);

		throw errorCode;
	}

	size_t lemon_extension_system::hash(lemon_extension_t id)
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