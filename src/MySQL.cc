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
#include <cstdlib>

#ifdef __STDC_HOSTED__
#include <sstream>
#else
#include <strstream>
#endif

using namespace std;

#include "Database.h"
#include "log.h"
#include "gnuae.h"

#ifdef HAVE_MYSQL
#include <mysql/errmsg.h>
#include <mysql/mysql.h>
#endif

namespace gnuae {

static LogFile& dbglogfile = LogFile::getDefaultInstance();

const int LINELEN = 80;
const int QUERYLEN = 10000;

Database::Database()
    : _dbtype(NODB),
      _dbport(0),
      _tblname(DBTABLE),
      _dbname(DBNAME),
      _dbuser(DBUSER),
      _dbpasswd(DBPASS),
      _dbhost(DBHOST),
      _connection(0)
{
    
}

Database::~Database()
{
  if (_connection) {
      closeDB();
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
    return openDB(_dbhost, _dbuser, _dbpasswd);
}

bool
Database::openDB (std::string &host, std::string &user, std::string &passwd)
{
  // DEBUGLOG_REPORT_FUNCTION;

    unsigned long flag = 0;

    dbglogfile <<"Connecting to MySQL database "  << _dbname.c_str()
               << " on " << host.c_str()
               << " as user " << user.c_str() << endl;

    mysql_init(&_mysql);
    _connection = mysql_real_connect(&_mysql, host.c_str(), user.c_str(),
        passwd.c_str(), _dbname.c_str(), 0, NULL, flag);

    // check for a connection error
    if(_connection == NULL) {
      dbglogfile << "MySQL error when connecting: \n" << mysql_error(&_mysql) << endl;
        return false;
    }

    dbglogfile << "Connected to MySQL database " << _dbname.c_str()
               <<  " on host " << host.c_str() << endl;

#if 0
    if(mysql_select_db(&_mysql, "mydb")){ 
        DBG_MSG(DBG_EROR, "%s: MySQL error when selecting database: %s\n",
            (mysql_error(&_mysql)));
        exit(1);
    }
#endif
    
    _state = Database::DBOPENED;
    
    return true;
}


bool
Database::closeDB (void)
{
    // DEBUGLOG_REPORT_FUNCTION;
    mysql_thread_end();
    mysql_close(&_mysql);
//    std::free(_connection);
    
    _state = Database::DBCLOSED;

    // FIXME: do something intelligent here
    return true;
}

// Return a vector of rows, each row being a vector of items.
vector<vector<string> > *
Database::queryResults(string &query)
{
    // DEBUGLOG_REPORT_FUNCTION;
    
    MYSQL_RES	*result;
    MYSQL_ROW	row;
    //    MYSQL_FIELD *fields;
    int         nrows;
    unsigned int i, res;
    
    dbglogfile << "Result Query is: " << query.c_str() << endl;

    res = mysql_real_query(&_mysql, query.c_str(), query.size());

    // the result should be zero if sucessful
    if (res) {
        switch (res) {
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
          default:
              dbglogfile << "MySQL error on query for:" << mysql_error(&_mysql) << endl;
          break;
        }
    }
    
    result = mysql_store_result(&_mysql);
    nrows = mysql_num_rows(result);
    // dbglogfile << nrows << " rows found for query." << endl;

    vector<vector<string> > *table = new vector<vector<string> >;
    while((row = mysql_fetch_row(result))) {
        // vector<string> *data = new vector<string>;
        vector<string> data;
        // MYSQL_FIELD *fields = mysql_fetch_fields(result);
        // dbglogfile << fields->name << endl;
        for (int i=0; i<mysql_num_fields(result); i++) {
	    // dbglogfile << row[i] << endl;
            data.push_back(row[i]);
        }
        table->push_back(data);
    }

    // We're done with the result
    mysql_free_result(result);

    return table;
}

bool
Database::queryInsert(const char *query)
{
    // DEBUGLOG_REPORT_FUNCTION;
    
    int retries, result;
    
    retries = 2;
    
    dbglogfile << "Query is: " << query << endl;
    
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

char *
Database::gettime()
{
    // DEBUGLOG_REPORT_FUNCTION;
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

void
Database::dump()
{
    // DEBUGLOG_REPORT_FUNCTION;
}

} // end of gnuae namespace

// local Variables:
// mode: C++
// indent-tabs-mode: t
// End:
