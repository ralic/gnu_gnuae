// 
//   Copyright (C) 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
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
#include "config.h"

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>

#include "Combiners.h"
#include "log.h"
#include "gnuae.h"
#include "DataType.h"
#include "Database.h"

using namespace std;

namespace gnuae {

static LogFile& dbglogfile = LogFile::getDefaultInstance();
static GnuAE& gdata = GnuAE::getDefaultInstance();

#if 0
extern "C" {
  
  combiner_t combiners[] = {
    { "None",               "None",         0.0, 0, 0 },
    { "PSPV",               "Outback",    139.0, 12, 2 },
    { "TCB10",              "Trace",      229.0, 10, 2 },
    { "TCB6",               "Trace",      189.0,  6, 1 },
    { "Custom",             "Yourself",   250.0, 12, 2 },
    { "PCB10",              "Pulse",      199.0, 10, 2 },
    { 0, 0, 0.0, 0, 0 }
  };
};
#endif

Combiners::Combiners() {

}

Combiners::~Combiners() {

}

int
Combiners::readCSV(std::string)
{
    DEBUGLOG_REPORT_FUNCTION;
}

int
Combiners::readSQL(Database &db)
{
    DEBUGLOG_REPORT_FUNCTION;
    if (db.getState() == Database::DBOPENED) {
    	string query = "SELECT * from combiners";
    	vector<vector<string> > *result = db.queryResults(query);
    	vector<vector<string> >::iterator it;
    	for (it=result->begin(); it!=result->end(); ++it) {
    	    combiner_t *thiscm = new combiner_t;
    	    vector<string> &row = *it;
    	    thiscm->name = const_cast<char *>(row[1].c_str());
    	    thiscm->manufacturer = const_cast<char *>(row[2].c_str());
    	    // thiscent->price = strtof(row[3].c_str(), NULL);
    	    thiscm->circuits = strtol(row[4].c_str(), NULL, 0);
    	    thiscm->conductors = strtol(row[5].c_str(), NULL, 0);
    	    addEntry(thiscm);
    	}
    }

    dbglogfile << "Loaded " << dataSize() << " records from centers table." << endl;

    return dataSize();
}

void
Combiners::dump()
{
    // DEBUGLOG_REPORT_FUNCTION;
    if (!dataSize()) {
    	cerr << "No PV Combiner data in memory." << endl;
    } else {
    	vector<string>::iterator it;
    	vector<string> *loadnames = dataNames();
    	for (it = loadnames->begin(); it != loadnames->end(); ++it) {
    	    dump(findEntry(*it));
    	}
    }
}

void
Combiners::dump(combiner_t *comp)
{
    // DEBUGLOG_REPORT_FUNCTION;
    const char *feature_str[] = {
    	"NONE",
    	"YES",
    	"NO",
    	"OPTIONAL",
    	"DEFAULT"
    };

    cerr << "PV combiner name is: " << comp->name;
    cerr << ", Manufacturer is: " << comp->manufacturer;
    // cerr << comp->price;
    cerr << ", Circuits are: " << comp->circuits;
    cerr << ", Conductors are: " << comp->conductors << endl;
}

} // end of gnuae namespace

// local Variables:
// mode: C++
// indent-tabs-mode: t
// End:
