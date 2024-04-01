#pragma once
#include "token_util.h"
#include <stack>
#include <deque>
#include "tucan_function.h"

namespace tucan_script
{
	class tucan_expression : public tucan_operable, public tucan_executable
	{
	private:
		std::vector<std::shared_ptr<tucan_entity>> m_tokens;

		static int precedence(const TUCAN_TYPE& type);

	public:
		tucan_expression();

		void appendOperable(std::shared_ptr<tucan_operable> token);
		void append(std::shared_ptr<tucan_entity> token);

		void execute() override;

		void reset();
	};
}

