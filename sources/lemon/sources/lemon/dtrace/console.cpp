#include <chrono>
#include <iomanip>
#include <iostream>
#include <lemon/abi.h>

namespace lemon{namespace dtrace{

#ifdef WIN32

	void set_error_warning_color()
	{
		HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hCon,FOREGROUND_RED);
	}

	void set_error_warning_color_end()
	{
		HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hCon,FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN);
	}

	void set_text_color()
	{
		HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hCon,FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	}

	void set_text_color_end()
	{
		HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hCon,FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN);
	}

	void set_debug_verbose_color()
	{
		HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hCon,FOREGROUND_INTENSITY);
	}

	void set_debug_verbose_color_end()
	{
		HANDLE hCon=GetStdHandle(STD_OUTPUT_HANDLE);

		SetConsoleTextAttribute(hCon,FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN);
	}
#else

	void set_error_warning_color_end()
	{

	}

	void set_text_color_end()
	{

	}

	void set_debug_verbose_color_end()
	{

	}

	void set_error_warning_color()
	{
		
	}

	void set_text_color()
	{
		
	}

	void set_debug_verbose_color()
	{
		
	}

#endif //WIN32

	inline void __console_printf(lemon_int64_t timestamp,lemon_t source, const char * msg)
	{
		using namespace std::chrono;

		std::cout << std::hex;

		time_t tt = (time_t)timestamp / 1000000;

#ifdef WIN32
		tm t;

		localtime_s(&t,&tt);

		std::cout << std::put_time(&t, "%m/%d/%y %H:%M:%S");
#else
		std::cout << std::put_time(std::localtime(&tt), "%m/%d/%y %H:%M:%S");
#endif //WIN32

		std::cout << "." << std::setw(6) << std::setfill('0') << std::dec << timestamp % 1000000 << " [0x" << std::setw(8) << std::setfill('0') << source << "] " << msg << std::endl;
	}

	inline void  console_f(void *, int level, lemon_int64_t timestamp,lemon_t source, const char * msg)
	{
		switch (level)
		{
		case LEMON_TRACE_FATAL:
			{
				set_error_warning_color();

				std::cout << "[FL] ";

				__console_printf(timestamp,source,msg);

				set_error_warning_color_end();

				break;
			}
		case LEMON_TRACE_ERROR:
			{
				set_error_warning_color();

				std::cout << "[ER] "; 

				__console_printf(timestamp,source,msg);

				set_error_warning_color_end();

				break;
			}
		case LEMON_TRACE_WARNING:
			{
				set_error_warning_color();

				std::cout << "[WN] "; 

				__console_printf(timestamp,source,msg);

				set_error_warning_color_end();

				break;
			}
		case LEMON_TRACE_TEXT:
			{
				set_text_color();

				std::cout << "[TX] "; 

				__console_printf(timestamp,source,msg);

				set_text_color_end();

				break;
			}
		case LEMON_TRACE_DEBUG:
			{
				set_debug_verbose_color();

				std::cout << "[DG] "; 

				__console_printf(timestamp,source,msg);

				set_debug_verbose_color_end();

				break;
			}
		case LEMON_TRACE_VERBOSE:
			{
				set_debug_verbose_color();

				std::cout << "[VB] ";

				__console_printf(timestamp,source,msg);

				set_debug_verbose_color_end();

				break;
			}
		default:
			{
				set_error_warning_color();

				std::cout << "[UN] ";

				__console_printf(timestamp,source,msg);

				set_error_warning_color_end();

			}
		}
	}

}}

LEMON_API lemon_trace_t lemon_console_trace(lemon_state self,int levels)
{
	return lemon_new_trace(self,&lemon::dtrace::console_f,nullptr,levels);
}
