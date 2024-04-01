#pragma once
#include "tucan_function.h"
#include "tucan_statement.h"
#include <functional>

namespace tucan_script
{
	class tucan_module : public tucan_executable, public tucan_returnable, public std::enable_shared_from_this<tucan_module>
	{
	private:
		std::unordered_map<std::string, std::shared_ptr<tucan_operable>> m_variables;
		std::unordered_map<std::string, std::shared_ptr<tucan_function>> m_functions;
		bool m_running;

		void process_container(
			std::vector<std::shared_ptr<tucan_entity>>& tokens,
			std::shared_ptr<tucan_executable_container> container,
			std::shared_ptr<tucan_returnable> lastReturnable,
			std::shared_ptr<tucan_loop> lastLoop);

		std::vector<std::shared_ptr<tucan_entity>> collect_internal_tokens(
			std::vector<std::shared_ptr<tucan_entity>>& tokens,
			size_t index,
			size_t& out_index);

		std::shared_ptr<tucan_expression> process_expression(
			std::shared_ptr<tucan_executable_container> parent,
			std::vector<std::shared_ptr<tucan_entity>>& tokens,
			size_t index,
			size_t& out_index);

		std::shared_ptr<tucan_operable> process_variable(const std::string& name, std::shared_ptr<tucan_executable_container> parent);

		std::shared_ptr<tucan_function_call> process_function_call(
			std::shared_ptr<tucan_executable_container> parent,
			std::shared_ptr<tucan_function>& function,
			std::vector<std::shared_ptr<tucan_entity>>& tokens,
			size_t& index);

	public:
		tucan_module();

		void load_external_executable(const std::string& name, const std::function<void(tucan_function&)>& del);
		void load_from_source(const std::string& src);

		std::shared_ptr<tucan_operable> tryGetVariable(const std::string& name);
		void setVariable(const std::string& name, std::shared_ptr<tucan_operable> variable);

		std::shared_ptr<tucan_function> tryGetFunction(const std::string& name);
		void setFunction(const std::string& name, std::shared_ptr<tucan_function> variable);

		void execute() override;
		void reset() override;
		void return_value() override;
	};
}

