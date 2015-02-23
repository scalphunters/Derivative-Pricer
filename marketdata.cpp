#include "marketdata.hpp"
#include<math.h>
using namespace psk::marketdata;
using namespace boost;
using namespace std;
using namespace boost::gregorian;


/// MarketData class

void MarketData::setRate(string ticker, double rate)
{
  if (marketdataset.find(ticker)!=marketdataset.end()) //if ticker exists
    {
      marketdataset[ticker]->rate=rate;
    }
  else //if ticker doesn't exist
    {
      shared_ptr<Rate> tmp(new Rate);
      tmp->name=ticker;
      tmp->rate=rate;
      addRate(tmp);
    }
}
void MarketData::addRate(shared_ptr<Rate> rt)
{
  marketdataset[rt->name]=rt;
}

boost::shared_ptr<Rate> MarketData::getRate(string ticker)
{
  if (marketdataset.find(ticker)!=marketdataset.end()) 
    {
      return marketdataset[ticker];
    }
  else
    {
      return marketdataset["NULL"];
    }
}

void MarketData::addRateConvention(boost::shared_ptr<psk::staticdata::RateConvention> rc)
{
  rateconvention[rc->name]=rc;
}

boost::shared_ptr<psk::staticdata::RateConvention> MarketData::getRateConvention(std::string rcname)
{
  return rateconvention[rcname];
}
MarketData::MarketData() : _today(day_clock::local_day())
{
      boost::shared_ptr<Rate> tmp(new Rate);
      tmp->name="NULL";
      tmp->rate=0.0;
      addRate(tmp);
}

MarketData::~MarketData()
{
}
