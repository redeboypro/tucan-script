#include "tucan_operable.h"

namespace tucan_script
{
	tucan_operable::tucan_operable() : m_intValue(0), m_floatValue(0.0), m_boolValue(false)
	{
		setType(TUCAN_TYPE::INTEGER);
	}

	tucan_operable::tucan_operable(const TUCAN_TYPE& type) : m_intValue(0), m_floatValue(0.0), m_boolValue(false)
	{
		setType(type);
	}

	long long tucan_operable::length() const
	{
		switch (getType())
		{
		case TUCAN_TYPE::ARRAY:
			return m_arrayValues.size();
		case TUCAN_TYPE::STRING:
			return m_stringValue.length();
		default:
			return 0;
		}
	}

	const std::string& tucan_operable::getStringValue() const
	{
		return m_stringValue;
	}

	long long tucan_operable::getIntValue() const
	{
		return m_intValue;
	}

	double tucan_operable::getFloatValue() const
	{
		return m_floatValue;
	}

	bool tucan_operable::getBoolValue() const
	{
		return m_boolValue;
	}

	bool tucan_operable::isArray() const
	{
		return getType() == TUCAN_TYPE::ARRAY;
	}

	bool tucan_operable::isString() const
	{
		return getType() == TUCAN_TYPE::STRING;
	}

	bool tucan_operable::isInt() const
	{
		return getType() == TUCAN_TYPE::INTEGER;
	}

	bool tucan_operable::isFloat() const
	{
		return getType() == TUCAN_TYPE::DOUBLE;
	}

	bool tucan_operable::isBoolean() const
	{
		return getType() == TUCAN_TYPE::BOOLEAN;
	}

	void tucan_operable::set(const std::string& rValue)
	{
		set(TUCAN_TYPE::STRING, rValue, 0, 0.0, false);
	}

	void tucan_operable::set(long long rValue)
	{
		set(TUCAN_TYPE::INTEGER, std::string(), rValue, 0.0, false);
	}

	void tucan_operable::set(double rValue)
	{
		set(TUCAN_TYPE::DOUBLE, std::string(), 0, rValue, false);
	}

	void tucan_operable::set(bool rValue)
	{
		set(TUCAN_TYPE::BOOLEAN, std::string(), 0, 0.0, rValue);
	}

	void tucan_operable::set(const tucan_operable& rValue)
	{
		set(rValue.getType(),
			rValue.m_stringValue,
			rValue.m_intValue,
			rValue.m_floatValue,
			rValue.m_boolValue,
			rValue.m_arrayValues);
	}

	void tucan_operable::add(const std::string& rValue)
	{
		set(toString().append(rValue));
	}

	void tucan_operable::add(is_numeric auto rValue)
	{
		if (isFloat())
		{
			set(m_floatValue + rValue);
			return;
		}

		set(toInt() + rValue);
	}

	void tucan_operable::add(const tucan_operable& rValue)
	{
		if (isString())
			add(rValue.toString());
		else
			switch (rValue.getType())
			{
			case TUCAN_TYPE::STRING:
				add(rValue.m_stringValue);
				break;
			case TUCAN_TYPE::INTEGER:
			case TUCAN_TYPE::BOOLEAN:
				add(rValue.toInt());
				break;
			case TUCAN_TYPE::DOUBLE:
				add(rValue.toFloat());
				break;
			}
	}

	void tucan_operable::sub(is_numeric auto rValue)
	{
		if (isFloat())
		{
			set(m_floatValue - rValue);
			return;
		}

		set(toInt() - rValue);
	}

	void tucan_operable::sub(const tucan_operable& rValue)
	{
		if (rValue.isFloat())
		{
			sub(rValue.m_floatValue);
			return;
		}

		sub(rValue.toInt());
	}

	void tucan_operable::mul(is_numeric auto rValue)
	{
		if (isFloat())
		{
			set(m_floatValue * rValue);
			return;
		}

		set(toInt() * rValue);
	}

	void tucan_operable::mul(const tucan_operable& rValue)
	{
		if (rValue.isFloat())
		{
			mul(rValue.m_floatValue);
			return;
		}

		mul(rValue.toInt());
	}

	void tucan_operable::div(is_numeric auto rValue)
	{
		if (isFloat())
		{
			set(m_floatValue / rValue);
			return;
		}

		set(toInt() / rValue);
	}

	void tucan_operable::div(const tucan_operable& rValue)
	{
		if (rValue.isFloat())
		{
			div(rValue.m_floatValue);
			return;
		}

		div(rValue.toInt());
	}

	void tucan_operable::rem(is_numeric auto rValue)
	{
		if (isFloat())
			set(std::fmod(m_floatValue, rValue));
		else if constexpr (std::is_same_v<decltype(rValue), long long>)
			set(toInt() % static_cast<long long>(rValue));
		else
			set(std::fmod(toInt(), rValue));
	}

	void tucan_operable::rem(const tucan_operable& rValue)
	{
		if (rValue.isFloat())
		{
			rem(rValue.m_floatValue);
			return;
		}

		rem(rValue.toInt());
	}

	void tucan_operable::equal(const std::string& rValue)
	{
		set(toString() == rValue);
	}

	void tucan_operable::equal(long long rValue)
	{
		set(toInt() == rValue);
	}

	void tucan_operable::equal(double rValue)
	{
		set(toFloat() == rValue);
	}

	void tucan_operable::equal(bool rValue)
	{
		set(toBoolean() == rValue);
	}

	void tucan_operable::equal(const tucan_operable& rValue)
	{
		switch (rValue.getType())
		{
		case TUCAN_TYPE::STRING:
			equal(rValue.m_stringValue);
			break;
		case TUCAN_TYPE::INTEGER:
			equal(rValue.m_intValue);
			break;
		case TUCAN_TYPE::DOUBLE:
			equal(rValue.m_floatValue);
			break;
		case TUCAN_TYPE::BOOLEAN:
			equal(rValue.m_boolValue);
			break;
		}
	}

	void tucan_operable::greater(is_numeric auto rValue)
	{
		if (isFloat())
		{
			set(m_floatValue > rValue);
			return;
		}

		set(toInt() > rValue);
	}

	void tucan_operable::greater(const tucan_operable& rValue)
	{
		if (rValue.isFloat())
		{
			greater(rValue.m_floatValue);
			return;
		}

		greater(rValue.toInt());
	}

	void tucan_operable::less(is_numeric auto rValue)
	{
		if (isFloat())
		{
			set(m_floatValue < rValue);
			return;
		}

		set(toInt() < rValue);
	}

	void tucan_operable::less(const tucan_operable& rValue)
	{
		if (rValue.isFloat())
		{
			less(rValue.m_floatValue);
			return;
		}

		less(rValue.toInt());
	}

	void tucan_operable::gEqual(is_numeric auto rValue)
	{
		if (isFloat())
		{
			set(m_floatValue >= rValue);
			return;
		}

		set(toInt() >= rValue);
	}

	void tucan_operable::gEqual(const tucan_operable& rValue)
	{
		if (rValue.isFloat())
		{
			gEqual(rValue.m_floatValue);
			return;
		}

		gEqual(rValue.toInt());
	}

	void tucan_operable::lEqual(is_numeric auto rValue)
	{
		if (isFloat())
		{
			set(m_floatValue <= rValue);
			return;
		}

		set(toInt() <= rValue);
	}

	void tucan_operable::lEqual(const tucan_operable& rValue)
	{
		if (rValue.isFloat())
		{
			lEqual(rValue.m_floatValue);
			return;
		}

		lEqual(rValue.toInt());
	}

	void tucan_operable::conjunction(bool rValue)
	{
		set(toBoolean() && rValue);
	}

	void tucan_operable::conjunction(const tucan_operable& rValue)
	{
		conjunction(rValue.toBoolean());
	}

	void tucan_operable::disjunction(bool rValue)
	{
		set(toBoolean() || rValue);
	}

	void tucan_operable::disjunction(const tucan_operable& rValue)
	{
		disjunction(rValue.toBoolean());
	}

	tucan_operable& tucan_operable::getElement(const size_t& index)
	{
		return *m_arrayValues[index];
	}

	void tucan_operable::setElement(const size_t& index, const tucan_operable& value)
	{
		if (!isArray())
			set(TUCAN_TYPE::ARRAY, std::string(), 0, 0.0, false, std::vector<std::shared_ptr<tucan_operable>>());

		size_t fixedSize = length();

		if (index >= fixedSize)
			for (size_t i = 0; i < fixedSize - index + 1; i++)
				m_arrayValues.push_back(std::make_shared<tucan_operable>(TUCAN_TYPE::INTEGER));

		m_arrayValues[index]->set(value);
	}

	std::string tucan_operable::toString() const
	{
		switch (getType())
		{
		case TUCAN_TYPE::STRING:
			return m_stringValue;
		case TUCAN_TYPE::INTEGER:
			return std::to_string(m_intValue);
		case TUCAN_TYPE::DOUBLE:
			return std::to_string(m_floatValue);
		case TUCAN_TYPE::BOOLEAN:
			return std::to_string(m_boolValue);
		case TUCAN_TYPE::ARRAY:
		{
			std::string arrayString;
			arrayString.append("{");
			for (size_t i = 0; i < length(); i++) {
				arrayString.append(m_arrayValues[i]->toString());

				if (i < length() - 1) {
					arrayString.append(", ");
				}
			}
			arrayString.append("}");
			return arrayString;
		}
		}

		return std::string();
	}

	long long tucan_operable::toInt() const
	{
		switch (getType())
		{
		case TUCAN_TYPE::STRING:
			return std::stoi(m_stringValue);
		case TUCAN_TYPE::INTEGER:
			return m_intValue;
		case TUCAN_TYPE::DOUBLE:
			return static_cast<int>(m_floatValue);
		case TUCAN_TYPE::BOOLEAN:
			return static_cast<int>(m_boolValue);
		}

		return 0;
	}

	double tucan_operable::toFloat() const
	{
		switch (getType())
		{
		case TUCAN_TYPE::STRING:
			return std::stod(m_stringValue);
		case TUCAN_TYPE::INTEGER:
			return static_cast<double>(m_intValue);
		case TUCAN_TYPE::DOUBLE:
			return m_floatValue;
		case TUCAN_TYPE::BOOLEAN:
			return static_cast<double>(m_boolValue);
		}

		return 0.0;
	}

	bool tucan_operable::toBoolean() const
	{
		switch (getType())
		{
		case TUCAN_TYPE::STRING:
		{
			bool resultBoolean;
			std::istringstream(m_stringValue) >> std::boolalpha >> resultBoolean;
			return resultBoolean;
		}
		case TUCAN_TYPE::INTEGER:
			return static_cast<bool>(m_intValue);
		case TUCAN_TYPE::DOUBLE:
			return static_cast<bool>(m_floatValue);
		case TUCAN_TYPE::BOOLEAN:
			return m_boolValue;
		}

		return false;
	}

	void tucan_operable::set(
		const TUCAN_TYPE& type,
		const std::string& stringValue,
		const long long& intValue,
		const double& floatValue,
		const bool& boolValue,
		const std::vector<std::shared_ptr<tucan_operable>>& arrayValues)
	{
		set(type, stringValue, intValue, floatValue, boolValue);

		for (size_t i = 0; i < arrayValues.size(); i++)
			setElement(i, *arrayValues[i]);
	}

	void tucan_operable::set(
		const TUCAN_TYPE& type,
		const std::string& stringValue,
		const long long& intValue,
		const double& floatValue,
		const bool& boolValue)
	{
		setType(type);
		m_stringValue = stringValue;
		m_intValue = intValue;
		m_floatValue = floatValue;
		m_boolValue = boolValue;
		m_arrayValues.clear();
	}

	tucan_operable_clone::tucan_operable_clone(std::shared_ptr<tucan_operable> source) : m_source(source) {}

	void tucan_operable_clone::setSource(std::shared_ptr<tucan_operable>& source)
	{
		m_source = source;
	}

	void tucan_operable_clone::reset()
	{
		if (m_source != nullptr)
			set(*m_source);
	}

	void tucan_operable_clone::apply()
	{
		m_source->set(*this);
	}
}