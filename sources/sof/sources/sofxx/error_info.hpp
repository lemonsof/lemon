/**
* 
* @file     error_info
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/09
*/
#ifndef SOF_ERROR_INFO_HPP
#define SOF_ERROR_INFO_HPP

#include <string>
#include <cassert>
#include <sof/abi.h>

#define raise(...)	__raise_errno(__VA_ARGS__)

namespace sof{

	class error_info : public sof_errno_info
	{
	public:

		error_info(){}

		error_info(const sof_errno_info &ec)
		{
			*this = ec;
		}

		error_info & operator = (const sof_errno_info &ec)
		{
			sof_errno_info::error  = ec.error;

			sof_errno_info::file = ec.file;

			sof_errno_info::lines = ec.lines;

			return *this;
		}

		void reset() { SOF_RESET_ERRORINFO(*this);}

		operator sof_errno_info * () { return this; }

		operator const sof_errno_info * () const { return this; }


		void __raise_errno(sof_state S, const char * msg)
		{
			sof_raise_errno(S,msg,this);

			throw *this;
		}

		void __raise_errno(sof_state S)
		{
			sof_raise_errno(S,NULL,this);

			throw *this;
		}

		bool fail() const { return SOF_FAILED(*this); }

		bool success() const { return SOF_SUCCESS(*this); }

	private:

		std::string										_errorMsg;
	};

}
#endif //SOF_ERROR_INFO_HPP