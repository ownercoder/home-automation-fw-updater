#ifndef HOME_CONFIG_PARSER_EXCEPTION_H_
#define HOME_CONFIG_PARSER_EXCEPTION_H_

#include <string.h>

class HomeConfigParserException
{
public:
	HomeConfigParserException(const char * str)
	{
		m_str = new char[strlen(str) + 1];
		strcpy(m_str, str);
	}

	HomeConfigParserException(const HomeConfigParserException & o)
	{
		m_str = new char[strlen(o.m_str) + 1];
		strcpy(m_str, o.m_str);
	}

	~HomeConfigParserException() { delete [] m_str; }

	//--------
	// Accessor
	//--------
	const char * c_str() const { return m_str; }

private:
	//--------
	// Instance variables
	//--------
	char *			m_str;

	//--------
	// The following are unimplemented
	//--------
	HomeConfigParserException();
	HomeConfigParserException operator=(const HomeConfigParserException &);
};

#endif
