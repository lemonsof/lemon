/**
* 
* @file     channel
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/24
*/
#ifndef LEMONXX_CHANNEL_HPP
#define LEMONXX_CHANNEL_HPP
#include <algorithm>
#include <lemon/abi.h>
#include <lemonxx/nocopyable.hpp>

namespace lemon{

	inline lemon_channel_t new_channel(lemon_state S,int type,size_t maxlen,lemon_msg_f f,void * userdata)
	{
		auto result = lemon_new_channel(S,type,maxlen,f,userdata);

		lemon_check_throw(S);

		return result;
	}


	inline lemon_channel_t new_channel(lemon_state S,int type,size_t maxlen)
	{
		auto result = lemon_new_channel(S,type,maxlen,nullptr,nullptr);

		lemon_check_throw(S);

		return result;
	}

	inline lemon_channel_t new_channel(lemon_state S,int type)
	{
		auto result = lemon_new_channel(S,type,size_t(-1),nullptr,nullptr);

		lemon_check_throw(S);

		return result;
	}

	inline bool send(lemon_state S, lemon_channel_t channel,void* msg,int flags,size_t timeout)
	{
		bool result = lemon_send(S,channel,msg,flags,timeout);

		lemon_check_throw(S);

		return result;
	}

	inline bool send(lemon_state S, lemon_channel_t channel,void* msg,int flags)
	{
		bool result = lemon_send(S,channel,msg,flags);

		lemon_check_throw(S);

		return result;
	}

	inline bool send(lemon_state S, lemon_channel_t channel,void* msg)
	{
		bool result = lemon_send(S,channel,msg,0);

		lemon_check_throw(S);

		return result;
	}

	//inline bool send(lemon_state S, lemon_channel_t channel,void* msg,size_t timeout)
	//{
	//	bool result = lemon_send(S,channel,msg,0,timeout);

	//	lemon_check_throw(S);

	//	return result;
	//}


	inline void* recv(lemon_state S, lemon_channel_t channel,int flags,size_t timeout)
	{
		auto result = lemon_recv(S,channel,flags,timeout);

		lemon_check_throw(S);

		return result;
	}

	inline void* recv(lemon_state S, lemon_channel_t channel,int flags)
	{
		auto result = lemon_recv(S,channel,flags);

		lemon_check_throw(S);

		return result;
	}

	inline void* recv(lemon_state S, lemon_channel_t channel)
	{
		auto result = lemon_recv(S,channel,0);

		lemon_check_throw(S);

		return result;
	}

	/*inline void* recv(lemon_state S, lemon_channel_t channel,size_t timeout)
	{
		auto result = lemon_recv(S,channel,0,timeout);

		lemon_check_throw(S);

		return result;
	}*/

	template<int T>
	struct basic_channel_t 
	{
		template<typename F>
		static inline lemon_channel_t make(lemon_state S,size_t maxlen,F f, void * userdata)
		{
			return new_channel(S,T,maxlen,f,userdata);
		}

		template<typename F>
		static inline lemon_channel_t make(lemon_state S,F f, void * userdata)
		{
			return new_channel(S,T,size_t(-1),f,userdata);
		}

		template<typename F>
		static inline lemon_channel_t make(lemon_state S,size_t maxlen,F f)
		{
			return make(S,T,maxlen,f);
		}

		template<typename F>
		static inline lemon_channel_t make(lemon_state S,F f)
		{
			return new_channel(S,T,size_t(-1),f,nullptr);
		}

		static inline lemon_channel_t make(lemon_state S,size_t maxlen)
		{
			return new_channel(S,T,maxlen,nullptr,nullptr);
		}

		static inline lemon_channel_t make(lemon_state S)
		{
			return new_channel(S,T,size_t(-1),nullptr,nullptr);
		}
	};

	typedef basic_channel_t<LEMON_CHANNEL_PUSH> channel_push_t;

	typedef basic_channel_t<LEMON_CHANNEL_PUB> channel_pub_t;

	typedef basic_channel_t<LEMON_CHANNEL_REQ> channel_req_t;


	class unique_channel : private nocopyable
	{
	public:

		unique_channel():_S(nullptr),_channel(LEMON_INVALID_HANDLE(lemon_channel_t)){}

		unique_channel(lemon_state S,lemon_channel_t channel):_S(S),_channel(channel){}

		~unique_channel()
		{
			if(LEMON_INVALID_HANDLE(lemon_channel_t) != _channel) lemon_close_channel(_S,_channel);
		}

		lemon_channel_t release()
		{
			lemon_channel_t invalid = LEMON_INVALID_HANDLE(lemon_channel_t);

			std::swap(_channel,invalid);

			return invalid;
		}

		operator lemon_channel_t () const
		{
			return _channel; 
		}

	private:

		lemon_state												_S;

		lemon_channel_t											_channel;
	};
	
}

#endif //LEMONXX_CHANNEL_HPP