/**
* 
* @file     buffer
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/21
*/
#ifndef LEMONXX_BUFFER_HPP
#define LEMONXX_BUFFER_HPP
#include <vector>
#include <type_traits>

namespace lemon{

	template<typename T> struct const_buff
	{
		typedef typename std::add_pointer<typename std::add_const<T>::type>::type buff_type;

		buff_type					buff;

		size_t						length;
	};


	template<typename T> struct mutable_buff
	{
		typedef typename std::add_pointer<typename std::remove_cv<T>::type>::type buff_type;

		buff_type					buff;

		size_t						length;
	};

	template<typename T>
	inline const_buff<T> cbuff(const T *source,size_t len)
	{
		const_buff<T> tmp = {source,len};

		return tmp;
	}

	template<typename T,size_t N>
	inline const_buff<T> cbuff(const T(&source)[N])
	{
		const_buff<T> tmp = {source,N};

		return tmp;
	}

	template<typename T>
	inline const_buff<T> cbuff(const std::vector<T> & source)
	{
		if(!source.empty())
		{
			const_buff<T> tmp = {&source[0],source.size()};

			return tmp;
		}

		const_buff<T> tmp;

		return reset(tmp);
	}

	template<typename Buffer>
	inline Buffer& reset(Buffer & buffer)
	{
		buffer.buff = nullptr; buffer.length = 0;

		return buffer;
	}

	template<typename Buffer>
	inline bool empty(const Buffer & buffer)
	{
		return buffer.buff == nullptr || buffer.length == 0;
	}
}

namespace std{

	template<typename Buffer>
	inline typename Buffer::buff_type begin(const Buffer & buffer)
	{
		return buffer.buff;
	}

	template<typename Buffer>
	inline typename Buffer::buff_type end(const Buffer & buffer)
	{
		return buffer.buff + buffer.length;
	}

}

#endif //LEMONXX_BUFFER_HPP