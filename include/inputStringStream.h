#include <string>  
#include <sstream>   

class inputStringStream  
{  
public:  
   inputStringStream(std::istream& istr);
   ~inputStringStream(){}

   int getInt(const std::string error,const int min = -1024, const int max = 1000);
   std::string getString();
   void checkEndOfInput();

private:  
   std::istringstream m_iss; 

};