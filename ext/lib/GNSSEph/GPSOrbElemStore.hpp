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
 * @file GPSOrbElemStore.hpp
 * Store GPS broadcast OrbElem information (i.e. like the data in
 * subframes 1-3) and computes satellite Xvt based upon this data and the
 * algorithms defined for that data in the IS-GPS-200.
 */

#ifndef GNSSTK_GPSORBELEMSTORE_HPP
#define GNSSTK_GPSORBELEMSTORE_HPP

#include <iostream>
#include <list>
#include <map>

#include "OrbElemBase.hpp"
#include "OrbElemStore.hpp"

namespace gnsstk
{
   /// @ingroup GNSSEph 
   //@{

   /// Class for storing and accessing GPS SV's position, 
   /// velocity, and clock data. Also defines a simple interface to remove
   /// data that has been added.
   class GPSOrbElemStore : public OrbElemStore
   {
   public:
      
      GPSOrbElemStore()
         throw()
      {
         initialTime.setTimeSystem(TimeSystem::Any);
         finalTime.setTimeSystem(TimeSystem::Any);
         addSatSys(SatelliteSystem::GPS);
         setTimeSystem(TimeSystem::GPS);
      }

      virtual ~GPSOrbElemStore()
      { clear();}

      void dump(std::ostream& s=std::cout, short detail=0) const
         throw();

   }; // end class

   //@}

} // namespace

#endif
