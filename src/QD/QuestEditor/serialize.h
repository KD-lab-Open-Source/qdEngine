#pragma once
#include <iosfwd>

/// сохраняет в поток RECT
std::ostream& operator<<(std::ostream& out, RECT const& rect);
/// считвает из потока RECT
std::istream& operator>>(std::istream& in, RECT& rect);

/// сохраняет в поток пару std::pair<std::string, CRect>
std::ostream& operator<<(std::ostream& out, std::pair<std::string, CRect> const& value);
/// считвает из потока паруstd::pair<std::string, CRect>
std::istream& operator>>(std::istream& in, std::pair<std::string, CRect>& value);