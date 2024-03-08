#pragma once

/// Запускает радактор текстовой базы
class RunDialogEditor
{
public:
	RunDialogEditor(void);
	~RunDialogEditor(void);
	bool run(CString const& projFileName);
};
