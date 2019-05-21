#ifndef HOME_CONFIG_PARSER_H_
#define HOME_CONFIG_PARSER_H_

#include <config4cpp/Configuration.h>
#include <stdint.h>
#include "HomeConfigParserException.h"

using namespace CONFIG4CPP_NAMESPACE;

typedef struct {
	uint16_t ce_pin;
} NRF_Config;

class HomeConfigParser
{
public:
	HomeConfigParser();
	~HomeConfigParser();

	void parse(const char * filename, const char * scope)
											throw (HomeConfigParserException);

	//--------
	// Operations to query information about recipes
	//--------
	const NRF_Config getNRFConfig() throw (HomeConfigParserException);

private:
	//--------
	// Instance variables
	//--------
	CONFIG4CPP_NAMESPACE::Configuration *	m_cfg;
	StringBuffer							m_scope;
	bool									m_parseCalled;
	StringVector							m_recipeScopeNames;

	//--------
	// The following are not implemented
	//--------
	HomeConfigParser & operator=(const HomeConfigParser &);
	HomeConfigParser(const HomeConfigParser &);
};

#endif
