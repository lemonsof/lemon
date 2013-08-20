/**
* 
* @file     channel
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/16
*/
#ifndef LEMON_KERNEL_CHANNEL_HPP
#define LEMON_KERNEL_CHANNEL_HPP
#include <list>
#include <mutex>
#include <lemon/abi.h>
#include <unordered_map>
#include <lemon/assembly.h>
#include <lemonxx/nocopyable.hpp>

namespace lemon{namespace impl{

	class lemon_system;

	struct lemon_msg
	{
		lemon_t											sender;

		void											*msg;
	};

	class lemon_channel : private nocopyable
	{
	public:
		typedef std::list<lemon_msg>					msg_queue;

		typedef std::list<lemon_t>						recv_queue;

		lemon_channel(lemon_system * sysm,lemon_msg_close_f f, size_t maxlen,int flags);

		~lemon_channel();

		int flags() const { return _flags; }

		void push(lemon_state S, const lemon_msg &msg)
		{
			if(_Q.size() == _maxlen)
			{
				throw lemon_raise_errno(S,nullptr,LEMON_RESOURCE_ERROR);
			}

			_Q.push_back(msg);
		}

		bool pop(lemon_msg & msg)
		{
			if(_Q.empty()) return false;

			msg = _Q.front();

			_Q.pop_front();

			return true;
		}

		void wait_recv(lemon_state S,lemon_t receiver)
		{
			if((LEMON_MULTI_RECV & _flags) == 0 && !_recvQ.empty())
			{
				throw lemon_raise_errno(S,nullptr,LEMON_MULIT_RECV_EXCEPTION);
			}
			
			_recvQ.push_back(receiver);
		}

		void notif_one(lemon_state S)
		{
			while(!_recvQ.empty())
			{
				auto target = _recvQ.front();

				_recvQ.pop_front();

				lemon_event_t evt[] = {(lemon_event_t)this};

				if(lemon_notify(S,target,evt,sizeof(evt)/sizeof(lemon_event_t)))
				{
					lemon_log_debug(S,"notify the target :0x%06llx",target);

					break;
				}
			}
		}

	private:					

		lemon_system									*_sysm;

		lemon_msg_close_f								_f;

		size_t											_maxlen;

		int												_flags;

		msg_queue										_Q;

		recv_queue										_recvQ;
	};
	
}}
#endif //LEMON_KERNEL_CHANNEL_HPP