#include "parser.hpp"

namespace psk
{
  namespace parser
  {
    gregorian::date _dailyconversion(gregorian::date today, int type, boost::shared_ptr<psk::staticdata::RateConvention> rc,psk::staticdata::StaticData &sd) // type : today=0, tom=1, spot=2
    {
      gregorian::date retdate=today;
      boost::shared_ptr<psk::staticdata::calendar::DateRule> dr;
      boost::shared_ptr<psk::staticdata::calendar::Calendar> cal;
      std::string tomrule=rc->conventions["tomrule"];
      std::string spotrule=rc->conventions["spotrule"];
      std::string calname=rc->conventions["paymentcalendar"];

      cal=sd.calendar->getCalendar(calname);
      switch(type)
	{
	case 0:  retdate=today;break;
	case 1:  
	  dr=sd.daterules->getDateRule(tomrule);
	  retdate=dr->operator()(today,cal);break;
	case 2:  
	  dr=sd.daterules->getDateRule(spotrule);
	  retdate=dr->operator()(today,cal);break;
	default: break;            
	}
      return retdate;
    }//end _dailyconversion

    bool _isDateValid(int year,int month,int days)
    {
      bool validity= ( year>=1900 && year<4000) && (month >=1 && month <=12);
      bool isLeapYear= ( double(year)/4.0 == int(year/4) );
    
      validity= validity && (days>=1) && (days<=31);

      if(validity)
	{
	  switch(month)
	    {
	    case 2 : validity=isLeapYear? (validity && (days<=29)) : (validity && days<=28) ; break;
	    case 4 :
	    case 6 :
	    case 9 :
	    case 11: validity=validity && (days<=30); break;
	    default: break;
	    }
	}
      return validity;
    }

    gregorian::date _dateadd(gregorian::date tdy, int iter, int tenortype)
    {
      gregorian::date retdate;               
      retdate=tdy;
     
      switch(tenortype)
	{
	case 1: retdate=retdate+gregorian::days(iter);break;//calendar day
	case 2: retdate=retdate+gregorian::weeks(iter);break;//week
	case 3: retdate=retdate+gregorian::months(iter);break;//month
	case 4: retdate=retdate+gregorian::years(iter);break;//year
	case 5: break;//opendays to be incorporated with calendar             
	default:break;
	}
      return retdate;
    }//end _dateadd

    gregorian::date _dateAdjustment(gregorian::date tdy,  int adjust_type , boost::shared_ptr<psk::staticdata::RateConvention> rc,psk::staticdata::StaticData &sd)
    {
      gregorian::date matdate=tdy;
      std::string ruletype="paymentrule";
      std::string caltype="paymentcalendar";
      switch(adjust_type)
	{
	case 0: ruletype="Indifferent";break;
	case 1: ruletype="paymentrule";caltype="paymentcalendar";break;
	case 2: ruletype="fixingrule";caltype="fixingcalendar";break;
	default:break;
	}
      if(adjust_type==1 || adjust_type==2)
	{
	  matdate=sd.daterules->getDateRule(rc->conventions[ruletype])->operator()(matdate,sd.calendar->getCalendar(rc->conventions[caltype]));
	}
      return matdate;
    }//end _dateAdjustment

  }
}
