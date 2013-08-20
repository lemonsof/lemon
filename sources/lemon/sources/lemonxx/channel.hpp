/**
* 
* @file     channel
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/20
*/
#ifndef LEMONXX_CHANNEL_HPP
#define LEMONXX_CHANNEL_HPP
#include <lemon/abi.h>
#include <lemonxx/actor.hpp>
#include <lemonxx/nocopyable.hpp>
namespace lemon{

	class channel : private nocopyable
	{
	public:
		enum 
		{
			send_noblock = LEMON_SEND_NOBLOCK ,

			recv_noblock = LEMON_RECV_NOBLOCK,

			mult_recv = LEMON_MULTI_RECV,

			send_op = LEMON_CHANNEL_SEND,

			recv_op = LEMON_CHANNEL_RECV
		};

		channel(actor& S,lemon_channel_t handle):_S(S),_handle(handle)
		{

		}

		channel(actor& S,lemon_msg_close_f f,size_t maxlen, int flags):_S(S)
		{
			_handle = lemon_make_channel(S,f,maxlen,flags);

			_S.check_throw();
		}

		channel(actor& S,size_t maxlen, int flags):_S(S)
		{
			_handle = lemon_make_channel(S,nullptr,maxlen,flags);

			_S.check_throw();
		}

		channel(actor& S):_S(S)
		{
			_handle = lemon_make_channel(S,nullptr,size_t(-1),0);

			_S.check_throw();
		}

		void send(void * data)
		{
			lemon_send(_S,_handle,data);

			_S.check_throw();
		}

		void* recv()
		{
			void * block = lemon_recv(_S,_handle);

			_S.check_throw();

			return block;
		}

		void close(lemon_state S)
		{
			lemon_close_channel(S,_handle);
		}

		operator lemon_state (){ return _S; }

		operator lemon_channel_t () { return _handle; }

	private:

		actor											&_S;

		lemon_channel_t									_handle;
	};

	class scope_channel : public channel
	{
	public:
		scope_channel(actor& S,lemon_channel_t handle):channel(S,handle)
		{

		}

		~scope_channel()
		{
			close(*this);
		}
	};
}
#endif //LEMONXX_CHANNEL_HPP