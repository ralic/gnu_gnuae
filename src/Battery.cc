// 
//   Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008,
//              2009 Free Software Foundation, Inc.
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

#include "Battery.h"
#include "log.h"
#include "gnuae.h"

using namespace std;

namespace gnuae {

static LogFile& dbglogfile = LogFile::getDefaultInstance();
static GnuAE& gdata = GnuAE::getDefaultInstance();

extern "C" {
#if 0
  
  battery_t batteries[] = {
    // Trojan Batteries
    { "T105",            "Trojan",         95,   6, 225,    0 },
    { "T125",            "Trojan",        109.,  6, 235,    0 },
    { "T145",            "Trojan",        145.,  6, 244,    0 },
    { "L16G",            "Trojan",        209.,  6, 350,    0 },
    { "L16P",            "Trojan",        234.,  6, 360,    0 },
    { "L16H",            "Trojan",        258.,  6, 395,    0 },
    // Real Good Gell Cells
    { "32AH Gell Cell",  "Real Goods",     69., 12,  32,    0 },
    { "51AH Gell Cell",  "Real Goods",    119., 12,  51,    0 },
    { "74AH Gell Cell",  "Real Goods",    149., 12,  74,    0 },
    { "86AH Gell Cell",  "Real Goods",    159., 12,  86,    0 },
    { "98AH Gell Cell",  "Real Goods",    189., 12,  98,    0 },
    { "183AH Gell Cell", "Real Goods",    339., 12, 183,    0 },
    { "225AH Gell Cell", "Real Goods",    399., 12, 225,    0 },
    { "180AH Gell Cell", "Real Goods",    189.,  6, 180,    0 },
    // 6 Volts, 3 Cell Polypropylene Container
    { "S-460",           "Surette",       173., 6,  350,  460 },
    { "S-530",           "Surette",       198., 6,  530,  530 },
    // 4 Volt, 2 cell batteries, Dual Container
    { "4CS-17S",         "Surette",       284., 4,  546,  770 },
    { "4CS-21S",         "Surette",       599., 4, 1104, 1557 },
    { "4CS-25S",         "Surette",       718., 4, 1350, 1900 },
    // 6 Volt, 3 cell batteries, Dual Container
    { "6HHG-31PS",       "Surette",       279., 6,  344,  430 },
    { "6EHG-31PS",       "Surette",       332., 6,  410,  512 },
    { "6CS-17PS",        "Surette",       478., 6,  546,  770 },
    { "6CS-21PS",        "Surette",       534., 6,  683,  963 },
    { "6CS-25PS",        "Surette",       642., 6,  820, 1156 },
    // 12 Volt, 6 cell batteries, , Dual Container
    { "8HHG-31PS",       "Surette",       372., 8,  344,  430 },
    { "8EHG-31PS",       "Surette",       444., 8,  410,  512 },
    { "8CS-17PS",        "Surette",       621., 8,  546,  770 },
    { "8CS-25PS",        "Surette",       933., 8,  820, 1156 },
    { 0, 0, 0., 0, 0,0 }
  };
  struct temp_comp battery_comp[] = {
    { 80, 26.7, 1.0  },
    { 70, 21.2, 1.04 },
    { 60, 15.6, 1.11 },
    { 50, 10.0, 1.19 },
    { 40,  4.4, 1.30 },
    { 30, -1.1, 1.40 },
    { 20, -6.7, 1.59 }
    {  0,  0.0,  0.0 }      
  };
#endif
};

Battery::Battery() {

};

Battery::~Battery() {

};

int
Battery::readCSV(std::string)
{
    DEBUGLOG_REPORT_FUNCTION;
}

int
Battery::readSQL(Database &db)
{
    // DEBUGLOG_REPORT_FUNCTION;
    if (db.getState() == Database::DBOPENED) {
	string query = "SELECT * from batteries";
	vector<vector<string> > *result = db.queryResults(query);
	vector<vector<string> >::iterator it;
	for (it=result->begin(); it!=result->end(); ++it) {
	    battery_t *thisbat = new battery_t;
	    vector<string> &row = *it;
	    thisbat->name = const_cast<char *>(row[1].c_str());
	    thisbat->manufacturer = const_cast<char *>(row[2].c_str());
	    thisbat->price = strtof(row[3].c_str(), NULL);
	    thisbat->voltage = strtol(row[4].c_str(), NULL, 0);
	    thisbat->rate20 = strtol(row[5].c_str(), NULL, 0);
	    thisbat->rate100 = strtol(row[6].c_str(), NULL, 0);
	    addEntry(thisbat);
	}
    }

    dbglogfile << "Loaded " << dataSize() << " records from batteries table." << endl;

    return dataSize();
}

void
Battery::dump()
{
    // DEBUGLOG_REPORT_FUNCTION;
    if (!dataSize()) {
	cerr << "No Battery data in memory." << endl;
    } else {
	vector<string>::iterator it;
	vector<string> *loadnames = dataNames();
	for (it = loadnames->begin(); it != loadnames->end(); ++it) {
	    dump(findEntry(*it));
	}
    }
}

void
Battery::dump(battery_t *bat)
{
    // DEBUGLOG_REPORT_FUNCTION;
    if (bat) {
	dbglogfile << "Battery Name is: " << bat->name;
	dbglogfile << ", Manufacturer is: " << bat->manufacturer << endl;
	// dbglogfile << bat->price << endl;
	dbglogfile << "\tVoltage is: "<< bat->voltage;
	dbglogfile << ", Rate20 is: " << bat->rate20;
	dbglogfile << ", Rate100 is: " << bat->rate100 << endl;
    } else {
	dbglogfile << "No Battery data in memory." << endl;
    }
}

} // end of gnuae namespace

// local Variables:
// mode: C++
// indent-tabs-mode: t
// End:
