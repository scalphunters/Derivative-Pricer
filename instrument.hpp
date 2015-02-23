#ifndef _INSTRUMENT_HPP
#define _INSTRUMENT_HPP
#include <string>
#include<boost/shared_ptr.hpp>
#include "core.hpp"
#include "convention.hpp"
#include "marketdata.hpp"

namespace psk
{

 
  namespace instrument
  {
    class Instrument : public psk::core::BaseObject
    {
    protected:
      boost::gregorian::date pricing_date;
      boost::shared_ptr<psk::staticdata::RateConvention> rateconvention;
      boost::shared_ptr<psk::staticdata::StaticData> staticdata; 
      std::map<std::string,std::string> _Params;
      boost::shared_ptr<psk::marketdata::Rate> spot_reference;
      std::string ref_curve_name; //reference curve's name (USD STD)
      std::string ref_curve_ccy; //reference curve quote currency
      boost::shared_ptr<psk::core::DiscountFactors> reference_curve; //for fx base curve usually USD std curve.
    public:
      virtual double _price(std::map<boost::gregorian::date,double>& dfs,boost::gregorian::date start,boost::gregorian::date maturity);
      virtual double operator()(std::map<boost::gregorian::date,double>& dfs,boost::gregorian::date start,boost::gregorian::date maturity);
      virtual double _solve(psk::core::DiscountFactors &df, boost::gregorian::date start,boost::gregorian::date maturity, double targetvalue);
      virtual void _register_DiscountFactors(boost::shared_ptr<psk::core::DiscountFactors> df){reference_curve=df;}

      void setPricingDate(boost::gregorian::date d1);
      Instrument();
      ~Instrument();
    };//end of Instrument class

    class Depo_Instrument : public Instrument
    {
    private:
      
    public:
      Depo_Instrument();
      Depo_Instrument(boost::shared_ptr<psk::staticdata::RateConvention> rc,boost::shared_ptr<psk::staticdata::StaticData> sd);
      ~Depo_Instrument();
      virtual double _price(std::map<boost::gregorian::date,double>& dfs,boost::gregorian::date,boost::gregorian::date);
    };//end of deposit intrument

    class IRS_Instrument : public Instrument //Interest rate swap
    {
    private:

    public:
      IRS_Instrument();
      ~IRS_Instrument();
      IRS_Instrument(boost::shared_ptr<psk::staticdata::RateConvention> rc, boost::shared_ptr<psk::staticdata::StaticData> sd);
      virtual double _price(std::map<boost::gregorian::date,double>& dfs,boost::gregorian::date,boost::gregorian::date);
    };

    class Swappoint_Instrument : public Instrument // FX Swap Point
    {
    private:
      
    public:
      Swappoint_Instrument(){};
      ~Swappoint_Instrument(){};
      Swappoint_Instrument(boost::shared_ptr<psk::staticdata::RateConvention> rc, boost::shared_ptr<psk::staticdata::StaticData> sd,boost::shared_ptr<psk::core::DiscountFactors> ref_curve,boost::shared_ptr<psk::marketdata::Rate> spot_ref,std::string ref_curve_currency,std::string ref_curve_name);
      virtual double _price(std::map<boost::gregorian::date,double>& dfs,boost::gregorian::date,boost::gregorian::date);
    };

    class CCS_Instrument : public Instrument //CCS Swap rate to be implemented
    {
    private:

    public:
      CCS_Instrument();
      ~CCS_Instrument(){};
      CCS_Instrument(boost::shared_ptr<psk::staticdata::RateConvention> rc, boost::shared_ptr<psk::staticdata::StaticData> sd,boost::shared_ptr<psk::core::DiscountFactors> ref_curve,boost::shared_ptr<psk::marketdata::Rate> spot_ref,std::string ref_curve_currency,std::string ref_curve_name);
      virtual double _price(std::map<boost::gregorian::date,double>& dfs,boost::gregorian::date,boost::gregorian::date);
    };

    class InstrumentData:public psk::core::BaseObject
    {
    protected:
      std::map<std::string,boost::shared_ptr<Instrument> > _instrument_Map;
    public:
      InstrumentData(){};
      ~InstrumentData(){};
      boost::shared_ptr<Instrument> getInstrument(std::string inst_name){return _instrument_Map[inst_name];};
      void addInstrument(std::string inst_name,boost::shared_ptr<Instrument> instptr){_instrument_Map[inst_name]=instptr;}
    };
  }//instrument


}



#endif
