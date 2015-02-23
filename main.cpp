#include <iostream>
#include <set>
#include <math.h>
  
#include "pskpricer.hpp"

using namespace std;
using namespace psk::marketdata;
using namespace boost;
using namespace psk::curve;
using namespace boost::gregorian;
using namespace psk::staticdata::basis;
using namespace psk::staticdata::calendar; 
using namespace psk::staticdata;
using namespace psk::instrument;
using namespace psk::core;
using namespace psk::io;
using namespace psk::parser;

void print_map(map<date,double> tmp)
{
  map<date,double>::iterator map_it;
  for( map_it=tmp.begin();map_it!=tmp.end();map_it++)
    {
      cout << map_it->first << " : " << map_it->second << endl;
    }
}

int main(int argc, char** argv)
{

	 shared_ptr<StaticData> staticdata(new StaticData);
	 IOHandlerXML myIO;

	 cout <<myIO.loadFileMap("./xmldata/filemap.xml")<<endl;
	 myIO.printfmap();

	 staticdata->holidays=myIO.loadHolidays();

	 cout<<staticdata->holidays->is_SpecialHoliday("SEO",from_simple_string("2011-10-03"))<<endl;
	 cout<<staticdata->holidays->is_SpecialHoliday("SEO",from_simple_string("2011-10-02"))<<endl;

	 staticdata->calendar=myIO.loadCalendar();
	 cout << *(staticdata->calendar);


	 boost::shared_ptr<Calendar> cal2 = staticdata->calendar->getCalendar("SEONY");
	 cout <<cal2->is_Holiday(from_simple_string("2011-10-03"))<<endl;
	 cout <<cal2->is_Holiday(from_simple_string("2011-10-02"))<<endl;
	 cout <<cal2->is_Holiday(from_simple_string("2011-10-01"))<<endl;
	 cout <<cal2->is_Holiday(from_simple_string("2011-09-30"))<<endl;
	 cout <<cal2->is_Holiday(from_simple_string("2011-10-10"))<<endl;

	 staticdata->daterules=myIO.loadDateRules();
	 boost::shared_ptr<MarketData> mr_dat;
	 boost::shared_ptr<DateRule> dr1 = staticdata->daterules->getDateRule("3M");
	 boost::shared_ptr<DateRule> dr2 = staticdata->daterules->getDateRule("FX SPOT");
	 date dd1=from_simple_string("2011-09-30");
	 date dd2=from_simple_string("2011-11-25");

	 cout<< dd1<<" =>  " <<(*dr1)(dd1,cal2) << endl;
	 cout<< dd1<<" =>  " <<(dr2)->operator()(dd1,cal2) << endl;
	 cout << "----"<<endl;
	 cout<< dd2<<" =>  " <<(*dr1)(dd2,cal2) << endl;
	 cout<< dd2<<" =>  " <<(dr2)->operator()(dd2,cal2) << endl;

	 mr_dat=myIO.loadMarketData();
	 cout<< mr_dat->getRate("USDKRW")->rate << endl;
	 cout<< mr_dat->getRate("EURUSD")->rate << endl;
	 cout<< mr_dat->getRate("KRW ON")->rate << endl;
	 cout<< mr_dat->getRate("KRW 1M")->rate << endl;
	 cout<< mr_dat->getRate("KRW 3M")->rate << endl;
	 cout<< mr_dat->getRate("KRW 2Y")->rate << endl;

	 std::string tnor;
	 std::vector<int> i;
	 std::vector<std::string> str;

	 date outdate;
	 date tdy=from_simple_string("2011-11-11");

	 cout<<"enter a tenor : ";

	 while(getline(cin,tnor))
	   {
		 cout<< "result : " << parse_tenor(tnor.begin(),tnor.end(),tdy,outdate,mr_dat->getRateConvention("USDKRW FX"),*staticdata,2,1)<< " -> " << outdate << endl;
		 cout<<"enter a tenor : ";
	   }
 return 0;
}
