#include "token_util.h"

namespace tucan_script
{
	static std::unordered_map<char, TUCAN_TYPE> single_token_map =
	{
		{ '=', TUCAN_TYPE::EQUAL },
		{ '*', TUCAN_TYPE::MUL },
		{ '/', TUCAN_TYPE::DIV },
		{ '%', TUCAN_TYPE::PERCENT },
		{ '+', TUCAN_TYPE::PLUS },
		{ '-', TUCAN_TYPE::MINUS },
		{ '>', TUCAN_TYPE::GREATER },
		{ '<', TUCAN_TYPE::LESS },
		{ '(', TUCAN_TYPE::LPAREN },
		{ ')', TUCAN_TYPE::RPAREN },
		{ '{', TUCAN_TYPE::LBLOCK },
		{ '}', TUCAN_TYPE::RBLOCK },
		{ ';', TUCAN_TYPE::SEMICOLON },
		{ ',', TUCAN_TYPE::COMMA },
		{ '&', TUCAN_TYPE::REF }
	};

	static std::unordered_map<std::string, TUCAN_TYPE> reserved_token_map =
	{
		{ "and", TUCAN_TYPE::AND },
		{ "or", TUCAN_TYPE::OR },
		{ "if", TUCAN_TYPE::IF },
		{ "while", TUCAN_TYPE::WHILE },
		{ "def", TUCAN_TYPE::DEF },
		{ "imp", TUCAN_TYPE::IMP },
		{ "for", TUCAN_TYPE::FOR },
		{ "in", TUCAN_TYPE::INSIDE },
		{ "break", TUCAN_TYPE::BREAK },
		{ "continue", TUCAN_TYPE::CONTINUE },
		{ "return", TUCAN_TYPE::RETURN }
	};

	bool isOperator(TUCAN_TYPE type)
	{
		return type >= TUCAN_TYPE::EQUAL && type <= TUCAN_TYPE::OR;
	}

	bool isSingleCharToken(char src, TUCAN_TYPE& type)
	{
		return h_tryGetValue<char>(single_token_map, src, type);
	}

	bool isReservedToken(const std::string& src, TUCAN_TYPE& type)
	{
		return h_tryGetValue<std::string>(reserved_token_map, src, type);
	}

	std::vector<std::shared_ptr<tucan_entity>> tokenize(const std::string& src)
	{
		TUCAN_TYPE tokenType = TUCAN_TYPE::NONE;
		TUCAN_TYPE nextTokenType = TUCAN_TYPE::NONE;
		std::string tokenString;
		std::vector<std::shared_ptr<tucan_entity>> tokenList;

		for (size_t i = 0; i < src.size(); i++)
		{
			char tokenChar = src[i];

			if (tokenChar == SHARP_CHAR)
			{
				while (tokenChar != '\n' && tokenChar != '\r')
					tokenChar = src[++i];

				continue;
			}

			if (std::isspace(tokenChar))
			{
				if (tokenString.size() > 0)
					tokenList.push_back(parseToken(tokenString));

				tokenString.clear();
				continue;
			}

			if (isSingleCharToken(tokenChar, tokenType))
			{
				if (tokenString.size() > 0)
					tokenList.push_back(parseToken(tokenString));

				tokenString.clear();

				if (i < src.size() - 1)
				{
					char nextTokenChar = src[i + 1];
					if (isSingleCharToken(nextTokenChar, nextTokenType) && nextTokenType == TUCAN_TYPE::EQUAL)
					{
						switch (tokenType)
						{
						case tucan_script::TUCAN_TYPE::EQUAL:
							tokenList.push_back(std::make_unique<tucan_entity>(TUCAN_TYPE::DOUBLE_EQUAL));
							break;
						case tucan_script::TUCAN_TYPE::GREATER:
							tokenList.push_back(std::make_unique<tucan_entity>(TUCAN_TYPE::GEQUAL));
							break;
						case tucan_script::TUCAN_TYPE::LESS:
							tokenList.push_back(std::make_unique<tucan_entity>(TUCAN_TYPE::EQUAL));
							break;
						case tucan_script::TUCAN_TYPE::PLUS:
							tokenList.push_back(std::make_unique<tucan_entity>(TUCAN_TYPE::PLUS_EQUAL));
							break;
						case tucan_script::TUCAN_TYPE::MINUS:
							tokenList.push_back(std::make_unique<tucan_entity>(TUCAN_TYPE::MINUS_EQUAL));
							break;
						case tucan_script::TUCAN_TYPE::MUL:
							tokenList.push_back(std::make_unique<tucan_entity>(TUCAN_TYPE::MUL_EQUAL));
							break;
						case tucan_script::TUCAN_TYPE::DIV:
							tokenList.push_back(std::make_unique<tucan_entity>(TUCAN_TYPE::DIV_EQUAL));
							break;
						}

						i++;
						continue;
					}
				}

				tokenList.push_back(std::make_unique<tucan_entity>(tokenType));
				continue;
			}

			if (tokenChar == QUOTE_CHAR)
			{
				i++;
				while (src[i] != QUOTE_CHAR)
				{
					tokenString += src[i];
					i++;
				}
				tokenList.push_back(std::make_unique<tucan_const>(tokenString));
				tokenString.clear();
				continue;
			}

			if (!std::iscntrl(static_cast<unsigned char>(tokenChar)))
				tokenString += tokenChar;
		}

		if (tokenString.size() > 0)
			tokenList.push_back(parseToken(tokenString));

		return tokenList;
	}

	std::shared_ptr<tucan_entity> parseToken(const std::string& src)
	{
		bool booleanValue;
		if (h_tryParseBool(src, booleanValue))
			return std::make_unique<tucan_const>(booleanValue);

		long long intValue;
		if (h_tryParseLongLong(src, intValue))
			return std::make_unique<tucan_const>(intValue);

		double floatValue;
		if (h_tryParseDouble(src, floatValue))
			return std::make_unique<tucan_const>(floatValue);

		TUCAN_TYPE type;
		if (isReservedToken(src, type))
			return std::make_unique<tucan_entity>(type);

		return std::make_unique<tucan_undefined>(src);
	}

	bool h_tryParseBool(const std::string& str, bool& value)
	{
		std::istringstream iss(str);
		iss >> std::boolalpha >> value;
		return !iss.fail();
	}

	bool h_tryParseLongLong(const std::string& str, long long& value)
	{
		if (str.find('.') != std::string::npos) {
			return false;
		}

		std::istringstream iss(str);
		iss >> std::boolalpha >> value;
		return !iss.fail();
	}

	bool h_tryParseDouble(const std::string& str, double& value)
	{
		std::istringstream iss(str);
		iss >> value;
		return !iss.fail();
	}

	template<typename T>
	bool h_tryGetValue(const std::unordered_map<T, TUCAN_TYPE>& map, const T& src, TUCAN_TYPE& type)
	{
		auto it = map.find(src);
		if (it != map.end()) {
			type = it->second;
			return true;
		}

		type = TUCAN_TYPE::NONE;
		return false;
	}
}