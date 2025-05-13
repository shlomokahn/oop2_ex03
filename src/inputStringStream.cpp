#include "inputStringStream.h"

inputStringStream::inputStringStream(std::istream& istr)
{
	std::getline(istr, m_inputLine);

	m_iss = std::istringstream(m_inputLine);
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
char inputStringStream::getChar()
{
	char ch;
	m_iss >> ch;
	return ch;
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
std::string inputStringStream::m_inputLine;

