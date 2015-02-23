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

  // psk::mds::MarketData test;
  // shared_ptr<Rate> a(new Rate);
  // a->ticker="KRW Curncy";
  // a->rate=3.2;

  // shared_ptr<Rate> b(new Rate);
  // b->ticker="SGD Curncy";
  // b->rate=1.2000;

  // // cout<< a->ticker << endl << a->rate<<endl;
  
  // // test.addRate(a);
  // // test.addRate(b);

  // // test.setRate("JPY Curncy",76.2);
  // // cout << test.getRate("KRW Curncy")->rate << endl;
  // // cout << test.getRate("JPY Curncy")->rate << endl;

  // // char cmd[256];
  // // do
  // //   {
  // //     cin.getline(cmd,256);
  // //     cout<< test.getRate(cmd)->rate<<endl;
  // //   }while(cmd!="quit\n");
  // date_duration dd(10);
  // date tdy=day_clock::local_day();
  // date d1(from_simple_string("2012-10-1"));
  // date d2(from_simple_string("2013-10-1"));
  // cout << to_iso_extended_string(d1) << endl;
  // cout << d2-d1 << endl;
  // cout << d1 +dd << endl;
  // cout << d1 -days(10) << endl;
  // cout << ((d1<d2)?1:2) << endl;
  // cout << tdy<<endl;

  // map<date,double> tmp;

  // tmp[d1]=0.950;
  // tmp[d2]=0.890;
  // tmp[d2+days(365)]=0.840;

  // print_map(tmp); //map is SORTED and associated container., good for curve and cashflow

  // shared_ptr<SimpleCurve> scv(new SimpleCurve);
  // Curve* cv;
  // cv=(Curve*)scv.get();

  // scv->setDiscountCurve(tmp);
  
  // date dt1(from_simple_string("2012-05-06"));
  // dt1=d2+days(6000);
  // //dt1=day_clock::local_day()-days(1);
  // double df1,df2;
  // df1=scv->getDiscountFactor(dt1);
  // df2=cv->getDiscountFactor(dt1);
  // cout << dt1 << " : " << df1 << endl;
  // cout << dt1 << " : " << cv->operator()(dt1) << endl;

  // load_Basis();
  // register_Basis("Act360",basis_Act365);

  // map_Basis["Act365"]=basis_Act365;
  // cout << basis_Act365(d1,d2)<<endl;
  // cout << map_Basis["Act365"](d1,d2) << endl;

  // //set test
  // set<date> s;
  // s.insert(d2);
  // s.insert(d1);
  
  // shared_ptr<Holidays> hol(new Holidays);
  // hol->register_Holiday("KRW",d1);
  // cout << hol->is_SpecialHoliday("KRW",d1)<<endl;
  // cout << hol->is_SpecialHoliday("KRW",d2)<<endl;

  // for(set<date>::iterator it=s.begin();it!=s.end();it++)
  //   cout << *it << endl;
  
  // cout<<(d1+days(3)).day_of_week()<<endl;

  // date d3(from_simple_string("2011-9-18"));
  // vector<string> ccys;
 
  // ccys.push_back("KRW");
  // ccys.push_back("USD");
  // Calendar cal1(ccys,hol);
  // cout << cal1.is_Holiday(d1)<<endl;
  // cout << cal1.is_Holiday(d1+days(3))<<endl;
  // cout << cal1.is_Holiday(d1+days(4))<<endl;
  // cout << cal1.is_Holiday(d1+days(5))<<endl;
  date d1(from_simple_string("2012-10-01"));
  date d2(from_simple_string("2013-10-01"));
  // cout << pow(2,0.5)<<endl;


  //load up conventions, static data
  shared_ptr<Basis> basis(new Basis);
  shared_ptr<RateConvention> rc(new RateConvention);
  // rc->basis="Act365";
  //rc->pricingdate=d1-days(1);
  shared_ptr<StaticData> staticdata(new StaticData); // all static data (basis, holidays, calendar, ...)
  staticdata->basis=basis;
  shared_ptr<Holidays> hols(new Holidays);
  vector<std::string> ccys;
  shared_ptr<Calendar> cal1(new Calendar(ccys,hols,true,true));

  //load up instruments
  shared_ptr<Depo_Instrument> depo1(new Depo_Instrument(rc,staticdata));
  shared_ptr<Instrument> inst1(depo1);
  //load up market data
 
  //load curves
  map<date,double> df_map;df_map[d1]=0.95;df_map[d2]=0.90;
  shared_ptr<SingleRatesCurve> cv(new SingleRatesCurve);
  cv->setDiscountCurve(df_map);

  shared_ptr<Curve> basecv;//SimpleCurve shd be out of public inheritance from Curve 
  basecv=cv;

  // cout << d2+months(13) << endl;
  // cout << d2+years(-1) << endl;
  // cout << d2.year()<<endl;
  // cout <<d2.month()<<endl;
  // cout <<d2.day()<<endl;

  Curve* cv2=(Curve*)cv.get();
  shared_ptr<BaseObject> baseobj(cv);

  d2=date(from_simple_string("2011-06-25"));
  date d3,d4;

  shared_ptr<DateShifter> dshift1(new DateShifter(m,1,true,ModifiedFollowing));
  shared_ptr<DateShifter> dshift2(new DateShifter(m,3,true,Indifferent));
  shared_ptr<DateShifter> dshift3(new DateShifter(od,0,false,Preceding));
  d3=dshift1->operator()(d2,cal1);
  d4=dshift2->operator()(d2,cal1);

  cout <<d2 <<" " <<d2.day_of_week()<< "->" <<d3 <<" " <<d3.day_of_week() <<endl;
  cout <<d2 <<" " <<d2.day_of_week()<< "->" <<d4 <<" " <<d4.day_of_week() <<endl;

  shared_ptr<CDateRule> crule1(new CDateRule);
  crule1->push_back(dshift1);
  crule1->push_back(dshift2);

  shared_ptr<DateRuleData> ruledata(new DateRuleData);

  ruledata->register_DateRule("1m modfol",dshift1);
  ruledata->register_DateRule("3m Prev",dshift2);
  ruledata->register_DateRule("4m modfol",crule1);
  ruledata->register_DateRule("spot",dshift3);

  cout<<d2<<" "<<d2.day_of_week()<< "->" << (*ruledata)["1m modfol"]->operator()(d2,cal1)<<endl;
  cout<<d2<<" "<<d2.day_of_week()<< "->" << (*ruledata)["3m Prev"]->operator()(d2,cal1)<<endl;
  cout<<d2<<" "<<d2.day_of_week()<< "->" << (*ruledata)["4m modfol"]->operator()(d2,cal1)<<endl;
  cout<<d2<<" "<<d2.day_of_week()<< "->" << (*ruledata)["spot"]->operator()(d2,cal1)<<endl;
  cout<<d2<<" "<<d2.day_of_week()<< "->" << (*ruledata)["spot"]->operator()(d2,cal1)<<endl;
  cout<<d2<<" "<<d2.day_of_week()<< "->" << crule1->operator()(d2,cal1)<<endl;

  

  //test


  for(int i=0;i<15;i++)
    {
      //cout << depo1->operator()(basecv,d2+days(365*i))*100<<endl;
      //cout << inst1->operator()(basecv,d2+days(365*i))*100<<endl;
      //cout << ((SimpleCurve*)baseobj.get())->getDiscountFactor(d2) <<endl;
    }
  //iotest
  
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

 cout<< mr_dat->getRate("USDKRW")->rateconvention->conventions["paymentcalendar"] << endl;
 cout<< mr_dat->getRate("EURUSD")->rateconvention->conventions["paymentcalendar"] << endl;
 cout<< mr_dat->getRate("EURUSD")->rateconvention->conventions["basecurrency"] << endl;

 shared_ptr<InstrumentData> inst_data;
 inst_data=myIO.loadInstruments();
 shared_ptr<CurveData> curve_data=myIO.loadCurves(); 

 
 std::map<boost::gregorian::date,double> disc_factors;

 disc_factors[from_simple_string("2011-11-1")]=1.0; 
 disc_factors[from_simple_string("2012-11-1")]=0.97; 
 disc_factors[from_simple_string("2013-11-1")]=0.95;
 disc_factors[from_simple_string("2014-11-1")]=0.85;

 boost::shared_ptr<Instrument> depo_inst=inst_data->getInstrument("KRW Depo");
 boost::shared_ptr<Instrument> irs_inst=inst_data->getInstrument("KRW IRS");

 cout << (*depo_inst)(disc_factors,from_simple_string("2011-11-1"),from_simple_string("2012-11-1")) << endl;
 cout << (*irs_inst)(disc_factors,from_simple_string("2011-11-1"),from_simple_string("2012-11-1")) << endl;
 cout << (*irs_inst)._solve(disc_factors,from_simple_string("2011-11-1"),from_simple_string("2012-11-1"), 0.0304919) << endl;


 //CURVE TEST
 boost::shared_ptr<psk::curve::Curve> mycurve=curve_data->getCurve("KRW STD");
 boost::shared_ptr<psk::core::DiscountFactors> mydf=mycurve->dumpDiscountFactors();
 psk::core::DiscountFactors::iterator mydfit=mydf->begin();

 cout<<"KRW STD\n----------------\n";
 for(mydfit;mydfit!=mydf->end();mydfit++)
   cout << mydfit->first << "\t" << mydfit->second<<endl;


 cout <<"-----------------------------------\n";
 cout << (*depo_inst)(*mydf,from_simple_string("2011-11-16"),from_simple_string("2012-3-16")) << endl;
 cout << (*irs_inst)(*mydf,from_simple_string("2011-11-16"),from_simple_string("2012-3-16")) << endl;
 cout << (*irs_inst)(*mydf,from_simple_string("2011-11-16"),from_simple_string("2013-11-18")) << endl;
 cout << (*irs_inst)(*mydf,from_simple_string("2011-11-16"),from_simple_string("2014-11-18")) << endl;
 cout << (*depo_inst)(*mydf,from_simple_string("2011-11-16"),from_simple_string("2014-11-18")) << endl;
 

 cout<<"----------------------------------\n";

 myIO.loadInstruments_Second();
 myIO.loadCurves_Second();
 
 boost::shared_ptr<psk::curve::Curve> mycurve3=curve_data->getCurve("USD STD");
 boost::shared_ptr<psk::core::DiscountFactors> mydf3=mycurve3->dumpDiscountFactors();
 psk::core::DiscountFactors::iterator mydfit3=mydf3->begin();

 cout<<"USD STD\n----------------\n";
 for(mydfit3;mydfit3!=mydf3->end();mydfit3++)
   cout << mydfit3->first << "\t" << mydfit3->second<<endl;

 boost::shared_ptr<psk::curve::Curve> mycurve2=curve_data->getCurve("USDKRW FX");
 boost::shared_ptr<psk::core::DiscountFactors> mydf2=mycurve2->dumpDiscountFactors();
 psk::core::DiscountFactors::iterator mydfit2=mydf2->begin();

 cout<<"KRW FX \n----------------\n";
 for(mydfit2;mydfit2!=mydf2->end();mydfit2++)
   cout << mydfit2->first << "\t" << mydfit2->second<<endl;

 date tdyxx=day_clock::local_day()+days(2);
 date matxx=tdyxx+months(24);

 cout << inst_data->getInstrument("USDKRW SWAPPOINT")->_price(*mydf2,tdyxx,matxx) << endl;
 cout << inst_data->getInstrument("KRW Depo")->_price(*mydf2,tdyxx,matxx) << endl;

 cout << inst_data->getInstrument("USDKRW SWAPPOINT")->_solve(*mydf2,tdyxx,matxx,2.1) << endl;

 cout << inst_data->getInstrument("USDKRW CCS")->_price(*mydf2,tdyxx,matxx) << endl;
 cout << inst_data->getInstrument("USDKRW CCS")->_price(*mydf2,tdyxx,matxx+years(1)) << endl;



using namespace psk::parser;
 
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
