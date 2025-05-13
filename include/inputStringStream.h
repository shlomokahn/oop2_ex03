#include <string>  
#include <sstream>   

class inputStringStream  
{  
public:  
   inputStringStream(std::istream& istr);
   ~inputStringStream();  

private:  
   std::istringstream m_iss; 

};