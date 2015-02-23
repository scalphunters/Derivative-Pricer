#ifndef _MARKET_DATA_HPP
#define _MARKET_DATA_HPP

#include<boost/shared_ptr.hpp>
#include<boost/date_time/gregorian/gregorian.hpp>
#include<vector>
#include<map>
#include<string>

#include "core.hpp"
#include "convention.hpp"

namespace psk
{
  namespace marketdata
  {

    struct _RateStruct{
      std::string name;
      double rate;
      std::string start; // ex tom, spot, ... or date
      std::string tenor; // ex) o/n, tom, spot, 1W, 2W, 1M, 2M, 1Y, 2Y, ... or date
      boost::shared_ptr<psk::staticdata::RateConvention> rateconvention;
    };  // Rate object definition

    typedef _RateStruct Rate;
    
    class MarketData : public psk::core::BaseObject
    {

    private:
      std::map<std::string, boost::shared_ptr<Rate> > marketdataset; //market data storage
      std::map<std::string, boost::shared_ptr<psk::staticdata::RateConvention> > rateconvention; // rateconvention data
      boost::gregorian::date _today; // snapshot date

    public: 
      void addRate(boost::shared_ptr<Rate>); // add rate object
      void addRateConvention(boost::shared_ptr<psk::staticdata::RateConvention>);
      boost::shared_ptr<psk::staticdata::RateConvention> getRateConvention(std::string);
      boost::shared_ptr<Rate> getRate(std::string ticker); //get rate object
      void setRate(std::string ticker, double rate);// set existing ticker rate , if not exist then create)

      MarketData();
      ~MarketData();
    }; 
  }// end of mds namespace


}//end of psk
#endif
