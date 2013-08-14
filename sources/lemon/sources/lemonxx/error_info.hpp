/**
* 
* @file     error_info
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/09
*/
#ifndef LEMON_ERROR_INFO_HPP
#define LEMON_ERROR_INFO_HPP

#include <string>
#include <cassert>
#include <lemon/abi.h>

#define raise(...)	__raise_errno(__VA_ARGS__)

namespace lemon{

	class error_info : public lemon_errno_info
	{
	public:

		error_info(){}

		error_info(const lemon_errno_info &ec)
		{
			*this = ec;
		}

		error_info & operator = (const lemon_errno_info &ec)
		{
			lemon_errno_info::error  = ec.error;

			lemon_errno_info::file = ec.file;

			lemon_errno_info::lines = ec.lines;

			return *this;
		}

		void reset() { LEMON_RESET_ERRORINFO(*this);}

		operator lemon_errno_info * () { return this; }

		operator const lemon_errno_info * () const { return this; }


		void __raise_errno(lemon_state S, const char * msg)
		{
			lemon_raise_errno(S,msg,this);

			throw *this;
		}

		void __raise_errno(lemon_state S)
		{
			lemon_raise_errno(S,NULL,this);

			throw *this;
		}

		bool fail() const { return LEMON_FAILED(*this); }

		bool success() const { return LEMON_SUCCESS(*this); }

	private:

		std::string										_errorMsg;
	};

}
#endif //LEMON_ERROR_INFO_HPP
