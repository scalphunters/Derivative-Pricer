#include "instrument.hpp"
#include "utilities.hpp"
#include <math.h>


using namespace psk::instrument;
using namespace std;
using namespace boost::gregorian;
using namespace boost;
using namespace psk::staticdata::basis;


//instruments
Instrument::Instrument(){};
Instrument::~Instrument(){};
double Instrument::operator()(std::map<boost::gregorian::date,double>&  cv,boost::gregorian::date start,boost::gregorian::date maturity)
{
  return _price(cv,start,maturity);
};

double Instrument::_price(std::map<boost::gregorian::date,double>& cv,boost::gregorian::date start,boost::gregorian::date maturity)
{
  return 0;
};


double abs(double x)
{
  return (x>=0)?x:-x;
}


double Instrument::_solve(psk::core::DiscountFactors &df, boost::gregorian::date start,boost::gregorian::date maturity, double targetvalue)
{
  //use bisection df initial = (0,10), tolerance=1e-7
  psk::core::DiscountFactors tmpdf=df;
  double lbound=0;
  double ubound=10;
  double init=1;
  double tolerance=1e-7;
  double estimation=0.0;
  double error;
  tmpdf[maturity]=init;
  estimation=_price(tmpdf,start,maturity);
  error=std::abs(targetvalue-estimation);
  while (error > tolerance)
    {
      if(estimation > targetvalue)
	{
	  lbound=init;
	  init=(lbound+ubound)/2;
	}
      else if(estimation<targetvalue)
	{
	  ubound=init;
	  init=(lbound+ubound)/2;
	}
      else if(estimation==targetvalue)
	{
	  return init;
	}
      tmpdf[maturity]=init;
      estimation=_price(tmpdf,start,maturity);
      error=std::abs(targetvalue-estimation);
    }
  return init;
}

void Instrument::setPricingDate(boost::gregorian::date d1)
{
  pricing_date=d1;
}

Depo_Instrument::Depo_Instrument(){};
Depo_Instrument::Depo_Instrument(shared_ptr<psk::staticdata::RateConvention> rc,shared_ptr<psk::staticdata::StaticData> sd)
{
  rateconvention=rc;
  staticdata=sd;
  
}
Depo_Instrument::~Depo_Instrument(){};


double Depo_Instrument::_price(std::map<boost::gregorian::date,double>& cv, boost::gregorian::date start,boost::gregorian::date maturity)
{
  double df=psk::utility::DF_Interpolator(cv,cv.begin()->first,start)/psk::utility::DF_Interpolator(cv,cv.begin()->first,maturity);
  double tau=staticdata->basis->operator[](rateconvention->conventions["basis"])(start,maturity);
  return (df-1.0)/tau;
}

//Swap instrument
IRS_Instrument::IRS_Instrument(){};
IRS_Instrument::~IRS_Instrument(){};
IRS_Instrument::IRS_Instrument(shared_ptr<psk::staticdata::RateConvention> rc,shared_ptr<psk::staticdata::StaticData> sd)
{
  rateconvention=rc;
  staticdata=sd;
}
double IRS_Instrument::_price(std::map<boost::gregorian::date,double>& cv, boost::gregorian::date start,boost::gregorian::date maturity)
{
  using psk::utility::DF_Interpolator;
  using boost::gregorian::date;

  boost::shared_ptr<psk::staticdata::calendar::Calendar> fixing_cal=staticdata->calendar->getCalendar(rateconvention->conventions["fixingcalendar"]);
  boost::shared_ptr<psk::staticdata::calendar::Calendar> payment_cal=staticdata->calendar->getCalendar(rateconvention->conventions["paymentcalendar"]);
  boost::shared_ptr<psk::staticdata::calendar::DateRule> fixed_tenor=staticdata->daterules->operator[](rateconvention->conventions["fixedtenor"]);
  boost::shared_ptr<psk::staticdata::calendar::DateRule> floating_tenor=staticdata->daterules->operator[](rateconvention->conventions["floatingtenor"]);
  boost::shared_ptr<psk::staticdata::calendar::DateRule> fixing_rule=staticdata->daterules->operator[](rateconvention->conventions["fixingrule"]);
  boost::shared_ptr<psk::staticdata::calendar::DateRule> payment_rule=staticdata->daterules->operator[](rateconvention->conventions["paymentrule"]);

  double tau;
  double swaprate=0.0;
  
   //floating cashflow
  date spotdate=start;
  date computedate=floating_tenor->operator()(spotdate,payment_cal);
  date paymentdate=payment_rule->operator()(computedate,payment_cal);

  double floatingsum=0.0;
  double df_prevpayment,df_payment;
  double df_start=DF_Interpolator(cv,cv.begin()->first, start);
  df_prevpayment=df_start;

  if(computedate>=maturity && spotdate<maturity)
    {
      computedate=maturity;
      paymentdate=maturity;
    }
  while(computedate<=maturity)
    {  
      tau=staticdata->basis->operator[](rateconvention->conventions["basis"])(spotdate,paymentdate);
      
      df_payment=DF_Interpolator(cv,cv.begin()->first, paymentdate);

      floatingsum+= (df_prevpayment/df_start-df_payment/df_start);
      //floatingsum+= df_payment/df_start*( pow(df_prevpayment/df_payment,1/tau) -1 )*tau;
      
      spotdate=paymentdate;
      computedate=floating_tenor->operator()(computedate,payment_cal);
      paymentdate=payment_rule->operator()(computedate,payment_cal);
      if(computedate>=maturity && spotdate<maturity)
	{
	  computedate=maturity;
	  paymentdate=maturity;
	}
      df_prevpayment=df_payment;
    }

  //fixed leg cashflow to be implemented

  spotdate=start;
  computedate=fixed_tenor->operator()(spotdate,payment_cal);
  paymentdate=payment_rule->operator()(computedate,payment_cal);

  double fixedsum=0.0;
  df_start=DF_Interpolator(cv,cv.begin()->first, start);
  df_prevpayment=df_start;

  if(computedate>=maturity && spotdate<maturity)
    {
      computedate=maturity;
      paymentdate=maturity;
    }
  while(computedate<=maturity)
    {  
      
      tau=staticdata->basis->operator[](rateconvention->conventions["basis"])(spotdate,paymentdate);
      
      df_payment=DF_Interpolator(cv,cv.begin()->first, paymentdate);

      fixedsum+= tau*df_payment/df_start;
      
      spotdate=paymentdate;
      computedate=fixed_tenor->operator()(computedate,payment_cal);
      paymentdate=payment_rule->operator()(computedate,payment_cal);
      if(computedate>=maturity && spotdate<maturity)
	{
	  computedate=maturity;
	  paymentdate=maturity;
	}
      df_prevpayment=df_payment;
    }

  swaprate=floatingsum/fixedsum;

  return swaprate;
}

//SWAP POINT instrument


Swappoint_Instrument::Swappoint_Instrument(shared_ptr<psk::staticdata::RateConvention> rc,shared_ptr<psk::staticdata::StaticData> sd,boost::shared_ptr<psk::core::DiscountFactors> ref_curve,boost::shared_ptr<psk::marketdata::Rate> spot_ref,std::string ref_curve_currency,std::string ref_curve_nm)
{
  rateconvention=rc;
  staticdata=sd;
  reference_curve=ref_curve;
  spot_reference=spot_ref;
  ref_curve_ccy=ref_curve_currency;
  ref_curve_name=ref_curve_nm;
}

double Swappoint_Instrument::_price(psk::core::DiscountFactors& dfs, boost::gregorian::date start, boost::gregorian::date maturity)
{
  double ret_rate=0.0;
  double spot=spot_reference->rate;
  psk::core::DiscountFactors basedf=*reference_curve;
  using psk::utility::DF_Interpolator;

  double df1=DF_Interpolator(dfs,dfs.begin()->first,maturity)/DF_Interpolator(dfs,dfs.begin()->first,start);
  double df2=DF_Interpolator(basedf,basedf.begin()->first,maturity)/DF_Interpolator(basedf,basedf.begin()->first,start);
  double df_final=0.0;
  if(rateconvention->conventions["quotecurrency"]!=ref_curve_ccy) // if instruments' ccy is quote ccy.
    {
      df_final=df2/df1;
      ret_rate=(df_final-1)*spot;// if(_DEBUG_FLAG) std::cout << df2 <<endl;
    }
  else //if instruments' ccy is base ccy (inverted)
    {
      df_final=df1/df2;
      ret_rate=(df_final-1)*spot;
    }
  return ret_rate;
}

//CCS Instrument implementation

CCS_Instrument::CCS_Instrument(shared_ptr<psk::staticdata::RateConvention> rc,shared_ptr<psk::staticdata::StaticData> sd,boost::shared_ptr<psk::core::DiscountFactors> ref_curve,boost::shared_ptr<psk::marketdata::Rate> spot_ref,std::string ref_curve_currency,std::string ref_curve_nm)
{
  rateconvention=rc;
  staticdata=sd;
  reference_curve=ref_curve;
  spot_reference=spot_ref;
  ref_curve_ccy=ref_curve_currency;
  ref_curve_name=ref_curve_nm;
}

double CCS_Instrument::_price(psk::core::DiscountFactors& cv, boost::gregorian::date start, boost::gregorian::date maturity)
{
  using psk::utility::DF_Interpolator;
  using boost::gregorian::date;

  boost::shared_ptr<psk::staticdata::calendar::Calendar> fixing_cal=staticdata->calendar->getCalendar(rateconvention->conventions["fixingcalendar"]);
  boost::shared_ptr<psk::staticdata::calendar::Calendar> payment_cal=staticdata->calendar->getCalendar(rateconvention->conventions["paymentcalendar"]);
  boost::shared_ptr<psk::staticdata::calendar::DateRule> fixed_tenor=staticdata->daterules->operator[](rateconvention->conventions["fixedtenor"]);
  boost::shared_ptr<psk::staticdata::calendar::DateRule> floating_tenor=staticdata->daterules->operator[](rateconvention->conventions["floatingtenor"]);
  boost::shared_ptr<psk::staticdata::calendar::DateRule> fixing_rule=staticdata->daterules->operator[](rateconvention->conventions["fixingrule"]);
  boost::shared_ptr<psk::staticdata::calendar::DateRule> payment_rule=staticdata->daterules->operator[](rateconvention->conventions["paymentrule"]);

  double tau;
  double swaprate=0.0;
  
   //floating cashflow
  date spotdate=start;
  date computedate=floating_tenor->operator()(spotdate,payment_cal);
  date paymentdate=payment_rule->operator()(computedate,payment_cal);

  double floatingsum=0.0;
  double df_prevpayment,df_payment;
  double df_start=DF_Interpolator(cv,cv.begin()->first, start);
  df_prevpayment=df_start;

  if(computedate>=maturity && spotdate<maturity)
    {
      computedate=maturity;
      paymentdate=maturity;
    }
  while(computedate<=maturity)
    {  
      tau=staticdata->basis->operator[](rateconvention->conventions["basis"])(spotdate,paymentdate);
      
      df_payment=DF_Interpolator(cv,cv.begin()->first, paymentdate);

      floatingsum+= (df_prevpayment/df_start-df_payment/df_start);
      //floatingsum+= df_payment/df_start*( pow(df_prevpayment/df_payment,1/tau) -1 )*tau;
      
      spotdate=paymentdate;
      computedate=floating_tenor->operator()(computedate,payment_cal);
      paymentdate=payment_rule->operator()(computedate,payment_cal);
      if(computedate>=maturity && spotdate<maturity)
	{
	  computedate=maturity;
	  paymentdate=maturity;
	}
      df_prevpayment=df_payment;
    }

  //fixed leg cashflow to be implemented

  spotdate=start;
  computedate=fixed_tenor->operator()(spotdate,payment_cal);
  paymentdate=payment_rule->operator()(computedate,payment_cal);

  double fixedsum=0.0;
  df_start=DF_Interpolator(cv,cv.begin()->first, start);
  df_prevpayment=df_start;

  if(computedate>=maturity && spotdate<maturity)
    {
      computedate=maturity;
      paymentdate=maturity;
    }
  while(computedate<=maturity)
    {  
      
      tau=staticdata->basis->operator[](rateconvention->conventions["basis"])(spotdate,paymentdate);
      
      df_payment=DF_Interpolator(cv,cv.begin()->first, paymentdate);

      fixedsum+= tau*df_payment/df_start;
      
      spotdate=paymentdate;
      computedate=fixed_tenor->operator()(computedate,payment_cal);
      paymentdate=payment_rule->operator()(computedate,payment_cal);
      if(computedate>=maturity && spotdate<maturity)
	{
	  computedate=maturity;
	  paymentdate=maturity;
	}
      df_prevpayment=df_payment;
    }

  swaprate=floatingsum/fixedsum;

  return swaprate;

}

