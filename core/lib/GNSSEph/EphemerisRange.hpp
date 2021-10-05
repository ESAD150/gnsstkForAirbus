//==============================================================================
//
//  This file is part of GNSSTk, the GNSS Toolkit.
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
//  Copyright 2004-2021, The Board of Regents of The University of Texas System
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
 * @file EphemerisRange.hpp
 * Computation of range and associated quantities from XvtStore
 */
 
#ifndef NEW_EPHEMERIS_RANGE_HPP
#define NEW_EPHEMERIS_RANGE_HPP

#include "CommonTime.hpp"
#include "SatID.hpp"
#include "Position.hpp"
#include "NavLibrary.hpp"
#include "ValidType.hpp"

namespace gnsstk
{
      /// @ingroup GNSSEph
      //@{

      /** Compute the corrected range from receiver at position Rx, to
       * the GPS satellite given by SatID sat, as well as azimuth,
       * elevation, etc., given a nominal timetag (either received or
       * transmitted time) and an XvtStore.
       */
   class CorrectedEphemerisRange
   {
   public:
         /// Default constructor.
      CorrectedEphemerisRange() {}

         /// Compute the corrected range at RECEIVE time, from
         /// receiver at position Rx, to the GPS satellite given by
         /// SatID sat, as well as all the CER quantities, given the
         /// nominal receive time tr_nom and an XvtStore.
      double ComputeAtReceiveTime(
         const CommonTime& tr_nom,
         const Position& Rx,
         const SatID sat,
         NavLibrary& eph);

         /// Compute the corrected range at TRANSMIT time, from
         /// receiver at position Rx, to the GPS satellite given by
         /// SatID sat, as well as all the CER quantities, given the
         /// nominal receive time tr_nom, the measured pseudorange,
         /// and an XvtStore.
      double ComputeAtTransmitTime(
         const CommonTime& tr_nom,
         const double& pr,
         const Position& Rx,
         const SatID sat,
         NavLibrary& eph);

         /// Compute the corrected range at TRANSMIT time, from
         /// receiver at position Rx, to the GPS satellite given by
         /// SatID sat, as well as all the CER quantities, given the
         /// nominal receive time tr_nom and an XvtStore.
         /// This doesn't use a pseudorange to initialize the
         /// time-of-flight computation; however note that this could
         /// be problematic since the measured pseudorange includes
         /// the Rx clock bias while this does not; prefer the version
         /// with measured pseudorange input.
      double ComputeAtTransmitTime(
         const CommonTime& tr_nom,
         const Position& Rx,
         const SatID sat,
         NavLibrary& eph);

         /// Compute the corrected range at TRANSMIT time, from
         /// receiver at position Rx, to the GPS satellite given by
         /// SatID sat, as well as all the CER quantities, given the
         /// nominal transmit time tt_nom and an XvtStore. This is
         /// used for data smoothed to transmit time.
      double ComputeAtTransmitSvTime(
         const CommonTime& tt_nom,
         const double& pr,
         const Position& Rx,
         const SatID sat,
         NavLibrary& eph);

         /// The computed raw (geometric) range in meters.
      double rawrange;
         /// The satellite clock bias in meters.
      double svclkbias;
         /// The satellite clock drift in m/s.
      double svclkdrift;
         /// The relativity correction in meters.
      double relativity;
         /// The satellite elevation (spheroidal), as seen at the
         /// receiver, in degrees.
      double elevation;
         /// The satellite azimuth (spheroidal), as seen at the
         /// receiver, in degrees.
      double azimuth;
         /// The satellite elevation (geodetic), as seen at the
         /// receiver, in degrees.
      double elevationGeodetic;
         /// The satellite azimuth (geodetic), as seen at the
         /// receiver, in degrees.
      double azimuthGeodetic;
         /// The computed transmit time of the signal.
      CommonTime transmit;
         /// The direction cosines of the satellite, as seen at the
         /// receiver (XYZ).
      Triple cosines;
         /// The satellite position (m) and velocity (m/s) in ECEF coordinates.
      Xvt svPosVel;
         /// The IODC of the GPS LNAV ephemeris, invalid for other GNSSes
      vshort iodc;
         /** The health bits from the GPS LNAV ephemeris, invalid for
          * other GNSSes */
      vshort health;

         /** This is a kludge to allow people to do compute ORDs using
          * "Nearest" instead of "User" (User is the default). */
      static NavSearchOrder order;

   private:
         // These are just helper functions to keep from repeating code
      void updateCER(const Position& Rx);
      void rotateEarth(const Position& Rx);
      bool getXvt(NavLibrary& navLib, const NavSatelliteID& sat,
                  const CommonTime& when);

   }; // end class CorrectedEphemerisRange

      /// Compute relativity correction (sec.s) from the satellite
      /// position and velocity
   double RelativityCorrection(const Xvt& svPosVel);

      //@}

}  // namespace gnsstk

#endif
