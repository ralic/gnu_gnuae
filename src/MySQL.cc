// 
//   Copyright (C) 2009 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

// This is generated by autoconf
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <iostream>
#include <iomanip> 
#include <vector>
#include <sys/time.h>
#include <cstdio>

#ifdef __STDC_HOSTED__
#include <sstream>
#else
#include <strstream>
#endif

using namespace std;

#include "Database.h"
#include "log.h"

#ifdef HAVE_MYSQL
#include <mysql/errmsg.h>
#include <mysql/mysql.h>
#endif

namespace gnuae {

static LogFile& dbglogfile = LogFile::getDefaultInstance();

const int LINELEN = 80;
const int QUERYLEN = 10000;

Database::Database()
    : _dbtype(NODB), _dbport(0)
{

    // These values may be replaced on the command line. These are the
    // default behaviour.
    _tblname  = DBTABLE;
    _dbname   = DBNAME;
    _dbuser   = DBUSER;
    _dbpasswd = DBPASS;
    _dbhost   = DBHOST;
}

Database::~Database()
{
  if (_connection) {
    //closeDB();
  }
}

// Accessors
void
Database::dbUserSet(std::string user)
{
    _dbuser = user;
}

void
Database::dbPasswdSet(std::string passwd)
{
    _dbpasswd = passwd;
}

void
Database::dbNameSet(std::string name)
{
    _dbname = name;
}

void
Database::dbHostSet(std::string host)
{
    _dbhost = host;
}

// methods that actually do something
bool
Database::openDB (void)
{
  // DEBUGLOG_REPORT_FUNCTION;

    unsigned long flag = 0;

    dbglogfile <<"Connecting to MySQL database "  << _dbname.c_str()
               << " on " << _dbhost.c_str()
               << " as user " << _dbuser.c_str() << endl;

    mysql_init(&_mysql);
    _connection = mysql_real_connect(&_mysql, _dbhost.c_str(), _dbuser.c_str(),
        _dbpasswd.c_str(), _dbname.c_str(), 0, NULL, flag);

    /* check for a connection error */
    if(_connection == NULL) {
        /* print the error message */
      dbglogfile << "MySQL error when connecting: \n" << mysql_error(&_mysql) << endl;
        return false;
    }

    dbglogfile << "Connected to MySQL database " << _dbname.c_str()
               <<  " on host " <<_dbhost.c_str() << endl;

#if 0
    if(mysql_select_db(&_mysql, "mydb")){ 
        DBG_MSG(DBG_EROR, "%s: MySQL error when selecting database: %s\n",
            (mysql_error(&_mysql)));
        exit(1);
    }
#endif
    return true;
}


bool
Database::closeDB (void)
{
    mysql_close(&_mysql);

    // FIXME: do something intelligent here
    return true;
}

#if 0
bool
Database::queryInsert(const char *query)
{
  DEBUGLOG_REPORT_FUNCTION;

  int retries, result;
  
  retries = 2;
  
  dbglogfile << "Query is: " << query << endl;
  
#if 0
  string str = query;
  // FIXME: We shouldn't ever get this condition
  if (str.find("INSERT", 10) != string::npos) {        
    dbglogfile << "Extra INSERT in query!\n", query << endl;
  }
#endif
  
  while (retries--) {
    result = mysql_real_query(&_mysql, query, strlen(query));
    
    switch (result) {
    case CR_SERVER_LOST:
    case CR_COMMANDS_OUT_OF_SYNC:
    case CR_SERVER_GONE_ERROR:
      dbglogfile << "MySQL connection error: " << mysql_error(&_mysql) << endl;
      // Try to reconnect to the database
      closeDB();
      openDB();
      continue;
      break;
    case -1:
    case CR_UNKNOWN_ERROR:
      dbglogfile << "MySQL error on query for:\n\t " <<
        mysql_error(&_mysql) << endl;
      dbglogfile << "Query was: " << query << endl;
            return false;
            break;            
    default:
      return true;
    }
    
    dbglogfile << "Lost connection to the database server, shutting down..." << endl;
    
    return false;
  }
  
  return false;
}

  
void *
Database::queryResults(const char *query)
{
  DEBUGLOG_REPORT_FUNCTION;

    MYSQL_RES	*result;
    MYSQL_ROW	row;
    //    MYSQL_FIELD *fields;
    int         nrows;
    unsigned int i, res;

    dbglogfile << "Query is: " << query << endl;

    res = mysql_real_query(&_mysql, query, strlen(query));
    
    switch (res)
    {
      case CR_SERVER_LOST:
      case CR_COMMANDS_OUT_OF_SYNC:
      case CR_SERVER_GONE_ERROR:
        dbglogfile << "MySQL connection error: "
             << mysql_error(&_mysql) << endl;
        // Try to reconnect to the database
        closeDB();
        openDB();
        break;
      case CR_UNKNOWN_ERROR:
        dbglogfile << "MySQL error on query for:" << mysql_error(&_mysql) << endl;
//        ACE_DEBUG((LM_INFO, "Query was: %s\n", query));
//        return false;
//      default:
//        return true;
    }
    
    
    result = mysql_store_result(&_mysql);
    nrows = mysql_num_rows(result);

    row = mysql_fetch_row(result);
    mysql_num_fields(result);      

    while((row = mysql_fetch_row(result)))
    {
        for (i=0; i<mysql_num_fields(result); i++)
        {
//        fields = mysql_fetch_fields(result);
//        ACE_DEBUG((LM_INFO, "%s: ", field->name));
//            ACE_DEBUG((LM_INFO, "Row is: %s\n", row[i]));
        }
    }

    mysql_free_result(result);

    // FIXME: return something intelligent here
    return (void *)0;
}

bool
Database::queryInsert(vector<meter_data_t *> data)
{
  DEBUGLOG_REPORT_FUNCTION;

  struct tm      *ttm;
  struct timeval tp;
  unsigned int   i;
  //char           query[QUERYLEN];
  //char           *ptr;
#ifdef __STDC_HOSTED__
  ostringstream  query;
#else
  ostrstream     query;
#endif

  if (data.size() == 0) {
    dbglogfile << "No data to insert." << endl;
    return false;
  }
  
  gettimeofday(&tp, 0);
  ttm = localtime (&tp.tv_sec);
  ttm->tm_year+= 1900;          // years since 1900
  ttm->tm_mon+=1;               // months since January

  query << "INSERT INTO inverter () VALUES ";

  for (i=0; i< data.size(); i++) {
    queryInsert(data[i]);
  }
  
  return true;  
}
  
bool
Database::queryInsert(meter_data_t *data)
{
  DEBUGLOG_REPORT_FUNCTION;

  struct tm             *ttm;
  struct timeval        tp;
  //char                  query[QUERYLEN];
  char                  *type = "MX";
#ifdef __STDC_HOSTED__
  std::ostringstream    query;
#else
  std::ostrstream       query;
#endif

  query.str("");
  //  memset (query, 0, QUERYLEN);
  
  // EtaMsg em;
  // em.dump(*data);
  
  gettimeofday(&tp, 0);
  ttm = localtime (&tp.tv_sec);
  ttm->tm_year+= 1900;          // years since 1900
  ttm->tm_mon+=1;               // months since January

#if 0  
  // FIXME: For now source is the facility
  // Build the query string to insert the data
  sprintf(query, "INSERT INTO meters () VALUES ( \
        '%d',                   // Unit address (int)\
        '%s',                   // Device Type enum)\
        '%d-%d-%d %d:%d:%d',    // timestamp\
        '%d',                   // Charger Amps (int)\
        '%d',                   // AC Load Amps (int)\
        '%f',                   // Battery Volts (float)\
        '%f',                   // AC Volts Out (float)\
        '%f',                   // AC1 Volts In (float)\
        '%f',                   // AC2 Volts In (float)\
        '%d',                   // PV Amps In (int)\
        '%f',                   // PV Volts In (float)\
        '%f',                   // Buy Amps (int)\
        '%f',                   // Sell Amps (int)\
        '%f',                   // Dail Kwh (float)\
        '%d',                   // Frequency in Hertz (int)\
        '%f'                    // Battery Temperature Compenation (float)\
        )",
          //  sprintf(query, "INSERT INTO meters () VALUES ('%d','%s','%0.4d-%0.2d-%d %0.2d:%0.2d:%0.2d','%d','%d','%f','%f','%f','%f','%d','%f','%f','%f','%d','%f','%f')",
          data->unit,
          type,
          ttm->tm_year,ttm->tm_mon,ttm->tm_mday,ttm->tm_hour,ttm->tm_min,ttm->tm_sec,
          data->charge_amps,
          data->ac_load_amps,
          data->battery_volts,
          data->ac_volts_out,
          data->ac1_volts_in,
          data->ac2_volts_in,
          data->pv_amps_in,
          data->pv_volts_in,
          data->buy_amps,
          data->sell_amps,
          data->daily_kwh,
          data->hertz,
          data->tempcomp_volts
          );
#else
  query << "INSERT INTO meters () VALUES (";
  query << data->unit << ",";
  query << "\'" << type << "\',";
  query << "\'" << ttm->tm_year << "-" << ttm->tm_mon << "-" << ttm->tm_mday << " ";
  query << ttm->tm_hour << ":" << ttm->tm_min << ":" << ttm->tm_sec << "\',";
  query << data->charge_amps << ",";
  query << data->ac_load_amps << ",";
  query << data->battery_volts << ",";
  query << data->ac_volts_out << ",";
  query << data->ac1_volts_in << ",";
  query << data->ac2_volts_in << ",";
  query << data->pv_amps_in << ",";
  query << data->pv_volts_in << ",";
  query << data->buy_amps << ",";
  query << data->sell_amps << ",";
  query << data->daily_kwh << ",";
  query << data->hertz << ",";
  query << data->tempcomp_volts << ")";
  query << ends;
#endif

#ifdef __STDC_HOSTED__
  string str = query.str().c_str();
#else
  string str = query.str();
#endif
  
  // Execute the query
  queryInsert(str.c_str());
  
  return true;
}
#endif

char *
Database::gettime()
{
    struct timeval tp;
    struct tm *tm, result;
    static char tmpbuf[30];
 
    gettimeofday(&tp, 0);
    tm = localtime_r(&(tp.tv_sec), &result);
    asctime(tm);
    memset(tmpbuf, 0, 20);
     
    strftime(tmpbuf, 20, "%Y-%m-%d %H:%M:%S", tm);
 
//    DBG_MSG(DBG_INFO, "TIMESTAMP is %s\n", tmpbuf);
    return tmpbuf;
}

} // end of gnuae namespace

// local Variables:
// mode: C++
// indent-tabs-mode: t
// End:
