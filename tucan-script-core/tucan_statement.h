#pragma once
#include "tucan_expression.h"

namespace tucan_script
{
	class tucan_statement_while : public tucan_statement, public tucan_loop
	{
	private:
		std::shared_ptr<tucan_expression> m_condition;
		bool m_running, m_continue;

	public:
		tucan_statement_while(std::shared_ptr<tucan_expression> condition);

		void execute() override;
		void break_loop() override;
		void continue_loop() override;
	};

	class tucan_statement_for : public tucan_statement, public tucan_loop
	{
	private:
		std::vector<std::shared_ptr<tucan_operable>> m_variables;
		std::shared_ptr<tucan_expression> m_array;
		bool m_running, m_continue;

	public:
		tucan_statement_for(const std::vector<std::shared_ptr<tucan_operable>>& variables, std::shared_ptr<tucan_expression> array);

		void execute() override;
		void break_loop() override;
		void continue_loop() override;
	};

	class tucan_statement_if : public tucan_statement
	{
	private:
		std::shared_ptr<tucan_expression> m_condition;
		bool m_running, m_continue;

	public:
		tucan_statement_if(std::shared_ptr<tucan_expression> condition);

		void execute() override;
	};
}