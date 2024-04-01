#include "tucan_module.h"

namespace tucan_script
{
	void tucan_module::process_container(
		std::vector<std::shared_ptr<tucan_entity>>& tokens,
		std::shared_ptr<tucan_executable_container> container,
		std::shared_ptr<tucan_returnable> lastReturnable,
		std::shared_ptr<tucan_loop> lastLoop)
	{
		auto& in_executables = container->executables;

		for (size_t index = 0; index < tokens.size(); index++)
		{
			auto& token = tokens[index];

			auto sub_skip_token = [&]() { token = tokens[++index]; };
			auto sub_get_undefined = [&]() -> std::string { return std::dynamic_pointer_cast<tucan_undefined>(token)->getContent(); };

			switch (token->getType())
			{
			case TUCAN_TYPE::UNDEFINED:
				in_executables.push_back(process_expression(lastReturnable, tokens, index, index));
				break;
			case TUCAN_TYPE::DEF:
			{
				sub_skip_token();
				auto function = std::make_shared<tucan_function>();
				std::string functionName = sub_get_undefined();

				sub_skip_token();
				while (token->getType() != TUCAN_TYPE::SEMICOLON)
				{
					if (token->getType() == TUCAN_TYPE::REF)
					{
						sub_skip_token();
						function->append(sub_get_undefined(), true);
					}
					else
						function->append(sub_get_undefined(), false);

					sub_skip_token();
				}

				m_functions[functionName] = function;
				break;
			}
			case TUCAN_TYPE::IMP:
			{
				sub_skip_token();
				auto& functionStored = m_functions[sub_get_undefined()];
				auto functionInternalTokens = collect_internal_tokens(tokens, index + 1, index);
				process_container(functionInternalTokens, functionStored, functionStored, lastLoop);
				break;
			}
			case TUCAN_TYPE::IF:
			case TUCAN_TYPE::WHILE:
			{
				auto condition = process_expression(lastReturnable, tokens, index + 1, index);
				auto statementInternalTokens = collect_internal_tokens(tokens, index, index);
				if (token->getType() == TUCAN_TYPE::IF)
				{
					auto conditionInstance = std::make_shared<tucan_statement_if>(condition);

					auto statementContainer = std::dynamic_pointer_cast<tucan_executable_container>(conditionInstance);
					process_container(statementInternalTokens, statementContainer, lastReturnable, lastLoop);

					in_executables.push_back(conditionInstance);
					break;
				}

				auto loopInstance = std::make_shared<tucan_statement_while>(condition);
				auto statementContainer = std::dynamic_pointer_cast<tucan_executable_container>(loopInstance);

				process_container(statementInternalTokens, statementContainer, lastReturnable, loopInstance);
				in_executables.push_back(loopInstance);
				break;
			}
			case TUCAN_TYPE::FOR:
			{
				std::vector<std::shared_ptr<tucan_operable>> variables;

				auto& variableToken = tokens[++index];
				while (variableToken->getType() != TUCAN_TYPE::INSIDE)
				{
					if (auto undefinedToken = std::dynamic_pointer_cast<tucan_undefined>(variableToken))
						variables.push_back(process_variable(undefinedToken->getContent(), container));

					variableToken = tokens[++index];
				}

				auto array = process_expression(lastReturnable, tokens, index + 1, index);
				auto statementInternalTokens = collect_internal_tokens(tokens, index, index);

				auto loopInstance = std::make_shared<tucan_statement_for>(variables, array);
				auto statementContainer = std::dynamic_pointer_cast<tucan_executable_container>(loopInstance);

				process_container(statementInternalTokens, statementContainer, lastReturnable, loopInstance);
				in_executables.push_back(loopInstance);
				break;
			}
			case TUCAN_TYPE::RETURN:
				in_executables.push_back(std::make_shared<tucan_return_point>(std::shared_ptr<tucan_returnable>(lastReturnable)));
				break;
			case TUCAN_TYPE::BREAK:
				in_executables.push_back(std::make_shared<tucan_break_point>(std::shared_ptr<tucan_loop>(lastLoop)));
				break;
			case TUCAN_TYPE::CONTINUE:
				in_executables.push_back(std::make_shared<tucan_continue_point>(std::shared_ptr<tucan_loop>(lastLoop)));
				break;
			}
		}
	}

	std::vector<std::shared_ptr<tucan_entity>> tucan_module::collect_internal_tokens(
		std::vector<std::shared_ptr<tucan_entity>>& tokens,
		size_t index,
		size_t& out_index)
	{
		std::vector<std::shared_ptr<tucan_entity>> rawTokenBuffer;
		auto& token = tokens[index];

		if (token->getType() != TUCAN_TYPE::LBLOCK)
			std::cerr << "Unexpected token" << std::endl;

		auto sub_skip_token = [&]() { token = tokens[++index]; };
		sub_skip_token();

		size_t bracketCount = 1;

		if (token->getType() == TUCAN_TYPE::RBLOCK)
			goto end;

		while (bracketCount > 0)
		{
			switch (token->getType())
			{
			case TUCAN_TYPE::LBLOCK:
				bracketCount++;
				break;
			case TUCAN_TYPE::RBLOCK:
			{
				bracketCount--;
				if (bracketCount == 0) continue;
				break;
			}
			}

			rawTokenBuffer.push_back(token);
			sub_skip_token();
		}

	end:
		out_index = index;
		return rawTokenBuffer;
	}

	std::shared_ptr<tucan_expression> tucan_module::process_expression(
		std::shared_ptr<tucan_executable_container> parent,
		std::vector<std::shared_ptr<tucan_entity>>& tokens,
		size_t index,
		size_t& out_index)
	{
		auto expression = std::make_shared<tucan_expression>();
		while (index < tokens.size())
		{
			auto& currentToken = tokens[index];
			TUCAN_TYPE currentType = currentToken->getType();

			if (currentType == TUCAN_TYPE::SEMICOLON ||
				currentType == TUCAN_TYPE::LBLOCK ||
				currentType == TUCAN_TYPE::RBLOCK)
				break;

			if (auto undefinedToken = std::dynamic_pointer_cast<tucan_undefined>(currentToken))
			{
				auto rawContent = undefinedToken->getContent();
				size_t nextIndex = index + 1;
				if (nextIndex < tokens.size() && tokens[nextIndex]->getType() == TUCAN_TYPE::LPAREN)
				{
					index += 2;
					if (auto function = tryGetFunction(rawContent))
						expression->append(process_function_call(parent, function, tokens, index));
					else
						std::cerr << "Function " + rawContent + " not defined" << std::endl;

					continue;
				}

				expression->appendOperable(process_variable(rawContent, parent));
			}
			else
				expression->append(currentToken);

			index++;
		}

		out_index = index;
		return expression;
	}

	std::shared_ptr<tucan_operable> tucan_module::process_variable(
		const std::string& name,
		std::shared_ptr<tucan_executable_container> parent)
	{
		if (auto parentFunction = std::dynamic_pointer_cast<tucan_function>(parent))
			if (auto localVariable = parentFunction->getArgByName(name))
				return std::shared_ptr<tucan_operable>(localVariable);

		if (auto storedVariable = tryGetVariable(name))
			return storedVariable;

		if (auto function = tryGetFunction(name))
			return function;

		auto variable = std::make_shared<tucan_operable>();
		m_variables[name] = variable;
		return variable;
	}

	std::shared_ptr<tucan_function_call> tucan_module::process_function_call(
		std::shared_ptr<tucan_executable_container> parent,
		std::shared_ptr<tucan_function>& function,
		std::vector<std::shared_ptr<tucan_entity>>& tokens,
		size_t& index)
	{
		auto functionCall = std::make_shared<tucan_function_call>(std::shared_ptr<tucan_function>(function));
		std::vector<std::shared_ptr<tucan_entity>> argTokenBuffer;

		size_t placeHolder;
		auto sub_proc_internal_expr = [&](size_t& localIndex)
			{
				if (argTokenBuffer.size() > 1) {
					functionCall->append(process_expression(
						std::dynamic_pointer_cast<tucan_executable_container>(function),
						argTokenBuffer, 0, placeHolder));
				}
				else if (argTokenBuffer.size() == 1)
				{
					auto& singleToken = argTokenBuffer[0];

					if (auto undefinedToken = std::dynamic_pointer_cast<tucan_undefined>(singleToken))
					{
						std::string rawContent = undefinedToken->getContent();
						auto variable = tryGetVariable(rawContent);
						if (!variable)
						{
							if (auto parentFunction = std::dynamic_pointer_cast<tucan_function>(parent))
							{
								std::cout << rawContent << std::endl;
								functionCall->append(parentFunction->getArgByName(rawContent));
								goto end;
							}
							else
								std::cerr << "Unexpected argument " + rawContent << std::endl;
						}


						functionCall->append(variable);
					}
					else if (auto operableToken = std::dynamic_pointer_cast<tucan_operable>(singleToken))
						functionCall->append(operableToken);
				}

			end:
				argTokenBuffer.clear();
				localIndex++;
			};

		size_t bracketCount = 1;
		while (bracketCount > 0)
		{
			auto& currentToken = tokens[index];

			switch (currentToken->getType())
			{
			case TUCAN_TYPE::COMMA:
				if (bracketCount == 1)
				{
					sub_proc_internal_expr(index);
					continue;
				}
				break;
			case TUCAN_TYPE::LPAREN:
				bracketCount++;
				break;
			case TUCAN_TYPE::RPAREN:
			{
				bracketCount--;
				if (bracketCount == 0)
				{
					sub_proc_internal_expr(index);
					continue;
				}
				break;
			}
			}

			argTokenBuffer.push_back(currentToken);
			index++;
		}

		return functionCall;
	}

	tucan_module::tucan_module() : m_running(false)
	{
		// Input / Output
		load_external_executable("print", [](tucan_function& function)
			{
				for (size_t index = 0; index < function.involvedArgumentCount; index++)
					std::cout << function.getArgById(index)->toString();
			});

		load_external_executable("println", [](tucan_function& function)
			{
				for (size_t index = 0; index < function.involvedArgumentCount; index++)
					std::cout << function.getArgById(index)->toString() << std::endl;
			});

		load_external_executable("input", [](tucan_function& function)
			{
				std::string inputString;

				for (size_t index = 0; index < function.involvedArgumentCount; index++)
					std::cout << function.getArgById(index)->toString();

				std::cin >> inputString;
				function.set(inputString);
			});

		// Casting
		load_external_executable("string", [](tucan_function& function)
			{
				function.set(function.getArgById(0)->toString());
			});

		load_external_executable("int", [](tucan_function& function)
			{
				function.set(function.getArgById(0)->toInt());
			});

		load_external_executable("float", [](tucan_function& function)
			{
				function.set(function.getArgById(0)->toFloat());
			});

		load_external_executable("bool", [](tucan_function& function)
			{
				function.set(function.getArgById(0)->toBoolean());
			});

		// Math
		load_external_executable("sin", [](tucan_function& function)
			{
				function.set(std::sin(function.getArgById(0)->toFloat()));
			});

		load_external_executable("cos", [](tucan_function& function)
			{
				function.set(std::cos(function.getArgById(0)->toFloat()));
			});

		load_external_executable("tan", [](tucan_function& function)
			{
				function.set(std::tan(function.getArgById(0)->toFloat()));
			});

		load_external_executable("asin", [](tucan_function& function)
			{
				function.set(std::asin(function.getArgById(0)->toFloat()));
			});

		load_external_executable("acos", [](tucan_function& function)
			{
				function.set(std::acos(function.getArgById(0)->toFloat()));
			});

		load_external_executable("atan", [](tucan_function& function)
			{
				function.set(std::atan(function.getArgById(0)->toFloat()));
			});

		load_external_executable("atan2", [](tucan_function& function)
			{
				function.set(std::atan2(
					function.getArgById(0)->toFloat(),
					function.getArgById(1)->toFloat()));
			});

		load_external_executable("pow", [](tucan_function& function)
			{
				function.set(std::pow(
					function.getArgById(0)->toFloat(),
					function.getArgById(1)->toFloat()));
			});

		load_external_executable("log", [](tucan_function& function)
			{
				function.set(std::log(function.getArgById(0)->toFloat()));
			});

		load_external_executable("log10", [](tucan_function& function)
			{
				function.set(std::log(function.getArgById(0)->toFloat()));
			});


		// Containers
		load_external_executable("vector", [](tucan_function& function)
			{
				for (size_t index = 0; index < function.involvedArgumentCount; index++)
					function.setElement(index, *function.getArgById(index));
			});

		load_external_executable("len", [](tucan_function& function)
			{
				function.set(function.getArgById(0)->length());
			});

		load_external_executable("get", [](tucan_function& function)
			{
				auto vector = function.getArgById(0);
				auto index = function.getArgById(1);

				TUCAN_TYPE type = vector->getType();

				if (type == TUCAN_TYPE::ARRAY)
					function.set(vector->getElement(index->toInt()));
				else if (type == TUCAN_TYPE::STRING)
					function.set(std::string(1, vector->getStringValue()[index->toInt()]));
			});

		m_functions["get"]->append("in_vector", true);

		load_external_executable("set", [](tucan_function& function)
			{
				auto vector = function.getArgById(0);
				auto index = function.getArgById(1);
				auto value = function.getArgById(2);
				vector->setElement(index->toInt(), *value);
			});

		m_functions["set"]->append("in_vector", true);

		// String
		setVariable("str_npos", std::make_shared<tucan_const>(static_cast<long long>(std::string::npos)));

		load_external_executable("substr", [](tucan_function& function)
			{
				auto str = function.getArgById(0);
				auto pos = function.getArgById(1);
				auto len = function.getArgById(2);

				function.set(str->toString().substr(pos->toInt(), len->toInt()));
			});

		m_functions["substr"]->append("str", true);

		load_external_executable("str_find", [](tucan_function& function)
			{
				auto str = function.getArgById(0);
				auto des = function.getArgById(1);

				function.set(static_cast<long long>(str->toString().find(des->toString())));
			});
	}

	void tucan_module::load_external_executable(const std::string& name, const std::function<void(tucan_function&)>& del)
	{
		auto function = std::make_shared<tucan_function>();

		auto executable = std::make_shared<tucan_external_executable>(function);
		executable->del_action = del;

		function->append(executable);

		setFunction(name, function);
	}

	void tucan_module::load_from_source(const std::string& src)
	{
		auto rawTokens = tokenize(src);
		auto this_as_shared = shared_from_this();
		process_container(rawTokens, this_as_shared, this_as_shared, nullptr);
	}

	std::shared_ptr<tucan_operable> tucan_module::tryGetVariable(const std::string& name)
	{
		auto it = m_variables.find(name);

		if (it != m_variables.end())
			return it->second;

		return nullptr;
	}

	void tucan_module::setVariable(const std::string& name, std::shared_ptr<tucan_operable> variable)
	{
		if (auto stored_variable = tryGetVariable(name))
		{
			stored_variable->set(*variable);
			return;
		}

		m_variables[name] = variable;
	}

	std::shared_ptr<tucan_function> tucan_module::tryGetFunction(const std::string& name)
	{
		auto it = m_functions.find(name);

		if (it != m_functions.end())
			return it->second;

		return nullptr;
	}

	void tucan_module::setFunction(const std::string& name, std::shared_ptr<tucan_function> function)
	{
		m_functions[name] = function;
	}

	void tucan_module::execute()
	{
		m_running = true;
		for (auto& executable : executables)
		{
			executable->execute();
			if (!m_running) break;
		}
	}

	void tucan_module::reset()
	{
		for (auto& executable : executables)
			executable->reset();

		for (auto& function : m_functions)
			function.second->reset();
	}

	void tucan_module::return_value()
	{
		m_running = false;
	}
}