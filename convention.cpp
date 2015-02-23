#include "convention.hpp"


using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace psk
{
  namespace staticdata
  {
    namespace basis
    {
      // initiate and loading basis functions

      Basis::Basis()
      {
	register_Basis("Act365",basis_Act365);
	register_Basis("Act360",basis_Act360);
	register_Basis("30360",basis_30360);
      }
      Basis::~Basis(){}
      void Basis::register_Basis(std::string basisname,double (*func)(boost::gregorian::date,boost::gregorian::date))
      {
	map_Basis[basisname]=func;
      }
      double(* Basis::operator[](std::string basisname))(boost::gregorian::date,boost::gregorian::date)
      {
	return map_Basis[basisname];
      }
      // implementing  Basis 

      double basis_Act365(boost::gregorian::date d1, boost::gregorian::date d2)
      {
	return double((d2-d1).days())/365;
      }
      double basis_Act360(boost::gregorian::date d1, boost::gregorian::date d2)
      {
	return double((d2-d1).days())/360;
      }

      double basis_30360(boost::gregorian::date d1, boost::gregorian::date d2)
      {
	double tau=0.0;
	
	tau+=double(d2.year()-d1.year());
	tau+= double((d2.month()-d1.month()))*30/360;
	tau+= double((d2.day()-d1.day()))/360;
	return tau;
      }
    }//end of basis 
    namespace calendar
    {
      Holidays::Holidays(){}
      Holidays::~Holidays(){}
      void Holidays::register_Holiday(std::string ccy, boost::gregorian::date date1)
      {
	map_holiday[ccy].insert(date1);
      }
      bool Holidays::is_SpecialHoliday(std::string ccy,boost::gregorian::date date1)
      {
	if(map_holiday[ccy].find(date1)==map_holiday[ccy].end()) return false;
	else return true;
      }

      Calendar::Calendar()
      {
	is_SatHoliday=true;
	is_SunHoliday=true;
      }
      Calendar::Calendar(std::vector<std::string> ccys,boost::shared_ptr<Holidays> hol,bool SatHol,bool SunHol)
      {
	is_SatHoliday=SatHol;
	is_SunHoliday=SunHol;
	holidays=hol;
	currencies=ccys;
      }
      Calendar::~Calendar(){}
      bool Calendar::is_Holiday(boost::gregorian::date date1)
      {
	bool ishol=false;
	for(std::vector<std::string>::iterator it=currencies.begin();it!=currencies.end();it++)
	  {
	    ishol= ishol | holidays->is_SpecialHoliday(*it,date1);
	  }
	std::string wd=date1.day_of_week().as_short_string();
	ishol= ishol | (is_SatHoliday & wd=="Sat") | (is_SunHoliday & wd=="Sun");
	return ishol;
      }
      //calendar data
      bool CalendarData::addCalendar(std::string calname,boost::shared_ptr<Calendar> cal)
      {
	calendars[calname]=cal;
	return true;
      }
      boost::shared_ptr<Calendar> CalendarData::getCalendar(std::string calname)
      {
	return calendars[calname];
      }

      std::ostream& operator<<(std::ostream& o, psk::staticdata::calendar::CalendarData& caldat)
      {
	std::map<std::string,boost::shared_ptr<Calendar> >::iterator map_it;
	for(map_it=caldat.calendars.begin();map_it!=caldat.calendars.end();map_it++)
	  {
	    o<< (*map_it).first << " " <<std::endl;
	  }
	return o;
      }

      //date functions
      boost::gregorian::date _ModifiedFollowing(boost::gregorian::date d1,boost::shared_ptr<Calendar> cal )
      {
	boost::gregorian::date retdate;
	retdate=_Following(d1,cal);
	if(d1.month()!=retdate.month())
	  retdate=_Preceding(d1,cal);
	return retdate;
      }
      boost::gregorian::date _Following(boost::gregorian::date d1,boost::shared_ptr<Calendar> cal )
      {
	boost::gregorian::date retdate=d1;
	while(cal->is_Holiday(retdate))
	  {
	    retdate=retdate+days(1);
	  }
	return retdate;
      }
      boost::gregorian::date _Preceding(boost::gregorian::date d1,boost::shared_ptr<Calendar> cal )
      {
	boost::gregorian::date retdate=d1;
	while(cal->is_Holiday(retdate))
	  {
	    retdate=retdate-days(1);
	  }
	return retdate;
      }
      boost::gregorian::date _Indifferent(boost::gregorian::date d1,boost::shared_ptr<Calendar> cal )
      {
	return d1;
      } 
      boost::gregorian::date _OpenDayShifter(boost::gregorian::date d1,boost::shared_ptr<Calendar> cal, int iter)
      {
	boost::gregorian::date retdate=d1;
	if(iter>=0)
	  {
	    for(int i=0;i<iter;i++)
	      {
		retdate=retdate+days(1);
		retdate=_Following(retdate,cal);
	      }
	  }
	else
	  {
	    for(int i=0;i>iter;i--)
	      {
		retdate=retdate-days(1);
		retdate=_Preceding(retdate,cal);
	      }
	  }
	return retdate;
      }
      //DateRule
      DateRule::DateRule(){};
      DateRule::~DateRule(){};
      boost::gregorian::date DateRule::operator()(boost::gregorian::date fromdate, boost::shared_ptr<Calendar> cal)
      {
	return fromdate;
      }
      //DateRuleDate : BaseObject ; daterule map
      bool DateRuleData::register_DateRule(std::string rulename, boost::shared_ptr<DateRule> d_rule)
      {
	if(map_DateRule.find(rulename)!=map_DateRule.end())
	  {
	    return false;
	  }
	else
	  {
	    map_DateRule[rulename]=d_rule;
	    return true;
	  }
      }
      bool DateRuleData::delete_DateRule(std::string rulename)
      {
	if(map_DateRule.find(rulename)!=map_DateRule.end())
	  {
	    map_DateRule.erase(rulename);
	    return true;
	  }
	else
	  {
	    return false;
	  }
      }

      //DateShifter : DateRule
      DateShifter::DateShifter(){};
      DateShifter::~DateShifter(){};
      DateShifter::DateShifter(PeriodType period, int iter, bool applySpecialHoliday,HolidayConvention holidayConvention)
      {
	_periodtype=period;
	_iteration=iter;
	_applyHols=applySpecialHoliday;
	_holConvention=holidayConvention;
      }
      boost::gregorian::date DateShifter::operator()(boost::gregorian::date fromdate, boost::shared_ptr<Calendar> cal)
      {
	boost::gregorian::date retdate; //result date
	boost::shared_ptr<Calendar> curcal;
	if(_applyHols)
	  curcal=cal;
	else
	  curcal=boost::shared_ptr<Calendar>(new Calendar);
	
	//shifting
	switch(_periodtype)
	  {
	  case d:
	    retdate=fromdate+days(_iteration);	    break;
	  case od: 	  
	    retdate=_OpenDayShifter(fromdate,curcal,_iteration);
	    break;
	  case w:
	    retdate=fromdate+weeks(_iteration);	    break;
	  case m:
	    retdate=fromdate+months(_iteration);    break;
	  case y:
	    retdate=fromdate+years(_iteration);	    break;
	  default: break;
	  }
	//adjusting 
	
	switch(_holConvention)
	  {
	  case ModifiedFollowing :
	    retdate=_ModifiedFollowing(retdate,curcal);break;
	  case Following:
	    retdate=_Following(retdate,curcal);break;
	  case Preceding:
	    retdate=_Preceding(retdate,curcal);break;
	  case Indifferent:
	    retdate=_Indifferent(retdate,curcal);break;
	  default:break;
	  }
	
	return retdate;
      }

      //CDateRule
      boost::gregorian::date CDateRule::operator()(boost::gregorian::date fromdate,boost::shared_ptr<Calendar> cal)
      {
	std::vector<boost::shared_ptr<DateRule> >::iterator tmp_it;
	boost::gregorian::date retdate=fromdate;
	for(tmp_it=_ruleList.begin();tmp_it!=_ruleList.end();tmp_it++)
	  {
	    retdate=(*tmp_it)->operator()(retdate,cal);
	  }
	return retdate;
      }
      
    }//end of calendar

    
  }//end of convention
}// end of psk
