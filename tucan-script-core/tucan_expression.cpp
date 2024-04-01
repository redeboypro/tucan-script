#include "tucan_expression.h"

namespace tucan_script
{
	int tucan_expression::precedence(const TUCAN_TYPE& type)
	{
		switch (type)
		{
		case TUCAN_TYPE::AND:
		case TUCAN_TYPE::OR:
			return 1;
		case TUCAN_TYPE::DOUBLE_EQUAL:
		case TUCAN_TYPE::GEQUAL:
		case TUCAN_TYPE::LEQUAL:
		case TUCAN_TYPE::GREATER:
		case TUCAN_TYPE::LESS:
			return 2;
		case TUCAN_TYPE::PLUS:
		case TUCAN_TYPE::MINUS:
			return 3;
		case TUCAN_TYPE::MUL:
		case TUCAN_TYPE::DIV:
		case TUCAN_TYPE::PERCENT:
			return 4;
		default:
			return 0;
		}
	}

	tucan_expression::tucan_expression() {}

	void tucan_expression::appendOperable(std::shared_ptr<tucan_operable> token)
	{
		m_tokens.push_back(std::make_shared<tucan_operable_clone>(token));
	}

	void tucan_expression::append(std::shared_ptr<tucan_entity> token)
	{
		m_tokens.push_back(token);
	}

	void tucan_expression::execute()
	{
		reset();

		std::stack<std::shared_ptr<tucan_entity>> tokenStack;
		std::deque<std::shared_ptr<tucan_entity>> result;

		for (auto& token : m_tokens)
		{
			TUCAN_TYPE type = token->getType();

			switch (type)
			{
			case TUCAN_TYPE::STRING:
			case TUCAN_TYPE::INTEGER:
			case TUCAN_TYPE::DOUBLE:
			case TUCAN_TYPE::BOOLEAN:
			case TUCAN_TYPE::ARRAY:
				result.push_back(token);
				break;
			case TUCAN_TYPE::CALL:
			{
				auto callToken = std::dynamic_pointer_cast<tucan_function_call>(token);
				result.push_back(callToken->invoke());
				break;
			}
			default:
			{
				if (isOperator(type))
				{
					while (tokenStack.size() > 0 &&
						isOperator(tokenStack.top()->getType()) &&
						precedence(type) <= precedence(tokenStack.top()->getType()))
					{
						result.push_back(tokenStack.top());
						tokenStack.pop();
					}

					tokenStack.push(token);
				}
				else
					switch (token->getType())
					{
					case TUCAN_TYPE::LPAREN:
						tokenStack.push(token);
						break;
					case TUCAN_TYPE::RPAREN:
					{
						while (tokenStack.size() > 0 && tokenStack.top()->getType() != TUCAN_TYPE::LPAREN)
						{
							result.push_back(tokenStack.top());
							tokenStack.pop();
						}

						if (tokenStack.size() == 0)
							std::cerr << "Mismatched parentheses!" << std::endl;

						tokenStack.pop();
						break;
					}
					}
			}
			}
		}

		while (tokenStack.size() > 0)
		{
			auto& top = tokenStack.top();
			TUCAN_TYPE topType = top->getType();

			if (topType == TUCAN_TYPE::LPAREN ||
				topType == TUCAN_TYPE::RPAREN)
				std::cerr << "Mismatched parentheses!" << std::endl;

			result.push_back(top);
			tokenStack.pop();
		}

		std::stack<std::shared_ptr<tucan_entity>> valueStack;
		while (!result.empty())
		{
			std::shared_ptr<tucan_entity> nextToken = result.front();
			result.pop_front();

			TUCAN_TYPE nextType = nextToken->getType();

			if (nextType == TUCAN_TYPE::STRING ||
				nextType == TUCAN_TYPE::INTEGER ||
				nextType == TUCAN_TYPE::DOUBLE ||
				nextType == TUCAN_TYPE::BOOLEAN ||
				nextType == TUCAN_TYPE::ARRAY)
				valueStack.push(nextToken);
			else if (isOperator(nextType))
			{
				auto rValue = std::dynamic_pointer_cast<tucan_operable>(valueStack.top());
				valueStack.pop();

				auto lValue = std::dynamic_pointer_cast<tucan_operable>(valueStack.top());
				valueStack.pop();

				auto operationType = nextType;

				switch (operationType)
				{
				case TUCAN_TYPE::PLUS:
					lValue->add(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::MINUS:
					lValue->sub(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::MUL:
					lValue->mul(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::PERCENT:
					lValue->rem(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::DIV:
					lValue->div(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::PLUS_EQUAL:
					lValue->add(*rValue);
					std::dynamic_pointer_cast<tucan_operable_clone>(lValue)->apply();
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::MINUS_EQUAL:
					lValue->sub(*rValue);
					std::dynamic_pointer_cast<tucan_operable_clone>(lValue)->apply();
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::MUL_EQUAL:
					lValue->mul(*rValue);
					std::dynamic_pointer_cast<tucan_operable_clone>(lValue)->apply();
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::DIV_EQUAL:
					lValue->div(*rValue);
					std::dynamic_pointer_cast<tucan_operable_clone>(lValue)->apply();
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::EQUAL:
					lValue->set(*rValue);
					std::dynamic_pointer_cast<tucan_operable_clone>(lValue)->apply();
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::DOUBLE_EQUAL:
					lValue->equal(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::GREATER:
					lValue->greater(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::LESS:
					lValue->less(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::GEQUAL:
					lValue->gEqual(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::LEQUAL:
					lValue->lEqual(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::AND:
					lValue->conjunction(*rValue);
					valueStack.push(lValue);
					break;
				case TUCAN_TYPE::OR:
					lValue->disjunction(*rValue);
					valueStack.push(lValue);
					break;
				}
			}
		}

		if (valueStack.size() != 1)
			std::cerr << "Invalid expression" << std::endl;

		std::shared_ptr<tucan_entity> topValue = valueStack.top();
		set(*std::dynamic_pointer_cast<tucan_operable>(topValue));
		valueStack.pop();
	}

	void tucan_expression::reset()
	{
		for (auto& token : m_tokens)
			if (auto resetable = std::dynamic_pointer_cast<tucan_resetable>(token))
				resetable->reset();
	}
}