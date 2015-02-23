#ifndef _CURVE_HPP_
#define _CURVE_HPP_

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include<boost/date_time/gregorian/gregorian.hpp>
#include "core.hpp"
#include "instrument.hpp"
#include "marketdata.hpp"
#include <iostream>

namespace psk
{
  namespace curve
  {

    typedef struct _Curvelet{
      boost::shared_ptr<psk::marketdata::Rate> rate;
      boost::shared_ptr<psk::instrument::Instrument> instrument;
    } Curvelet;
    class Curve : public psk::core::BaseObject//virtual class of curve
    {
    protected:
      std::map<boost::gregorian::date, Curvelet > curvelets; //instruments for bootstrapping
      std::string curve_name;
      boost::gregorian::date _today;
      boost::shared_ptr<psk::core::DiscountFactors> discount_factors;
      boost::shared_ptr<psk::staticdata::StaticData> staticdata;
      bool hasBootstrapped;
    public:
      Curve();
      Curve(boost::shared_ptr<psk::staticdata::StaticData> sd){setStaticData(sd);};
      ~Curve();
      virtual double operator()(boost::gregorian::date d1){return getDiscountFactor(d1);}; // get interpolated discount factor; 
      virtual void addCurvelet(Curvelet cvlet,psk::staticdata::StaticData&); // add curvelet
      virtual int setDiscountCurve(std::map<boost::gregorian::date,double> &);
      virtual double getDiscountFactor(boost::gregorian::date); // get interpolated discount factor;
      virtual boost::shared_ptr<psk::core::DiscountFactors> dumpDiscountFactors(){return discount_factors;}
      virtual void setCurveName(std::string);// set curve name
      virtual void setStaticData(boost::shared_ptr<psk::staticdata::StaticData> sd){staticdata=sd;};
      virtual std::string getCurveName(void);// get curve name
      virtual void bootstrap();//bootstrapper

      //utilities
   
   
    };

    class SingleRatesCurve : public Curve //simple curve which returns disc factors , this is going to be entry of CurveData
    {
    private:
  
    public:
    
      SingleRatesCurve();
      SingleRatesCurve(boost::shared_ptr<psk::staticdata::StaticData> sd){setStaticData(sd);};
      ~SingleRatesCurve();
    };
    
    class CurveData : public psk::core::BaseObject// Set of simple curves
    {
    private:
      std::map<std::string, boost::shared_ptr<Curve> > curve_map;
    public:
      boost::shared_ptr<Curve> getCurve(std::string curve_name);
      void registerCurve(std::string curve_name, boost::shared_ptr<Curve> curve);
      CurveData();
      ~CurveData();
    };
    
  }// end of curve namespace
}

#endif
