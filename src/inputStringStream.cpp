#include "inputStringStream.h"

inputStringStream::inputStringStream(std::istream& istr)
{
	std::string inputLine;
	std::getline(istr, inputLine);

	m_iss = std::istringstream(inputLine);
	m_iss.exceptions(std::ios::failbit | std::ios::badbit);
}
//=================================
int inputStringStream::getInt(const std::string error,const int min, const int max)
{
	int i = 0;
	m_iss >> i;
	if (i < min || i > max)
	{
		throw std::invalid_argument(error);
	}
	return i;
}
//=================================
std::string inputStringStream::getString()
{
	std::string str;
	m_iss >> str;
	return str;
}
//=================================
void inputStringStream::checkEndOfInput()
{
	if (m_iss.eof())
		return;

	m_iss >> std::ws;
	if (!m_iss.eof())
		throw std::invalid_argument("Too many characters");
}
