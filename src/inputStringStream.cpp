#include "inputStringStream.h"

inputStringStream::inputStringStream(std::istream& istr)
{
	std::string inputLine;
	std::getline(istr, inputLine);

	m_iss = std::istringstream(inputLine);
	m_iss.exceptions(std::ios::failbit | std::ios::badbit);
}
