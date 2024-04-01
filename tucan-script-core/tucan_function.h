#pragma once
#include <functional>
#include <vector>
#include <unordered_map>
#include "tucan_expression.h"

namespace tucan_script
{
	class tucan_function_argument : public tucan_operable_clone
	{
	public:
		bool is_reference;

		tucan_function_argument(bool reference, std::shared_ptr<tucan_operable> operable);
	};

	class tucan_function : public tucan_operable, public tucan_executable, public tucan_returnable
	{
	private:
		std::unordered_map<std::string, std::shared_ptr<tucan_function_argument>> m_arguments;
		bool m_running;

	public:
		~tucan_function() = default;

		long long involvedArgumentCount;

		size_t getArgumentCount() const;

		std::shared_ptr<tucan_function_argument> getArgById(int index);
		std::shared_ptr<tucan_function_argument> getArgByName(const std::string& name);

		void setArg(size_t index, std::shared_ptr<tucan_operable>& operable);

		void append(std::shared_ptr<tucan_executable> executable);
		void append(const std::string& name, bool reference);

		void execute() override;
		void return_value() override;
		void reset() override;
	};

	class tucan_function_call : public tucan_entity
	{
	private:
		std::vector<std::shared_ptr<tucan_operable>> m_arguments;
		std::shared_ptr<tucan_function> m_function;
		std::shared_ptr<tucan_operable> m_result;

	public:
		tucan_function_call(std::shared_ptr<tucan_function> function);

		void append(std::shared_ptr<tucan_operable> arg);
		std::shared_ptr<tucan_operable> invoke();
	};

	class tucan_external_executable : public tucan_executable
	{
	private:
		std::shared_ptr<tucan_function> m_function;

	public:
		tucan_external_executable(std::shared_ptr<tucan_function>& function);

		std::function<void(tucan_function&)> del_action;

		void execute() override;

		void reset() override;
	};
}

