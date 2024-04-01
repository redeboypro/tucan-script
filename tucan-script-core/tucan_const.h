#pragma once
#include "tucan_operable.h"

namespace tucan_script
{
	class tucan_const : public tucan_operable, public tucan_resetable
	{
	private:
		TUCAN_TYPE m_originType;
		std::string m_originStringValue;
		long long m_originIntValue;
		double m_originFloatValue;
		bool m_originBooleanValue;

	public:
		tucan_const(const std::string& originStringValue);
		tucan_const(long long originIntValue);
		tucan_const(double originFloatValue);
		tucan_const(bool originBooleanValue);
		~tucan_const();

		void reset() override;
	};
}


