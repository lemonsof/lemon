/**
* 
* @file     nocopyable
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/01/14
*/
#ifndef HELIX_CXX_UTILITY_NOCOPYABLE_HPP
#define HELIX_CXX_UTILITY_NOCOPYABLE_HPP
namespace helix{

	class nocopyable{
	public:
		nocopyable(){}
	private:

		nocopyable(const nocopyable & rhs);

		const nocopyable & operator = (const nocopyable & rhs);
	};

}
#endif //HELIX_CXX_UTILITY_NOCOPYABLE_HPP
