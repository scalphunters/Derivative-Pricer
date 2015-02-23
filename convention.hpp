#ifndef _CONVENTION_HPP
#define _CONVENTION_HPP

#include<map>
#include<set>
#include<boost/date_time/gregorian/gregorian.hpp>
#include<boost/shared_ptr.hpp>
#include<string>

#include "core.hpp"


namespace psk
{
  namespace staticdata
  {

    namespace basis
    {
 
      class Basis : public psk::core::BaseObject
      {
      private:
	std::map<std::string,double (*)(boost::gregorian::date,boost::gregorian::date)> map_Basis;
      public:
	Basis();
	~Basis();
	void register_Basis(std::string,double(*)(boost::gregorian::date,boost::gregorian::date) );
	double (*operator[](std::string))(boost::gregorian::date,boost::gregorian::date);
      };// Basis class

       // Basis functions
      
      double basis_Act365(boost::gregorian::date, boost::gregorian::date);
      double basis_Act360(boost::gregorian::date, boost::gregorian::date);
      double basis_30360(boost::gregorian::date, boost::gregorian::date);

    }//end of basis

    namespace calendar
    {
      class Holidays : public psk::core::BaseObject
      {
      private:
	std::map<std::string,std::set<boost::gregorian::date> > map_holiday;
	public:
	Holidays();
	~Holidays();
	void register_Holiday(std::string ccy,boost::gregorian::date date1);
	bool is_SpecialHoliday(std::string ccy,boost::gregorian::date date1);
      };
      
      class Calendar : public psk::core::BaseObject
      {
      private:
	std::vector<std::string> currencies;
	bool is_SatHoliday; // weekday 0 = Sun, 6=Sat
	bool is_SunHoliday;
	boost::shared_ptr<Holidays> holidays;
      public:
	Calendar();
	Calendar(std::vector<std::string> ccys,boost::shared_ptr<Holidays> hol,bool SatHol,bool SunHol);
	~Calendar();
	bool is_Holiday(boost::gregorian::date date1);
      };
      class CalendarData:public psk::core::BaseObject
      {
      private:
	std::map<std::string,boost::shared_ptr<Calendar> > calendars;
      public:
	CalendarData(){};
	~CalendarData(){};
	bool addCalendar(std::string,boost::shared_ptr<Calendar>);
	boost::shared_ptr<Calendar> getCalendar(std::string);
	boost::shared_ptr<Calendar> operator()(std::string calname){return getCalendar(calname);}
	friend std::ostream& operator<<(std::ostream&, psk::staticdata::calendar::CalendarData&);
			 
      };

      //date functions
      boost::gregorian::date _ModifiedFollowing(boost::gregorian::date d1,boost::shared_ptr<Calendar> cal );
      boost::gregorian::date _Following(boost::gregorian::date d1,boost::shared_ptr<Calendar> cal );
      boost::gregorian::date _Preceding(boost::gregorian::date d1,boost::shared_ptr<Calendar> cal );
      boost::gregorian::date _Indifferent(boost::gregorian::date d1,boost::shared_ptr<Calendar> cal );
      
      boost::gregorian::date _OpenDayShifter(boost::gregorian::date d1, boost::shared_ptr<Calendar> cal, int iter);

      class DateRule : public psk::core::BaseObject //abstract class for all the date calculation classes
      {
      protected:

      public: 
	DateRule();
	~DateRule();
	virtual boost::gregorian::date operator()(boost::gregorian::date fromdate, boost::shared_ptr<Calendar> cal);
      };

      //daterule data
      class DateRuleData : public psk::core::BaseObject
      {
      protected:
	std::map<std::string,boost::shared_ptr<DateRule> > map_DateRule;
      public:
	DateRuleData(){};
	~DateRuleData(){};
	virtual bool register_DateRule(std::string rulename, boost::shared_ptr<DateRule> d_rule); //return false if failed
	virtual bool delete_DateRule(std::string rulename);
	virtual boost::shared_ptr<DateRule> getDateRule(std::string rulename){return map_DateRule[rulename];};
	virtual boost::shared_ptr<DateRule> operator[](std::string rulename){return getDateRule(rulename);};
      };

      typedef enum _PeriodType{d,od,w,m,y} PeriodType;
      typedef enum _HolidayConvention{ModifiedFollowing,Following,Preceding,Indifferent} HolidayConvention;

      class DateShifter : public DateRule // Simple Date shifter
      {
      protected:
	PeriodType _periodtype ; // d,od,w,m,y ...
	int _iteration; //
	bool _applyHols;//
	HolidayConvention _holConvention;//

      public: 
	DateShifter();
	DateShifter(PeriodType period, int iteration, bool applySpecialHoliday,HolidayConvention holidayConvention);
	~DateShifter();
	virtual boost::gregorian::date operator()(boost::gregorian::date fromdate, boost::shared_ptr<Calendar> cal);
      };

      class CDateRule : public DateRule // Combined date rule (multiple)
      {
      
      protected:
	std::vector<boost::shared_ptr<DateRule> > _ruleList;
      public:
	CDateRule(){};
	~CDateRule(){};
	virtual boost::gregorian::date operator()(boost::gregorian::date fromdate, boost::shared_ptr<Calendar> cal);
	virtual void push_back(boost::shared_ptr<DateRule> rule1){_ruleList.push_back(rule1);};
	
      };



    }//end of calendar

    typedef struct _RateConvention
    {
      std::string name;

      std::map<std::string,std::string> conventions;
      // conventions
      // std::string type; // ex) NULL, SWAP, DEPO, SWAPPOINT, OUTRIGHT ....
      // std::string basis; // ex) NULL, Act/365, 30/360 , ...
      // boost::gregorian::date pricingdate; //rate date
      // std::string currency; // ex) KRW,USD, JPY,... single currencies
      // std::string calendar; // ex) SEO, NY, LDN, SEOLDN, NYLDN... calendar name
      // std::string tomrule; //ex) 1od, or -1od from spotrule, ...
      // std::string spotrule; // ex) 1od,2od, ... //DateRule  class
      // std::string fixingrule; // ex) -2od , ... //DateRule class
      // std::string paymentrule; // ex) +0od , ...//DateRule class
      
      // // FX specific conventions
      // std::string currencypair; //ex) USD/KRW, USD/JPY .. only for fx derivatives

      // // SWAP specific conventions
      // std::string fixedTenor; //ex) 3M, 6M , ... //DateRule class
      // std::string floatingTenor; //ex) 3M,6M, ...//DateRule class
    } RateConvention;

    typedef struct _StaticData
    {
      boost::shared_ptr<basis::Basis> basis;
      boost::shared_ptr<calendar::Holidays> holidays;
      boost::shared_ptr<calendar::CalendarData> calendar;
      boost::shared_ptr<calendar::DateRuleData> daterules;
    } StaticData; // static data collection

  }// end of staticdata
}// end of psk
#endif
