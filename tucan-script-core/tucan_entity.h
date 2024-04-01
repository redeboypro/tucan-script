#pragma once
#include <string>
#include <vector>
#include <memory>

namespace tucan_script
{
	enum class TUCAN_TYPE
	{
		UNDEFINED = -2,
		NONE = -1,

		STRING = 0,
		INTEGER = 1,
		DOUBLE = 2,
		BOOLEAN = 3,

		LPAREN = 4,
		RPAREN = 5,

		EQUAL = 6,
		DOUBLE_EQUAL = 7,
		GEQUAL = 8,
		LEQUAL = 9,
		GREATER = 10,
		LESS = 11,
		PLUS_EQUAL = 12,
		MINUS_EQUAL = 13,
		MUL_EQUAL = 14,
		DIV_EQUAL = 15,
		PLUS = 18,
		MINUS = 19,
		PERCENT = 20,
		MUL = 21,
		DIV = 22,

		AND = 23,
		OR = 24,

		IF = 25,
		WHILE = 26,

		DEF = 27,
		IMP = 28,

		SEMICOLON = 29,
		COMMA = 30,

		LBLOCK = 31,
		RBLOCK = 32,

		BREAK = 33,
		RETURN = 34,
		CALL = 35,
		REF = 36,

		ARRAY = 37,

		FOR = 38,
		INSIDE = 39,

		CONTINUE = 40
	};

	class tucan_loop
	{
	public:
		virtual void break_loop() = 0;
		virtual void continue_loop() = 0;
	};

	class tucan_resetable
	{
	public:
		virtual ~tucan_resetable() = default;

		virtual void reset() = 0;
	};

	class tucan_executable : public tucan_resetable
	{
	public:
		virtual ~tucan_executable() = default;

		virtual void execute() = 0;
	};

	class tucan_executable_container
	{
	public:
		std::vector<std::shared_ptr<tucan_executable>> executables;

		virtual ~tucan_executable_container() = default;
	};

	class tucan_returnable : public tucan_executable_container
	{
	public:
		virtual void return_value() = 0;
	};

	class tucan_break_point : public tucan_executable
	{
	private:
		std::shared_ptr<tucan_loop> m_loop;

	public:
		tucan_break_point(std::shared_ptr<tucan_loop> loop);
		void execute() override;
		void reset() override {}
	};

	class tucan_continue_point : public tucan_executable
	{
	private:
		std::shared_ptr<tucan_loop> m_loop;

	public:
		tucan_continue_point(std::shared_ptr<tucan_loop> loop);
		void execute() override;
		void reset() override {}
	};

	class tucan_return_point : public tucan_executable
	{
	private:
		std::shared_ptr<tucan_returnable> m_returnable;

	public:
		tucan_return_point(std::shared_ptr<tucan_returnable> returnable);
		void execute() override;
		void reset() override {}
	};

	class tucan_statement : public tucan_executable, public tucan_executable_container
	{
	public:
		void reset() override;
		void append(std::shared_ptr<tucan_executable>& executable);

		virtual void execute() override = 0;
	};

	class tucan_entity
	{
	private:
		TUCAN_TYPE m_type;

	public:
		tucan_entity();
		tucan_entity(const TUCAN_TYPE& type);
		virtual ~tucan_entity() = default;

		TUCAN_TYPE getType() const;

	protected:
		void setType(const TUCAN_TYPE& type);
	};

	class tucan_undefined : public tucan_entity
	{
	private:
		std::string m_content;

	public:
		tucan_undefined(const std::string& content);
		~tucan_undefined() = default;

		std::string getContent() const;
	};
}
