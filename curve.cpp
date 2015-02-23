#include "curve.hpp"
#include<math.h>
#include "utilities.hpp"
#include "parser.hpp"
#include <iostream> //for test

using namespace psk::curve;
using namespace std;
using namespace boost::gregorian;
using namespace boost;
using namespace psk::utility;

//Curve class

namespace psk
{
  namespace curve
  {
    Curve::Curve()
    {
      _today=day_clock::local_day();
      hasBootstrapped=false; 
      discount_factors=boost::shared_ptr<psk::core::DiscountFactors>(new psk::core::DiscountFactors);
      (*discount_factors)[_today]=1.0; 
   };
    Curve::~Curve(){};
    void Curve::addCurvelet(Curvelet cvlet,psk::staticdata::StaticData& sd)
    {

      boost::gregorian::date matdate;
      std::string tenor=cvlet.rate->tenor;
      boost::shared_ptr<psk::staticdata::RateConvention> rc= cvlet.rate->rateconvention;
      psk::parser::parse_tenor(tenor.begin(),tenor.end(), _today, matdate, rc, sd,2,1); 
      
      if(_DEBUG_FLAG)
	cout << matdate << " " << cvlet.rate->name << " " << cvlet.rate->rate << endl << "curve.cpp - L35" << endl;
      
      curvelets[matdate]=cvlet;
    }

    int Curve::setDiscountCurve(std::map<boost::gregorian::date, double> & curvemap)
    {
      (*discount_factors).clear();
      map<date,double>::iterator map_it;
      (*discount_factors)[_today]=1.0;
      for (map_it=curvemap.begin();map_it!=curvemap.end();map_it++)
	{
	  (*discount_factors)[map_it->first]=map_it->second;
	}
      return 1;
    }

    double Curve::getDiscountFactor(date d1) //test : linear
    {
      return psk::utility::DF_Interpolator(*discount_factors,_today,d1);

    }

    void Curve::setCurveName(string cn)
    {
      curve_name=cn;
    }

    string Curve::getCurveName(void)
    {
      return curve_name;
    }

    void Curve::bootstrap()
    {
      std::map<boost::gregorian::date,Curvelet>::iterator cv_it=curvelets.begin();
      boost::shared_ptr<psk::staticdata::RateConvention> rc;
      std::string tenor;
      boost::gregorian::date startdate;
      boost::gregorian::date maturitydate;
      double targetvalue;
      double df;
      for(cv_it=curvelets.begin();cv_it!=curvelets.end();cv_it++)
	{
	  maturitydate=cv_it->first;
	  targetvalue=cv_it->second.rate->rate;
	  tenor=cv_it->second.rate->start;
	  rc=cv_it->second.rate->rateconvention;
	  if(!psk::parser::parse_tenor(tenor.begin(),tenor.end(),_today,startdate,rc,*staticdata,2,1))
	    {
	      std::cerr << "Error on parsing tenor" << std::endl;
	    }

	  df=cv_it->second.instrument->_solve(*discount_factors,startdate,maturitydate,targetvalue);
	  (*discount_factors)[maturitydate]=df;
	}
      hasBootstrapped=true;
    }//end of bootstrapping


    //SingleRatesCurve classes

    SingleRatesCurve::SingleRatesCurve()
    {
      _today=day_clock::local_day();
      hasBootstrapped=false;
      discount_factors=boost::shared_ptr<psk::core::DiscountFactors>(new psk::core::DiscountFactors);
      (*discount_factors)[_today]=1.0;
    }
    SingleRatesCurve::~SingleRatesCurve()
    {}

    //CurveData

    CurveData::CurveData()
    {
      //Null curve 
      shared_ptr<Curve> tmp(new SingleRatesCurve());
      tmp->setCurveName(string("NULL"));
      registerCurve(tmp->getCurveName(),tmp);
    }
    CurveData::~CurveData()
    {}

    void CurveData::registerCurve(string cn, shared_ptr<Curve> cv )
    {
      curve_map[cn]=cv;
    }
    shared_ptr<Curve> CurveData::getCurve(string curve_name)
    {
      if (curve_map.find(curve_name)!=curve_map.end())
	return curve_map[curve_name];
      else
	return curve_map[string("NULL")];
    }
  }//end of curve namespace
}//end of psk namespace
