#ifndef _UTILITIES_HPP
#define _UTILITIES_HPP
#include <boost/date_time/gregorian/gregorian.hpp>
#include <map>
#include <string>
#include "core.hpp"

namespace psk
{
  namespace utility
  {
    double DF_Interpolator(std::map<boost::gregorian::date,double>& disc_factors , boost::gregorian::date _today,boost::gregorian::date maturity);

  }
  namespace pricefunction
  {
   
  }
}



#endif
