#include "utilities.hpp"

#include<math.h>

namespace psk
{
  namespace utility
  {

    double DF_Interpolator(std::map<boost::gregorian::date,double>& discount_factors,boost::gregorian::date _today,boost::gregorian::date d1) //test : linear
    {
      std::map<boost::gregorian::date,double>::iterator map_it=discount_factors.begin();
      double df=0.0;
      boost::gregorian::date dt1,dt2;
      double df1,df2;
      
      while(map_it!=discount_factors.end())
	{
	  dt1=map_it->first;
	  df1=map_it->second;
	  map_it++;
	  dt2=map_it->first;
	  df2=map_it->second;
	  
	  if( dt1<=d1 && dt2 >=d1 && d1 >=_today)
	    {
	      df=pow(df1,double((dt2-d1).days())/double((dt2-dt1).days()))*pow(df2,double((d1-dt1).days())/double((dt2-dt1).days()));
	      break;
	    }
	  else if(d1 <=_today)
	    {
	      df=1.0;
	      break;
	    }
	}
      if(map_it==discount_factors.end())
	{
	  map_it--;
	  df= pow(map_it->second,double((d1-_today).days())/double(((map_it->first-_today)).days()));
	}
      return df;
    }
  }//end of utility
}//end of psk
