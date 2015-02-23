#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_directive.hpp>
#include <boost/spirit/include/qi_no_case.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <iostream>
#include <vector>
#include <string>

#include "convention.hpp"



namespace psk
{
  namespace parser
  {

    using namespace std;
    namespace qi=boost::spirit::qi;
    namespace phoenix=boost::phoenix;
    namespace gregorian=boost::gregorian;
    namespace asc=boost::spirit::ascii;

    gregorian::date _dailyconversion(gregorian::date today, int type, boost::shared_ptr<psk::staticdata::RateConvention> rc,psk::staticdata::StaticData &sd); // type : today=0, tom=1, spot=2    
    bool _isDateValid(int year,int month,int days);
    gregorian::date _dateadd(gregorian::date tdy, int iter, int tenortype);
    gregorian::date _dateAdjustment(gregorian::date tdy,  int adjust_type , boost::shared_ptr<psk::staticdata::RateConvention> rc,psk::staticdata::StaticData &sd); 
    //tenor parser
    template<typename Iterator>
    bool parse_tenor(Iterator first, Iterator last,gregorian::date today,gregorian::date &d /* output date */,boost::shared_ptr<psk::staticdata::RateConvention> rc,psk::staticdata::StaticData &sd,int starttype/* 0 : from tod, 1: from tom 2: from spot*/, int adjustment /* 0: indifferent, 1: paymentrule, 2: fixingrule */)
    {
      bool r;
      int year,month,day;
      int iter;
      std::string tnrstr;
      int tenortype;
      int type=0;
      std::string dailystr;
      int dailytype=0;
      gregorian::date spdate;
  
      qi::rule<std::string::iterator, std::string()> date_ = qi::skip(' ')[qi::int_[phoenix::ref(year)=qi::_1]
									   >> '-'
									   >> qi::int_[phoenix::ref(month)=qi::_1]
									   >> '-'
									   >> qi::int_[phoenix::ref(day)=qi::_1]];

      qi::rule<std::string::iterator,std::string()> tnrstr_ = 
	qi::no_case[std::string("m")][phoenix::ref(tenortype)=phoenix::val(3)]
	| qi::no_case[std::string("d")][phoenix::ref(tenortype)=phoenix::val(1)]
	| qi::no_case[std::string("y")][phoenix::ref(tenortype)=phoenix::val(4)]
	| qi::no_case[std::string("w")][phoenix::ref(tenortype)=phoenix::val(2)]
	| qi::no_case[std::string("od")][phoenix::ref(tenortype)=phoenix::val(5)]
	;
      qi::rule<std::string::iterator,std::string()> tenor_ = qi::int_[phoenix::ref(iter)=qi::_1] 
	>>tnrstr_[phoenix::ref(tnrstr)=qi::_1];
					   			

      qi::rule<std::string::iterator,std::string()> dailystr_ = qi::no_case[std::string("tod")][phoenix::ref(dailytype)=phoenix::val(0)] 
	| qi::no_case[std::string("tom")][phoenix::ref(dailytype)=phoenix::val(1)] 
	| qi::no_case[std::string("spot")][phoenix::ref(dailytype)=phoenix::val(2)];
                                                          
      qi::rule<std::string::iterator,std::string()> daily_ = dailystr_[phoenix::ref(dailystr)=qi::_1];
  
      r=qi::phrase_parse(first,last,     date_[phoenix::ref(type)=phoenix::val(1)] 
			 | tenor_[phoenix::ref(type)=phoenix::val(2)] 
			 | daily_[phoenix::ref(type)=phoenix::val(3)] , qi::space);
  
  
      if(first!=last) r=false;
      if(r) 
	{
	  switch(type)
	    {
	    case 1:   
	      if(_isDateValid(year,month,day)) 
		d=gregorian::date(year,month,day);
	      else
		d=gregorian::date(1900,1,1); //default error date
	      break;
	    case 2:   
	      spdate=_dailyconversion(today,starttype,rc,sd);
	      d=_dateadd(spdate,iter,tenortype);
	      d=_dateAdjustment(d,adjustment,rc,sd);
	      break;
	    case 3:   d=_dailyconversion(today, dailytype,rc,sd);break;
	    default: break;
	    }
	}
      return r;	 
    } //end of parse_tenor

    
    
  }//end parser
}//end psk


#endif
