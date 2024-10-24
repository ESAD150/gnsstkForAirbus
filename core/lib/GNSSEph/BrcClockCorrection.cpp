//==============================================================================
//
//  This file is part of GNSSTk, the ARL:UT GNSS Toolkit.
//
//  The GNSSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GNSSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GNSSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2022, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public
//                            release, distribution is unlimited.
//
//==============================================================================

/**
 * @file BrcClockCorrection.cpp
 * Ephemeris data encapsulated in engineering terms
 */

#include "StringUtils.hpp"
#include "BrcClockCorrection.hpp"
#include "GPS_URA.hpp"
#include <cmath>

namespace gnsstk
{
   using namespace std;
   using namespace gnsstk;

   BrcClockCorrection::BrcClockCorrection()
      noexcept
   {
      dataLoaded = false;

      PRNID = 0;

      satSys = "";

      healthy = false;

      URAoc = -16;
      URAoc1 = URAoc2 = 0;
      af0 = af1 = af2 = 0.0;
   }

   BrcClockCorrection::BrcClockCorrection(const std::string satSysArg,
                                          const ObsID obsIDArg,
                                          const short PRNIDArg,
                                          const CommonTime TocArg,
                                          const CommonTime TopArg,
                                          const short URAocArg,
                                          const short URAoc1Arg,
                                          const short URAoc2Arg,
                                          const bool healthyArg,
                                          const double af0Arg,
                                          const double af1Arg,
                                          const double af2Arg )
   {
      loadData(satSysArg, obsIDArg, PRNIDArg, TocArg, TopArg, URAocArg,
               URAoc1Arg, URAoc2Arg, healthyArg, af0Arg, af1Arg, af2Arg );
   }

      /// Legacy GPS Subframe 1-3
   BrcClockCorrection::BrcClockCorrection(const ObsID obsIDArg,
                                          const short PRNID,
                                          const short fullweeknum,
                                          const long subframe1[10] )
   {
      loadData(obsIDArg, PRNID,fullweeknum,subframe1 );
   }


   bool BrcClockCorrection::operator==(const BrcClockCorrection& right) const
      noexcept
   {
         // EngNav has no data
      return ((dataLoaded == right.dataLoaded) &&
              (satSys == right.satSys) &&
              (obsID == right.obsID) &&
              (PRNID == right.PRNID) &&
              (Toc == right.Toc) &&
              (Top == right.Top) &&
              (URAoc == right.URAoc) &&
              (URAoc1 == right.URAoc1) &&
              (URAoc2 == right.URAoc2) &&
              (healthy == right.healthy) &&
              (af0 == right.af0) &&
              (af1 == right.af1) &&
              (af2 == right.af2));
   }


   void BrcClockCorrection::loadData(const std::string satSysArg,
                                     const ObsID obsIDArg,
                                     const short PRNIDArg,
                                     const CommonTime TocArg,
                                     const short URAocArg,
                                     const bool healthyArg,
                                     const double af0Arg,
                                     const double af1Arg,
                                     const double af2Arg )
   {
      const CommonTime TopArg;
      const short URAoc1Arg = 0;
      const short URAoc2Arg = 0;

      loadData(satSysArg, obsIDArg, PRNIDArg, TocArg, TopArg, URAocArg,
               URAoc1Arg, URAoc2Arg, healthyArg, af0Arg, af1Arg, af2Arg );
   }

   void BrcClockCorrection::loadData(const std::string satSysArg,
                                     const ObsID obsIDArg,
                                     const short PRNIDArg,
                                     const CommonTime TocArg,
                                     const CommonTime TopArg,
                                     const short URAocArg,
                                     const short URAoc1Arg,
                                     const short URAoc2Arg,
                                     const bool healthyArg,
                                     const double af0Arg,
                                     const double af1Arg,
                                     const double af2Arg )
   {
      satSys      = satSysArg;
      obsID       = obsIDArg;
      PRNID       = PRNIDArg;
      Toc         = TocArg;
      Top         = TopArg;
      URAoc       = URAocArg;
      URAoc1      = URAoc1Arg;
      URAoc2      = URAoc2Arg;
      healthy     = healthyArg;
      af0         = af0Arg;
      af1         = af1Arg;
      af2         = af2Arg;
      dataLoaded  = true;
   }

   void BrcClockCorrection::loadData(const ObsID obsIDArg, const short PRNIDArg,
                                     const short fullweeknum,
                                     const long subframe1[10] )
   {
      double ficked[60];

         //Load overhead members
      satSys = "G";
      obsID = obsIDArg;
      PRNID = PRNIDArg;

         //Convert Subframe 1
      if (!subframeConvert(subframe1, fullweeknum, ficked))
      {
         InvalidParameter exc("Subframe 1 not valid.");
         GNSSTK_THROW(exc);
      }
      double Txmit  = ficked[2];     // Time of week from handover word
      short weeknum = static_cast<short>( ficked[5] );
      short accFlag = static_cast<short>( ficked[7] );
      short health  = static_cast<short>( ficked[8] );
      double TocSOW = ficked[12];
      af2           = ficked[13];
      af1           = ficked[14];
      af0           = ficked[15];

      double diff = Txmit - TocSOW;
         // NOTE: This USED to be in DayTime, but DayTime is going
         // away.  Where is it now?
      if (diff > HALFWEEK)
      {
            // Convert week # of transmission to week # of epoch time
            // when Toc is forward across a week boundary
         weeknum++;
      }
      else if (diff < -HALFWEEK)
      {
            // Convert week # of transmission to week # of epoch time
            // when Toc is back across a week boundary
         weeknum--;
      }

      Toc = GPSWeekSecond( weeknum, TocSOW, TimeSystem::GPS );
      URAoc = accFlag;              //Store L1 C/A URA as URAoc
      URAoc1 = 0;
      URAoc2 = 0;
      healthy = false;
      if (health == 0)
         healthy = true;
      dataLoaded = true;
      return;
   }

   bool BrcClockCorrection::hasData() const
   {
      return(dataLoaded);
   }

   CommonTime BrcClockCorrection::getEpochTime() const
   {
      return Toc;
   }

   double BrcClockCorrection::svClockBias(const CommonTime& t) const
   {
      double dtc,elaptc;
      elaptc = t - getEpochTime();
      dtc = af0 + elaptc * ( af1 + elaptc * af2 );
      return dtc;
   }

   double BrcClockCorrection::svClockBiasM(const CommonTime& t) const
   {
      double ret = svClockBias(t);
      ret = ret*C_MPS;
      return (ret);
   }

   double BrcClockCorrection::svClockDrift(const CommonTime& t) const
   {
      double drift,elaptc;
      elaptc = t - getEpochTime();
      drift = af1 + elaptc * af2;
      return drift;
   }

   short BrcClockCorrection::getPRNID() const
   {
      if(!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GNSSTK_THROW(exc);
      }
      return PRNID;
   }

   ObsID BrcClockCorrection::getObsID() const
   {
      if(!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GNSSTK_THROW(exc);
      }
      return obsID;
   }

   short BrcClockCorrection::getFullWeek()  const
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GNSSTK_THROW(exc);
      }
      GPSWeekSecond gpsws(Toc);
      return (gpsws.week);
   }

   double BrcClockCorrection::getAccuracy(const CommonTime& t)  const
   {
      double accuracy;

      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GNSSTK_THROW(exc);
      }
         //if (obsID.code == "tcCA" ) // L1 C/A
      accuracy = ura2accuracy(URAoc);
         //else

         // deleted because algorithms in -705 and -800 changed
         //     accuracy = uraoc2CNAVaccuracy(URAoc, URAoc1, URAoc2, t, Top);
      return accuracy;
   }

   short BrcClockCorrection::getURAoc(const short& ndx) const
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
      }
      if (ndx == 0) return URAoc;
      else if (ndx == 1) return URAoc1;
      else if (ndx == 2) return URAoc2;
      InvalidParameter exc ("Required data not stored.");
      GNSSTK_THROW(exc);
   }

   double BrcClockCorrection::getToc() const
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GNSSTK_THROW(exc);
      }
      GPSWeekSecond gpsws(Toc);
      return gpsws.sow;
   }

   double BrcClockCorrection::getAf0() const
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GNSSTK_THROW(exc);
      }
      return af0;
   }

   double BrcClockCorrection::getAf1() const
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GNSSTK_THROW(exc);
      }
      return af1;
   }

   double BrcClockCorrection::getAf2() const
   {
      if (!dataLoaded)
      {
         InvalidRequest exc("Required data not stored.");
         GNSSTK_THROW(exc);
      }
      return af2;
   }

   static void timeDisplay( ostream & os, const CommonTime& t )
   {
         // Convert to CommonTime struct from GPS wk,SOW to M/D/Y, H:M:S.
      GPSWeekSecond dummyTime;
      dummyTime = GPSWeekSecond(t);
      os << setw(4) << dummyTime.week << "(";
      os << setw(4) << (dummyTime.week & 0x03FF) << ")  ";
      os << setw(6) << setfill(' ') << dummyTime.sow << "   ";

      switch (dummyTime.getDayOfWeek())
      {
         case 0: os << "Sun-0"; break;
         case 1: os << "Mon-1"; break;
         case 2: os << "Tue-2"; break;
         case 3: os << "Wed-3"; break;
         case 4: os << "Thu-4"; break;
         case 5: os << "Fri-5"; break;
         case 6: os << "Sat-6"; break;
         default: break;
      }
      os << "   " << (static_cast<YDSTime>(t)).printf("%3j   %5.0s  ")
         << (static_cast<CivilTime>(t)).printf("%02m/%02d/%04Y   %02H:%02M:%02S");
   }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
   static void shortcut(ostream & os, const long HOW )
   {
      short DOW, hour, min, sec;
      long SOD, SOW;
      short SOH;

      SOW = static_cast<long>( HOW );
      DOW = static_cast<short>( SOW / SEC_PER_DAY );
      SOD = SOW - static_cast<long>( DOW * SEC_PER_DAY );
      hour = static_cast<short>( SOD/3600 );

      SOH = static_cast<short>( SOD - (hour*3600) );
      min = SOH/60;

      sec = SOH - min * 60;
      switch (DOW)
      {
         case 0: os << "Sun-0"; break;
         case 1: os << "Mon-1"; break;
         case 2: os << "Tue-2"; break;
         case 3: os << "Wed-3"; break;
         case 4: os << "Thu-4"; break;
         case 5: os << "Fri-5"; break;
         case 6: os << "Sat-6"; break;
         default: break;
      }

      os << ":" << setfill('0')
         << setw(2) << hour
         << ":" << setw(2) << min
         << ":" << setw(2) << sec
         << setfill(' ');
   }
#pragma clang diagnostic pop
   void BrcClockCorrection::dump(ostream& s) const
   {
      const ios::fmtflags oldFlags = s.flags();
      s.setf(ios::fixed, ios::floatfield);
      s.setf(ios::right, ios::adjustfield);
      s.setf(ios::uppercase);
      s.precision(0);
      s.fill(' ');

      s << "****************************************************************"
        << "************" << endl
        << "Broadcast Ephemeris (Engineering Units)" << endl
        << endl
        << "PRN : " << setw(2) << PRNID << endl
        << endl;

      s << "              Week(10bt)     SOW     DOW   UTD     SOD"
        << "  MM/DD/YYYY   HH:MM:SS\n";
      s << "Clock Epoch:  ";

      timeDisplay(s, getEpochTime());
      s << endl;

      s.setf(ios::scientific, ios::floatfield);
      s.precision(11);

      s << endl
        << "           CLOCK"
        << endl
        << endl
        << "Bias T0:     " << setw(18) << af0 << " sec" << endl
        << "Drift:       " << setw(18) << af1 << " sec/sec" << endl
        << "Drift rate:  " << setw(18) << af2 << " sec/(sec**2)" << endl;

      s << "****************************************************************"
        << "************" << endl;

      s.flags(oldFlags);
   }

   ostream& operator<<(ostream& s, const BrcClockCorrection& eph)
   {
      eph.dump(s);
      return s;
   } // end of operator<<

} // namespace
