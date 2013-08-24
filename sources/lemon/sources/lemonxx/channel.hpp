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

	class channel 
	{
	public:
		channel(lemon_state S,lemon_channel_t id):_S(S),_channel(id)
		{

		}

		channel(lemon_state S, int type,size_t maxlen,lemon_msg_close_f f,void * userdata):_S(S)
		{
			_channel = lemon_new_channel(_S,type,maxlen,f,userdata);

			lemon_check_throw(S);
		}

		channel(lemon_state S, int type,lemon_msg_close_f f,void * userdata):_S(S)
		{
			_channel = lemon_new_channel(_S,type,size_t(-1),f,userdata);

			lemon_check_throw(S);
		}

		channel(lemon_state S, int type,lemon_msg_close_f f):_S(S)
		{
			_channel = lemon_new_channel(_S,type,size_t(-1),f,nullptr);

			lemon_check_throw(S);
		
		}

		channel(lemon_state S, int type):_S(S)
		{
			_channel = lemon_new_channel(_S,type,size_t(-1),nullptr,nullptr);

			lemon_check_throw(S);
		}

		~channel()
		{
			if(!empty()) lemon_close_channel(_S,_channel);
		}

		bool empty() const
		{
			return _channel == LEMON_INVALID_HANDLE(lemon_channel_t);
		}

		lemon_channel_t release()
		{
			lemon_channel_t result = _channel;

			_channel = LEMON_INVALID_HANDLE(lemon_channel_t);

			return result;
		}

		void swap(channel & rhs)
		{
			std::swap(_channel,rhs._channel);
		}

		channel& operator = (lemon_channel_t id)
		{
			channel(_S,id).swap(*this);

			return *this;
		}

		operator lemon_channel_t() const
		{
			return _channel;
		}

		void* recv()
		{
			void * data = lemon_recv(_S,_channel,0);

			lemon_check_throw(_S);

			return data;
		}

		void* recv(int flags)
		{
			void * data = lemon_recv(_S,_channel,flags);

			lemon_check_throw(_S);

			return data;
		}

		void send(void * data)
		{
			lemon_send(_S,_channel,data,0);

			lemon_check_throw(_S);
		}

		void send(void * data,int flags)
		{
			lemon_send(_S,_channel,data,flags);

			lemon_check_throw(_S);
		}

		void send_noblock(void * data)
		{
			lemon_send(_S,_channel,data,0,0);

			lemon_check_throw(_S);
		}

	private:

		lemon_state										_S;	

		lemon_channel_t									_channel;
	};

	
}

#endif //LEMONXX_CHANNEL_HPP