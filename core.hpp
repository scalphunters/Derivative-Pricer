#ifndef _CORE_HPP
#define _CORE_HPP

#include<map>
#include<boost/date_time/gregorian/gregorian.hpp>
#include<boost/variant.hpp>
#include<string>

const bool _DEBUG_FLAG = true; 

namespace psk
{
  namespace core
  {
    class BaseObject // mother of all classes
    {
    protected:

    public:
      BaseObject(){};
      ~BaseObject(){};
    };

    typedef std::map<boost::gregorian::date,double> Cashflow;
    typedef std::map<boost::gregorian::date,double> DiscountFactors;

    typedef std::map<std::string, boost::variant<int,double,std::string> > Parameter; //input for functors
    typedef std::map<std::string, boost::variant<int,double,std::string> > Result; // output from functors


}// end of core namespace

  
}


#endif
