#include "io.hpp"
#include <stdlib.h>
namespace psk
{
  namespace io
  {
    //Base class IOHandler

    //IOHandlerMySQL using libmysql



    //IOHandlerXML using tinyXML

    bool IOHandlerXML::loadFileMap(std::string filename)
    {
      if(xmldoc.LoadFile(filename.c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* fmap=root->FirstChildElement();fmap;fmap=fmap->NextSiblingElement())
	    {
	      //	      TiXmlElement* fn=fmap->FirstChildElement();
	      filemap[fmap->Attribute("name")]=fmap->FirstChild()->Value();
	    }

	  return true;
	}
      else
	{
	  return false;
	}
    }


    boost::shared_ptr<psk::marketdata::MarketData> IOHandlerXML::loadMarketData()
    {
      mr_data=boost::shared_ptr<psk::marketdata::MarketData>(new psk::marketdata::MarketData);
      std::string type;
      std::string rc;
      boost::shared_ptr<psk::staticdata::RateConvention> rcobj;
      boost::shared_ptr<psk::marketdata::Rate> rateobj;
      std::map<std::string,std::string> xmlparams;

      //first parsing (rateconvention)
     if(xmldoc.LoadFile(filemap["marketdata"].c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* elm=root->FirstChildElement();elm;elm=elm->NextSiblingElement())
	    {
	      type=elm->Attribute("type");
	      rc=elm->Attribute("name");
	      xmlparams.clear();
	      if(type=="rateconvention") // rate data
		{
		  rcobj=boost::shared_ptr<psk::staticdata::RateConvention>(new psk::staticdata::RateConvention);
		  rcobj->name=rc;
		  for(TiXmlElement* lastnode=elm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
		    {
		      xmlparams[std::string(lastnode->Value())]=lastnode->FirstChild()->Value();
		    }
		  rcobj->conventions=xmlparams;
		  mr_data->addRateConvention(rcobj);
		}

	    }
	} //end loading rate (1st parse)
     

     //2nd parsing (rates)

      if(xmldoc.LoadFile(filemap["marketdata"].c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* elm=root->FirstChildElement();elm;elm=elm->NextSiblingElement())
	    {
	      type=elm->Attribute("type");
	      rc=elm->Attribute("name");
	      xmlparams.clear();
	      if(type=="rate") // rate data
		{
		  rateobj=boost::shared_ptr<psk::marketdata::Rate>(new psk::marketdata::Rate);
		  rateobj->name=rc;
		  for(TiXmlElement* lastnode=elm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
		    {
		      xmlparams[std::string(lastnode->Value())]=lastnode->FirstChild()->Value();
		    }
		  rateobj->tenor=xmlparams["tenor"];
		  rateobj->start=xmlparams["start"];
		  rateobj->rate=(double)atof(xmlparams["value"].c_str());
		  rateobj->rateconvention=mr_data->getRateConvention(xmlparams["rc"]);
		  mr_data->addRate(rateobj);
		}
	    }
	} //end loading rate (2nd parse)
      return mr_data;
    }//end loadMarketdata()


    boost::shared_ptr<psk::staticdata::calendar::Holidays> IOHandlerXML::loadHolidays() // 1st load
    {
      std::string holname;
      boost::gregorian::date holiday;
      holdata=boost::shared_ptr<psk::staticdata::calendar::Holidays>(new psk::staticdata::calendar::Holidays);

      if(xmldoc.LoadFile(filemap["holidays"].c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* elm=root->FirstChildElement();elm;elm=elm->NextSiblingElement())
	    {
	      holname= elm->Attribute("name");
	      for(TiXmlElement* lastnode=elm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
		{
		  holiday=boost::gregorian::date(boost::gregorian::from_simple_string(lastnode->FirstChild()->Value()));
		  holdata->register_Holiday(holname,holiday);
		}

	    }
	}
      return holdata; 
    }

    boost::shared_ptr<psk::staticdata::calendar::CalendarData> IOHandlerXML::loadCalendar()
    {
      cal_data=boost::shared_ptr<psk::staticdata::calendar::CalendarData>(new psk::staticdata::calendar::CalendarData);
      std::string calname;
      std::vector<std::string> holnames;
      bool SatHol;
      bool SunHol;
      
      if(xmldoc.LoadFile(filemap["calendar"].c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* elm=root->FirstChildElement();elm;elm=elm->NextSiblingElement())
	    {
	      holnames.clear();
	      calname= elm->Attribute("name");
	      SatHol=(std::string(elm->Attribute("SatHol"))=="Y")?true:false;
	      SunHol=(std::string(elm->Attribute("SunHol"))=="Y")?true:false;

	      for(TiXmlElement* lastnode=elm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
		{
		  holnames.push_back(lastnode->FirstChild()->Value());
		}
	      cal_data->addCalendar(calname,boost::shared_ptr<psk::staticdata::calendar::Calendar>(new psk::staticdata::calendar::Calendar(holnames ,holdata,SatHol,SunHol)));	      
	    }
	}
      
      return cal_data;
    }

    boost::shared_ptr<psk::staticdata::calendar::DateRuleData> IOHandlerXML::loadDateRules() // 3rd load
    {
      dr_data=boost::shared_ptr<psk::staticdata::calendar::DateRuleData>(new psk::staticdata::calendar::DateRuleData);
      std::string drname;
      std::string drtype;
      std::vector<std::string> drnames; //for combined rules
 
      std::map<std::string,std::string> parameters;
      std::map<std::string,std::string>::iterator param_it;
      psk::staticdata::calendar::PeriodType pt; //period type (d,od,m,y)
      psk::staticdata::calendar::HolidayConvention hc; //holiday convention
      int iteration; //iteration
      bool applySpecHol;//apply speacial holiday

      //first parsing for simple rules (shifter,...)

      if(xmldoc.LoadFile(filemap["daterules"].c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* elm=root->FirstChildElement();elm;elm=elm->NextSiblingElement())
	    {
	      boost::shared_ptr<psk::staticdata::calendar::DateRule> drule;
	      drname=elm->Attribute("name");
	      drtype=elm->Attribute("type");
	      parameters.clear();
	      if (drtype=="shifter")
		{
		  for(TiXmlElement* lastnode=elm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
		    {
		      parameters[std::string(lastnode->Value())]=std::string(lastnode->FirstChild()->Value());
		    }
		  //process params
		  if(parameters["periodtype"]=="m") pt=psk::staticdata::calendar::m;
		  else if(parameters["periodtype"]=="od") pt=psk::staticdata::calendar::od;
		  else if(parameters["periodtype"]=="d") pt=psk::staticdata::calendar::d;
		  else if(parameters["periodtype"]=="w") pt=psk::staticdata::calendar::w;
		  else if(parameters["periodtype"]=="y") pt=psk::staticdata::calendar::y;
		  
		  if(parameters["holidayConvention"]=="Following") hc=psk::staticdata::calendar::Following;
		  else if(parameters["holidayConvention"]=="ModifiedFollowing") hc=psk::staticdata::calendar::ModifiedFollowing;
		  else if(parameters["holidayConvention"]=="Preceding") hc=psk::staticdata::calendar::Preceding;
		  else if(parameters["holidayConvention"]=="Indifferent") hc=psk::staticdata::calendar::Indifferent;

		  iteration=atoi(parameters["iteration"].c_str());
		  applySpecHol=(parameters["applyHoliday"]=="Y")? true:false;
		  
		  drule=boost::shared_ptr<psk::staticdata::calendar::DateRule>(new psk::staticdata::calendar::DateShifter(pt,iteration,applySpecHol,hc));
		  dr_data->register_DateRule(drname , drule);
		}//end shifter if
	      else if(drtype=="OTHER RULE // to implement")
		{
		}//end other if
	    }//end for elm
	}//end if xmldoc

      //second parsing for combined rules (combined rules)
      if(xmldoc.LoadFile(filemap["daterules"].c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* elm=root->FirstChildElement();elm;elm=elm->NextSiblingElement())

	    {
	      drname=elm->Attribute("name");
	      drtype=elm->Attribute("type");
	      parameters.clear();
	      boost::shared_ptr<psk::staticdata::calendar::CDateRule> drule;

	      if (drtype=="combined")
		{
		  for(TiXmlElement* lastnode=elm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
		    {
		      parameters[std::string(lastnode->Attribute("no"))]=std::string(lastnode->FirstChild()->Value());
		    }
		  //process params
		  drule=boost::shared_ptr<psk::staticdata::calendar::CDateRule>(new psk::staticdata::calendar::CDateRule);	
		  for(param_it=parameters.begin();param_it!=parameters.end();param_it++)
		    {
		       drule->push_back(dr_data->getDateRule(param_it->second));
		    }
		  dr_data->register_DateRule(drname , drule);
		}//end if combined

	    }//end for elm
	}//end if xmldoc
      return dr_data;
    }

    //Instrument load

    boost::shared_ptr<psk::instrument::InstrumentData> IOHandlerXML::loadInstruments()
    {
      inst_data=boost::shared_ptr<psk::instrument::InstrumentData>(new psk::instrument::InstrumentData);
      boost::shared_ptr<psk::instrument::Instrument> inst_ptr;
      std::string type;
      std::string instname;
      boost::shared_ptr<psk::staticdata::RateConvention> rcobj;
      boost::shared_ptr<psk::marketdata::Rate> rateobj;
      std::map<std::string,std::string> xmlparams;
      staticdata=boost::shared_ptr<psk::staticdata::StaticData>(new psk::staticdata::StaticData);
      staticdata->basis=boost::shared_ptr<psk::staticdata::basis::Basis>(new psk::staticdata::basis::Basis);
      staticdata->holidays=holdata;
      staticdata->calendar=cal_data;
      staticdata->daterules=dr_data;
  
     if(xmldoc.LoadFile(filemap["instruments"].c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* elm=root->FirstChildElement();elm;elm=elm->NextSiblingElement())
	    {
	      type=elm->Attribute("type");
	      instname=elm->Attribute("name");
	      xmlparams.clear();  
	      if(type=="DEPO") // depo instrument
		{

		  for(TiXmlElement* lastnode=elm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
		    {
		      xmlparams[std::string(lastnode->Value())]=lastnode->FirstChild()->Value();
		    }
		  inst_ptr=boost::shared_ptr<psk::instrument::Instrument>(new psk::instrument::Depo_Instrument(mr_data->getRateConvention(xmlparams["rc"]),staticdata));
		  inst_data->addInstrument(instname, inst_ptr);
		}
	      if(type=="IRS") // IRS instrument
		{

		  for(TiXmlElement* lastnode=elm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
		    {
		      xmlparams[std::string(lastnode->Value())]=lastnode->FirstChild()->Value();
		    }
		  inst_ptr=boost::shared_ptr<psk::instrument::Instrument>(new psk::instrument::IRS_Instrument(mr_data->getRateConvention(xmlparams["rc"]),staticdata));
		  inst_data->addInstrument(instname, inst_ptr);
		}

	    }//end elm
	}
     return inst_data;
    }//end loading intruments
    
    // 2ND INSTRUMENTS LOAD (after 1st curve)
    boost::shared_ptr<psk::instrument::InstrumentData> IOHandlerXML::loadInstruments_Second()
    {
      boost::shared_ptr<psk::instrument::Instrument> inst_ptr;
      std::string type;
      std::string instname;
      std::string ref_cv_name, baseccy;
      boost::shared_ptr<psk::core::DiscountFactors> df;
      boost::shared_ptr<psk::staticdata::RateConvention> rcobj;
      boost::shared_ptr<psk::marketdata::Rate> rateobj;
      std::map<std::string,std::string> xmlparams;
    
      if(xmldoc.LoadFile(filemap["instruments"].c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* elm=root->FirstChildElement();elm;elm=elm->NextSiblingElement())
	    {
	      type=elm->Attribute("type");
	      instname=elm->Attribute("name");
	      xmlparams.clear();  
	      if(type=="SWAPPOINT") // swap point instrument
		{  

		  for(TiXmlElement* lastnode=elm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
		    {
		      xmlparams[std::string(lastnode->Value())]=lastnode->FirstChild()->Value();
		    }
		  baseccy=xmlparams["baseccy"];
		  ref_cv_name=xmlparams["ref_curve"];
		  rateobj=mr_data->getRate(xmlparams["ref_spot"]);
		  df=curve_data->getCurve(ref_cv_name)->dumpDiscountFactors();
		  inst_ptr=boost::shared_ptr<psk::instrument::Instrument>(new psk::instrument::Swappoint_Instrument(mr_data->getRateConvention(xmlparams["rc"]),staticdata,df,rateobj,baseccy,ref_cv_name));
		  inst_data->addInstrument(instname, inst_ptr);
		}
	      if(type=="CCS") // cross rate instrument
		{  

		  for(TiXmlElement* lastnode=elm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
		    {
		      xmlparams[std::string(lastnode->Value())]=lastnode->FirstChild()->Value();
		    }
		  baseccy=xmlparams["baseccy"];
		  ref_cv_name=xmlparams["ref_curve"];
		  rateobj=mr_data->getRate(xmlparams["ref_spot"]);
		  df=curve_data->getCurve(ref_cv_name)->dumpDiscountFactors();
		  inst_ptr=boost::shared_ptr<psk::instrument::Instrument>(new psk::instrument::CCS_Instrument(mr_data->getRateConvention(xmlparams["rc"]),staticdata,df,rateobj,baseccy,ref_cv_name));
		  inst_data->addInstrument(instname, inst_ptr);
		}

	    }//end elm
	}
      return inst_data;
    }//end loading intruments_Second
 
    //Loading Curves
    boost::shared_ptr<psk::curve::CurveData> IOHandlerXML::loadCurves()
    {
      curve_data=boost::shared_ptr<psk::curve::CurveData>(new psk::curve::CurveData);
      boost::shared_ptr<psk::instrument::Instrument> inst_ptr;
      boost::shared_ptr<psk::curve::Curve> curve_ptr;
      boost::shared_ptr<psk::marketdata::Rate> rate_ptr;
      boost::shared_ptr<psk::staticdata::RateConvention> rcobj;
      psk::curve::Curvelet cvlet;

      std::string type;
      std::string curvename;

      std::map<std::string,std::string> xmlparams;

      //first parsing (Single Curves)
      if(xmldoc.LoadFile(filemap["curves"].c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* elm=root->FirstChildElement();elm;elm=elm->NextSiblingElement())
	    {
	      type=elm->Attribute("type"); 
	      curvename=elm->Attribute("name"); 

	      if(type=="SingleRatesCurve") // Single Curves
		{
		  curve_ptr=boost::shared_ptr<psk::curve::Curve>(new psk::curve::SingleRatesCurve(staticdata));
		  for(TiXmlElement* secondelm=elm->FirstChildElement();secondelm;secondelm=secondelm->NextSiblingElement())//curvelets
		    {
		      xmlparams.clear();
		      for(TiXmlElement* lastnode=secondelm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
			{
			  xmlparams[std::string(lastnode->Value())]=lastnode->FirstChild()->Value(); 
			}
		      inst_ptr=inst_data->getInstrument(xmlparams["instrument"]);
		      rate_ptr=mr_data->getRate(xmlparams["rate"]);
		      cvlet.rate=rate_ptr; 
		      cvlet.instrument=inst_ptr; 
		      curve_ptr->addCurvelet(cvlet,*staticdata);
		    }//endFOR secondelm
		  curve_ptr->bootstrap();
		  curve_data->registerCurve(curvename,curve_ptr);
		}//endIF SingleRates curve

	      //to add curve here

	    }//endFOR elm 
	}// end first parsing

      //SECOND PARSING to be implemented below

      return curve_data;
    }//end loading curves

 boost::shared_ptr<psk::curve::CurveData> IOHandlerXML::loadCurves_Second()
    {
      //curve_data=boost::shared_ptr<psk::curve::CurveData>(new psk::curve::CurveData);
      boost::shared_ptr<psk::instrument::Instrument> inst_ptr;
      boost::shared_ptr<psk::curve::Curve> curve_ptr;
      boost::shared_ptr<psk::marketdata::Rate> rate_ptr;
      boost::shared_ptr<psk::staticdata::RateConvention> rcobj;
      psk::curve::Curvelet cvlet;

      std::string type;
      std::string curvename;

      std::map<std::string,std::string> xmlparams;

      //first parsing (Single Curves)
      if(xmldoc.LoadFile(filemap["curves"].c_str()))
	{
	  TiXmlElement* root=xmldoc.RootElement();
	  for(TiXmlElement* elm=root->FirstChildElement();elm;elm=elm->NextSiblingElement())
	    {
	      type=elm->Attribute("type"); 
	      curvename=elm->Attribute("name"); 

	      if(type=="DualRatesCurve") // Single Curves
		{
		  curve_ptr=boost::shared_ptr<psk::curve::Curve>(new psk::curve::SingleRatesCurve(staticdata));
		  for(TiXmlElement* secondelm=elm->FirstChildElement();secondelm;secondelm=secondelm->NextSiblingElement())//curvelets
		    {
		      xmlparams.clear();
		      for(TiXmlElement* lastnode=secondelm->FirstChildElement();lastnode;lastnode=lastnode->NextSiblingElement())
			{
			  xmlparams[std::string(lastnode->Value())]=lastnode->FirstChild()->Value(); 
			}
		      inst_ptr=inst_data->getInstrument(xmlparams["instrument"]);
		      rate_ptr=mr_data->getRate(xmlparams["rate"]);
		      cvlet.rate=rate_ptr; 
		      cvlet.instrument=inst_ptr; 
		      curve_ptr->addCurvelet(cvlet,*staticdata);
		    }//endFOR secondelm
		  curve_ptr->bootstrap();
		  curve_data->registerCurve(curvename,curve_ptr);
		}//endIF SingleRates curve

	      //to add curve here

	    }//endFOR elm 
	}// end first parsing

      //SECOND PARSING to be implemented below

      return curve_data;
    }//end 2nd loading curves


    void IOHandlerXML::printfmap()
    {
      std::map<std::string,std::string>::iterator it=filemap.begin();
      for(it=filemap.begin();it!=filemap.end();it++)
	std::cout << it->first << " " << it->second << std::endl;
    }
  }//end io

}//end psk
