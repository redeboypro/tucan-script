#include "tucan_const.h"

namespace tucan_script
{
	tucan_script::tucan_const::tucan_const(const std::string& originStringValue) : m_originIntValue(0), m_originFloatValue(0.0), m_originBooleanValue(false)
	{
		m_originType = TUCAN_TYPE::STRING;
		m_originStringValue = originStringValue;
		reset();
	}

	tucan_script::tucan_const::tucan_const(long long originIntValue) : m_originStringValue(std::string()), m_originFloatValue(0.0), m_originBooleanValue(false)
	{
		m_originType = TUCAN_TYPE::INTEGER;
		m_originIntValue = originIntValue;
		reset();
	}

	tucan_script::tucan_const::tucan_const(double originFloatValue) : m_originStringValue(std::string()), m_originIntValue(0), m_originBooleanValue(false)
	{
		m_originType = TUCAN_TYPE::DOUBLE;
		m_originFloatValue = originFloatValue;
		reset();
	}

	tucan_script::tucan_const::tucan_const(bool originBooleanValue) : m_originStringValue(std::string()), m_originIntValue(0), m_originFloatValue(0.0)
	{
		m_originType = TUCAN_TYPE::BOOLEAN;
		m_originBooleanValue = originBooleanValue;
		reset();
	}

	tucan_script::tucan_const::~tucan_const()
	{
		m_originType = TUCAN_TYPE::STRING;
		m_originStringValue = std::string();
		m_originIntValue = 0;
		m_originFloatValue = 0.0;
		m_originBooleanValue = false;
		reset();
	}

	void tucan_script::tucan_const::reset()
	{
		set(m_originType, m_originStringValue, m_originIntValue, m_originFloatValue, m_originBooleanValue);
	}
}