#pragma once
#include "tucan_const.h"
#include <unordered_map>

namespace tucan_script
{
    const char QUOTE_CHAR = '"';
    const char SHARP_CHAR = '#';

    bool isOperator(TUCAN_TYPE type);

    bool isSingleCharToken(char src, TUCAN_TYPE& type);

    bool isReservedToken(const std::string& src, TUCAN_TYPE& type);

    std::vector<std::shared_ptr<tucan_entity>> tokenize(const std::string& src);

    std::shared_ptr<tucan_entity> parseToken(const std::string& src);

    template<typename T>
    bool h_tryGetValue(const std::unordered_map<T, TUCAN_TYPE>& map, const T& src, TUCAN_TYPE& type);

    bool h_tryParseBool(const std::string& str, bool& value);

    bool h_tryParseLongLong(const std::string& str, long long& value);

    bool h_tryParseDouble(const std::string& str, double& value);
}



