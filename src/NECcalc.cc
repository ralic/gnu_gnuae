// 
//   Copyright (C) 2004 Free Software Foundation, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

// Gnueaec - Do the various calculations as a command line utility.
//           These are usually done as part of the other programs, but
//           this gives us a quick and accurate way to make individual
//           calculations when we need them, as well as to test the
//           code that does the calculations.

// This is generated by autoconf
#include "config.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <getopt.h>
#include <iostream>
#include <sstream>

#include "NEC.h"

using namespace std;
using namespace gnuae;

static void usage (const char *);

// This is the global handle for the data logging system.

int verbosity = 0;

main(int argc, char **argv) {
  int c, status, result;
  bool logopen = false;
  const char *errmsg;
  char buf[30];
  bool tfactor  = false;
  bool dump     = false;
  bool wattage  = false;
  bool voltage  = false;
  bool amperage = false;
  bool awg      = false;
  bool voltdrop = false;
  bool voltloss = false;
  bool exact    = false;
  
  NEC nec;
  float percentdrop = 0.0;
  float watts    = 0.0;
  int volts      = 0;
  float amps     = 0.0;
  int temp       = 0;
  float loss     = 0.0;
  float drop     = 0.0;
  int   distance = 0;
  int   gauge    = 0;
  istringstream  iss;
  string optstr;

  while ((c = getopt (argc, argv, "DhVcel:d:t:v:l:m:w:a:g:")) != -1) {
    switch (c) {
    case 'L':
      logopen = true;
      //      dbglogfile.Open (optarg);
      break;

      // Calculate the resistance rather than looking it up in NEC
      // Table 8
    case 'e':
      exact = true;
      nec.toggleExact(true);
      break;
      
    case 'c':
      tfactor = true;
      break;
      
    case 'D':
      dump = true;
      break;
      
    case 'd':
      voltdrop = true;
      optstr = optarg;
      optstr.replace(optstr.find(",", 0), 1, " ");
      optstr.replace(optstr.find(",", 0), 1, " ");
      optstr.replace(optstr.find(",", 0), 1, " ");
      iss.str(optstr);
      iss >> distance;
      iss >> gauge;
      iss >> amps;
      iss >> volts;
      if (verbosity)
        cerr << "Distance is " << distance <<
          " AWG gauge is " << gauge <<
          ", Amps is " << amps <<
          ", Voltage is " << volts << endl;
      break;
      
    case 'l':
      voltloss = true;
      optstr = optarg;
      optstr.replace(optstr.find(",", 0), 1, " ");
      optstr.replace(optstr.find(",", 0), 1, " ");
      iss.str(optstr);
      iss >> distance;
      iss >> gauge;
      iss >> amps;
      if (verbosity)
        cerr << "Distance is " << distance <<
          " AWG gauge is " << gauge <<
          " Amps is " << amps << endl;
      break;
      
    case 't':
      iss.str(optarg);
      //      iss >> temp;
      // FIXME: why does istringstream not get this one vaue correctly ?
      temp = atoi(optarg);
      if (verbosity)
        cerr << "Temperature is " << temp << endl;
      break;
      
    case 'w':
      wattage = true;
      optstr = optarg;
      optstr.replace(optstr.find(",", 0), 1, " ");
      iss.str(optstr);
      iss >> volts;
      iss >> amps;
      if (verbosity)
        cerr << "Volts is " << volts << " Amps is " << amps << endl;
      break;
      
    case 'v':
      voltage = true;
      optstr = optarg;
      optstr.replace(optstr.find(",", 0), 1, " ");
      iss.str(optstr);
      iss >> watts;
      iss >> amps;
      if (verbosity)
        cerr << "Watts is " << watts << " Amps is " << amps << endl;
      break;
      
    case 'a':
      amperage = true;
      optstr = optarg;
      optstr.replace(optstr.find(",", 0), 1, " ");
      iss.str(optstr);
      iss >> watts;
      iss >> volts;
      if (verbosity)
        cerr << "Watts is " << watts << " Volts is " << volts << endl;
      break;

      // find the AWG wire gauge for this distance, voltage, and amperage
    case 'g':
      awg = true;
      optstr = optarg;
      optstr.replace(optstr.find(",", 0), 1, " ");
      optstr.replace(optstr.find(",", 0), 1, " ");
      iss.str(optstr);
      iss >> distance;
      iss >> volts;
      iss >> amps;
      if (verbosity)
        cerr << "Distance is " << distance <<
          " Volts is " << volts <<
          " Amps is " << amps << endl;
      break;
      
    case 'h':
      usage (argv[0]);
      break;
      
    case 'V':
      nec.toggleDebug(true);
      //      dbglog.set_verbosity();
      verbosity = 1;
      cout << "Verbose output turned on" << endl;
      break;
      break;
      
    default:
      usage (argv[0]);
      break;
    }
  }
  
  if (!logopen)
    //    dbglogfile.Open (DEFAULT_LOGFILE);

  if (wattage) {
    cout << "Wattage is " << nec.watts(volts, amps) << endl;
  }

  if (voltage) {
    cout << "Voltage is " << nec.volts(watts, amps) << endl;
  }
  
  if (amperage) {
    cout << "Amperage is " << nec.amps(watts, volts) << endl;
  }

  // If the temperature isn't specified, set to the nominal temp as
  // used by NEC Table 8
  if (temp == 0) {
    temp = NOMINALTEMP;
  }
  
  // Calculate the AWG wire gauge we need for this circuit
  if (awg) {
    gauge = nec.findGauge(distance, volts, amps, temp);
    if (gauge > 0) {
      cout << "The AWG wire gauge needs to be: #" << gauge << endl;
    }
    else {
      cout << "The AWG wire gauge needs to be: #" << abs(gauge) << "/0" << endl;
    }
  }

  // Calculate the voltage loss using NEC Table 8
  if (voltloss) {
    loss = nec.voltLoss(distance, gauge, amps, temp);
    cout << "The voltage loss is: " << loss << endl;
  }
  
  // Calculate the voltage drop using NEC Table 8
  if (voltdrop) {
    loss = nec.voltLoss(distance, gauge, amps, temp);
    drop = nec.voltDrop(awg, volts, amps, loss);
    percentdrop = (loss/volts) * 100;
    
    cout << "The voltage drop for " << volts << " volts is: "
         << cout.precision(2) << percentdrop << "%" << endl;
    
    if (percentdrop < 3.0) {
      cerr << "Voltage drop acceptable, under 3%" << endl;
    }
    else {
      cerr << "Voltage drop too high, over 3%" << endl;
    }
    
    if (percentdrop < 5.0 && percentdrop > 3.0) {
      cerr << "Voltage drop marginally acceptable, under 5%" << endl;
    }

    if (percentdrop > 5.0)
      cerr << "Voltage drop too high, over 5%" << endl;
  }
  
  // Get the temperature compensation factor
  if (tfactor){
    cout << "The temperaure compensation factor for crystaline panels is: "
         << nec.crystalCompensation(temp) << endl;
  }
  
  if (dump) {
    nec.dump();
  }
}

static void
usage (const char *prog)
{
  fprintf (stderr, "This program does NEC 2002 code compliancy calculations....\n"
           );
  fprintf (stderr, "Usage: %s [hDLVeawvgt]\n", prog);
  fprintf (stderr, "-h\t\tHelp\n");
  fprintf (stderr, "-c\tCrystaline Temp Factor\n");
  fprintf (stderr, "-e\tCalculate Exact Resistance\n");
  fprintf (stderr, "-w volts,amps\tCalculate wattage\n");
  fprintf (stderr, "-v watts,amps\tCalculate voltage\n");
  fprintf (stderr, "-t temperature\tSpecify the temperature in Farenheit\n");
  fprintf (stderr, "-a watts,volts\tCalculate amperage\n");
  fprintf (stderr, "-g distance,watts,volts\tFind the AWG wire gauge\n");
  fprintf (stderr, "-d distance,awg,amps,volts\tFind the voltage drop\n");
  fprintf (stderr, "-l distance,awg,amps\tFind the voltage loss\n");
  fprintf (stderr, "-L\t\tSpecify logfile name\n");
  fprintf (stderr, "-V\t\tVerbose mode\n");
  fprintf (stderr, "-D\t\tPrint NEC Tables\n");
  exit (-1);
}
