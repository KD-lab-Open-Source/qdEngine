#pragma once

/// ������������ TGA ���� � QDA ������.
class ConvertTGAtoQDA
{
public:
	ConvertTGAtoQDA(void);
	~ConvertTGAtoQDA(void);
	/// ��������� �����������. 
	/**
		\retval - ��� ����������� �����. ������ ������, ���� �� �������
	 */
	CString const convert(CString const& strFileName) const;
};
