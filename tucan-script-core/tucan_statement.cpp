#include "tucan_statement.h"

namespace tucan_script
{
	tucan_statement_while::tucan_statement_while(std::shared_ptr<tucan_expression> condition) :
		m_condition(std::move(condition)), m_running(false), m_continue(false) {}

	void tucan_statement_while::execute()
	{
		m_running = true;
		m_condition->execute();
		while (m_condition->toBoolean())
		{
			for (auto& executable : executables)
			{
				executable->execute();

				if (m_continue)
				{
					m_continue = false;
					continue;
				}

				if (!m_running) return;
			}

			m_condition->execute();
		}
	}

	void tucan_statement_while::break_loop()
	{
		m_running = false;
	}

	void tucan_statement_while::continue_loop()
	{
		m_continue = true;
	}

	tucan_statement_for::tucan_statement_for(const std::vector<std::shared_ptr<tucan_operable>>& variables, std::shared_ptr<tucan_expression> array) :
		m_variables(variables), m_array(array), m_running(false), m_continue(false) {}

	void tucan_statement_for::execute()
	{
		m_running = true;
		m_array->execute();
		for (size_t index = 0; index < m_array->length(); index++)
		{
			auto& element = (*m_array).getElement(index);

			for (auto& operable : m_variables)
				operable->set(element);

			for (auto& executable : executables)
			{
				executable->execute();

				if (m_continue)
				{
					m_continue = false;
					continue;
				}

				if (!m_running) return;
			}
		}
	}

	void tucan_statement_for::break_loop()
	{
		m_running = false;
	}

	void tucan_statement_for::continue_loop()
	{
		m_continue = true;
	}

	tucan_statement_if::tucan_statement_if(std::shared_ptr<tucan_expression> condition) :
		m_condition(condition), m_running(false), m_continue(false) {}

	void tucan_statement_if::execute()
	{
		m_condition->execute();

		if (!m_condition->toBoolean()) return;

		for (auto& executable : executables)
			executable->execute();
	}
}
