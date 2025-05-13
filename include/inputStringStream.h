#pragma once

#include <string>  
#include <sstream>   

class inputStringStream
{
public:
	inputStringStream(std::istream& istr);
	~inputStringStream() {}

	int getInt(const std::string error = ("element out of range"), const int min = -1024, const int max = 1000);
	char getChar();
	std::string getString();
	void checkEndOfInput();
	static std::string getLine() { return m_inputLine; }


private:  
   std::istringstream m_iss; 
   static std::string m_inputLine;
};