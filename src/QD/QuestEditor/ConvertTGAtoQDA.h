#pragma once

/// Конвертирует TGA файл в QDA формат.
class ConvertTGAtoQDA
{
public:
	ConvertTGAtoQDA(void);
	~ConvertTGAtoQDA(void);
	/// Выполняет конвертацию. 
	/**
		\retval - имя полученного файла. Пустая строка, если не удалось
	 */
	CString const convert(CString const& strFileName) const;
};
