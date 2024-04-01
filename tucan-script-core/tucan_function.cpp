#include "tucan_function.h"

namespace tucan_script
{
	tucan_function_argument::tucan_function_argument(bool reference, std::shared_ptr<tucan_operable> operable) :
		is_reference(reference), tucan_operable_clone(operable) {}

	size_t tucan_function::getArgumentCount() const
	{
		return m_arguments.size();
	}

	std::shared_ptr<tucan_function_argument> tucan_function::getArgById(int index)
	{
		if (index >= m_arguments.size())
			throw std::out_of_range("Index out of range");

		auto it = m_arguments.begin();
		std::advance(it, index);

		return it->second;
	}

	std::shared_ptr<tucan_function_argument> tucan_function::getArgByName(const std::string& name)
	{
		auto it = m_arguments.find(name);

		if (it != m_arguments.end())
			return it->second;

		return nullptr;
	}

	void tucan_function::setArg(size_t index, std::shared_ptr<tucan_operable>& operable)
	{
		if (index >= m_arguments.size())
			append("arg" + std::to_string(index), false);

		auto arg = getArgById(index);
		arg->setSource(operable);
		arg->reset();
	}

	void tucan_function::append(std::shared_ptr<tucan_executable> executable)
	{
		executables.push_back(executable);
	}

	void tucan_function::append(const std::string& name, bool reference)
	{
		m_arguments[name] = std::make_shared<tucan_function_argument>(reference, nullptr);
	}

	void tucan_function::execute()
	{
		m_running = true;

		for (auto& executable : executables)
		{
			executable->execute();
			if (!m_running) return;
		}

		for (auto& pair : m_arguments)
		{
			auto& arg = pair.second;

			if (!arg)
				std::cerr << "Invalid argument" << std::endl;

			if (arg->is_reference)
				arg->apply();
		}
	}

	void tucan_function::return_value()
	{
		m_running = false;
	}

	void tucan_function::reset()
	{
		for (auto& executable : executables)
			executable->reset();
	}

	tucan_function_call::tucan_function_call(std::shared_ptr<tucan_function> function) :
		m_result(std::make_shared<tucan_operable>()),
		m_function(function), tucan_entity(TUCAN_TYPE::CALL) {}

	void tucan_function_call::append(std::shared_ptr<tucan_operable> arg)
	{
		m_arguments.push_back(arg);
	}

	std::shared_ptr<tucan_operable> tucan_function_call::invoke()
	{
		if (m_function == nullptr)
			return m_result;

		for (size_t index = 0; index < m_arguments.size(); index++)
		{
			auto& arg = m_arguments[index];

			auto expression = std::dynamic_pointer_cast<tucan_expression>(arg);
			auto resetable = std::dynamic_pointer_cast<tucan_resetable>(arg);

			if (resetable && !expression)
				resetable->reset();
			else if (expression)
				expression->execute();

			m_function->setArg(index, arg);
		}

		m_function->involvedArgumentCount = m_arguments.size();
		m_function->execute();
		m_result->set(*m_function);

		return m_result;
	}

	tucan_external_executable::tucan_external_executable(std::shared_ptr<tucan_function>& function) : m_function(function) {}

	void tucan_external_executable::execute()
	{
		del_action(*m_function);
	}

	void tucan_external_executable::reset()
	{
		std::cerr << "Cannot be reset" << std::endl;
	}
}
