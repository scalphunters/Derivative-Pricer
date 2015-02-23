#ifndef _IO_HPP_
#define _IO_HPP_
// handling IO to load staticdata,marketdata,curvedata,instruments, from file/databases

#include "core.hpp"
#include <tinyxml/tinyxml.h>
#include <string>
#include <vector>
#include <map>
#include "convention.hpp"
#include "marketdata.hpp"
#include "instrument.hpp"
#include "curve.hpp"
#include "my_global.h"
#include "mysql.h"

#define TIXML_USE_STL

namespace psk
{
  namespace io
  {
    class IOHandler : public psk::core::BaseObject //base class for IO handlings
    {
    protected:

      
      boost::shared_ptr<psk::marketdata::MarketData> mr_data;
      boost::shared_ptr<psk::instrument::InstrumentData> inst_data;
      boost::shared_ptr<psk::curve::CurveData> curve_data;
      
      boost::shared_ptr<psk::staticdata::calendar::Holidays> holdata;
      boost::shared_ptr<psk::staticdata::calendar::CalendarData> cal_data;
      boost::shared_ptr<psk::staticdata::calendar::DateRuleData> dr_data;

      boost::shared_ptr<psk::staticdata::StaticData> staticdata;
    
    public:
      IOHandler(){};
      ~IOHandler(){};
      
      //marketdata
      virtual boost::shared_ptr<psk::marketdata::MarketData> loadMarketData(){};
      //staticdata
      virtual boost::shared_ptr<psk::staticdata::calendar::Holidays> loadHolidays(){}; // 1st load
      virtual boost::shared_ptr<psk::staticdata::calendar::CalendarData> loadCalendar(){}; // 2nd load
      virtual boost::shared_ptr<psk::staticdata::calendar::DateRuleData> loadDateRules(){}; // 3rd load
      
      //instruments
      virtual boost::shared_ptr<psk::instrument::InstrumentData> loadInstruments(){}; //load this before curve_1
      virtual boost::shared_ptr<psk::instrument::InstrumentData> loadInstruments_Second(){}; //load this after curve_1
      //curves
      virtual boost::shared_ptr<psk::curve::CurveData> loadCurves(){};
      virtual boost::shared_ptr<psk::curve::CurveData> loadCurves_Second(){};
      //all
      virtual bool loadData(std::string dataname)
      {
		loadHolidays();
		loadCalendar();
		loadDateRules();
		loadMarketData();
		loadInstruments();
		loadCurves();
		loadInstruments_Second();
		loadCurves_Second();
      }; //datename : holidays,calendar,marketdata,daterules,curves,instruments
    
    };

    class IOHandlerMySQL : public IOHandler
    {
    private:

    public:
    	IOHandlerMySQL(){};
    	IOHandlerMySQL(std::string host,std::string userid,std::string password,std::string database){};
    	~IOHandlerMySQL(){};
    };

    class IOHandlerXML : public IOHandler
    {
    private:
      TiXmlDocument xmldoc;
      std::string filename;
      std::map<std::string,std::string> filemap; //xml files mapping
      bool initialize(){loadFileMap(filename);};
    public:
      IOHandlerXML(){};
      IOHandlerXML(std::string fn){filename=fn;initialize();};
      ~IOHandlerXML(){};
      bool loadFileMap(std::string filename);

             //overridings

      //marketdata
      virtual boost::shared_ptr<psk::marketdata::MarketData> loadMarketData();
      //staticdata
      virtual boost::shared_ptr<psk::staticdata::calendar::Holidays> loadHolidays(); // 1st load
      virtual boost::shared_ptr<psk::staticdata::calendar::CalendarData> loadCalendar(); // 2nd load
      virtual boost::shared_ptr<psk::staticdata::calendar::DateRuleData> loadDateRules(); // 3rd load
      
      //instruments
      virtual boost::shared_ptr<psk::instrument::InstrumentData> loadInstruments(); //load this before curve_1
      virtual boost::shared_ptr<psk::instrument::InstrumentData> loadInstruments_Second(); //load this after curve_1
      //curves
      virtual boost::shared_ptr<psk::curve::CurveData> loadCurves();
      virtual boost::shared_ptr<psk::curve::CurveData> loadCurves_Second();
      //all
      virtual bool loadData(std::string dataname)
      {
		loadFileMap(dataname);
		loadHolidays();
		loadCalendar();
		loadDateRules();
		loadMarketData();
		loadInstruments();
		loadCurves();
		loadInstruments_Second();
		loadCurves_Second();
      };

      void printfmap();
    };//end of IOHandlerXML class


  }//end io
}//end psk


#endif
