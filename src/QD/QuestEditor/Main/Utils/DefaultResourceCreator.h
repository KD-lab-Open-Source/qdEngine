#pragma once

/// Создает ресурсы необходимые по умолчанию для проекта
class DefaultResourceCreator
{
	DefaultResourceCreator(void);
	~DefaultResourceCreator(void);
public:
	static bool create();
};
