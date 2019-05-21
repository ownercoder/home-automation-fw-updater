#include "HomeConfigParser.h"

#include <config4cpp/SchemaValidator.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string>

using CONFIG4CPP_NAMESPACE::Configuration;
using CONFIG4CPP_NAMESPACE::ConfigurationException;
using CONFIG4CPP_NAMESPACE::SchemaValidator;

HomeConfigParser::HomeConfigParser()
{
	m_cfg = 0;
	m_parseCalled = false; // set to 'true' after a successful parse().
}



HomeConfigParser::~HomeConfigParser()
{
	m_cfg->destroy();
}



void
HomeConfigParser::parse(
	const char *		filename,
	const char *		scope) throw (HomeConfigParserException)
{
	SchemaValidator		sv;
	StringBuffer		filter;

	assert(!m_parseCalled);
	m_cfg = Configuration::create();
	m_scope = scope;
	try {
		m_cfg->parse(filename);
	} catch(const ConfigurationException & ex) {
		throw HomeConfigParserException(ex.c_str());
	}
	m_parseCalled = true;
}

const NRF
HomeConfigParser::getNRFConfig() throw (HomeConfigParserException)
{
	assert(m_parseCalled);
	try {
		std::string scope(m_scope.c_str());
		scope += ".nrf";
		NRF nrf;
		nrf.ce_pin = m_cfg->lookupInt(scope.c_str(), "pin");
		return nrf;
	} catch(const ConfigurationException & ex) {
		throw HomeConfigParserException(ex.c_str());
	}
}
