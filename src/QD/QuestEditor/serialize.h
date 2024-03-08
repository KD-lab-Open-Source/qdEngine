#pragma once
#include <iosfwd>

/// ��������� � ����� RECT
std::ostream& operator<<(std::ostream& out, RECT const& rect);
/// �������� �� ������ RECT
std::istream& operator>>(std::istream& in, RECT& rect);

/// ��������� � ����� ���� std::pair<std::string, CRect>
std::ostream& operator<<(std::ostream& out, std::pair<std::string, CRect> const& value);
/// �������� �� ������ ����std::pair<std::string, CRect>
std::istream& operator>>(std::istream& in, std::pair<std::string, CRect>& value);