#pragma once

/**
	»нтерфейс предоставл€ет возможность получить у класса 
	или назначить ему некую строку 
 */
interface INameSetter
{
	virtual const CString& get_string() const = 0;
	virtual void set_string(const CString& str) = 0;
};

/**
	ѕозвол€ет получить у класса, реализующего данный интерфейс, уникальную строку
	построеную на основе текущей
 */
interface INameTester
{
	virtual const CString get_unique_name(const CString& test) const = 0;
};