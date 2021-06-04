//==============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
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
#include "MultiFormatNavDataFactory.hpp"
#include "BasicTimeSystemConverter.hpp"

namespace gpstk
{
   MultiFormatNavDataFactory ::
   MultiFormatNavDataFactory()
   {
         // keys for factories are not unique but that doesn't really matter.
      for (const auto& i : factories())
      {
         supportedSignals.insert(i.first);
      }
   }


   MultiFormatNavDataFactory ::
   ~MultiFormatNavDataFactory()
   {
      clear();
   }


   bool MultiFormatNavDataFactory ::
   find(const NavMessageID& nmid, const CommonTime& when,
        NavDataPtr& navData, SVHealth xmitHealth, NavValidityType valid,
        NavSearchOrder order)
   {
         // search factories until we find what we want.
      auto range = factories().equal_range(nmid);
      for (auto fi = range.first; fi != range.second; ++fi)
      {
         if (fi->second->find(nmid, when, navData, xmitHealth, valid, order))
            return true;
      }
      return false;
   }


   bool MultiFormatNavDataFactory ::
   getOffset(TimeSystem fromSys, TimeSystem toSys,
             const CommonTime& when, NavDataPtr& offset,
             SVHealth xmitHealth, NavValidityType valid)
   {
         // Search through factories until we get a match or run out
         // of factories.  Use unique pointers to avoid double-searching.
      std::set<NavDataFactory*> uniques;
      for (auto fi = factories().begin(); fi != factories().end(); ++fi)
      {
         NavDataFactory *ndfp = dynamic_cast<NavDataFactory*>(fi->second.get());
         if (uniques.count(ndfp))
            continue; // already processed
         uniques.insert(ndfp);
         if (fi->second->getOffset(fromSys, toSys, when, offset, xmitHealth,
                                   valid))
         {
            return true;
         }
      }
      return false;
   }


   void MultiFormatNavDataFactory ::
   edit(const CommonTime& fromTime, const CommonTime& toTime)
   {
         // use a set to make sure we only process a factory once
      std::set<NavDataFactory*> edited;
      for (auto& i : factories())
      {
         NavDataFactory *ndfp = i.second.get();
         if (edited.count(ndfp))
            continue; // already processed
         edited.insert(ndfp);
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            ndfs->edit(fromTime,toTime);
         }
      }
   }


   void MultiFormatNavDataFactory ::
   edit(const CommonTime& fromTime, const CommonTime& toTime,
        const NavSatelliteID& satID)
   {
         // use a set to make sure we only process a factory once
      std::set<NavDataFactory*> edited;
      for (auto& i : factories())
      {
         NavDataFactory *ndfp = i.second.get();
         if (edited.count(ndfp))
            continue; // already processed
         edited.insert(ndfp);
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            ndfs->edit(fromTime,toTime,satID);
         }
      }
   }


   void MultiFormatNavDataFactory ::
   edit(const CommonTime& fromTime, const CommonTime& toTime,
        const NavSignalID& signal)
   {
         // use a set to make sure we only process a factory once
      std::set<NavDataFactory*> edited;
      for (auto& i : factories())
      {
         NavDataFactory *ndfp = i.second.get();
         if (edited.count(ndfp))
            continue; // already processed
         edited.insert(ndfp);
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            ndfs->edit(fromTime,toTime,signal);
         }
      }
   }


   void MultiFormatNavDataFactory ::
   clear()
   {
         // use a set to make sure we only process a factory once
      std::set<NavDataFactory*> edited;
      for (auto& i : factories())
      {
         NavDataFactory *ndfp = i.second.get();
         if (edited.count(ndfp))
            continue; // already processed
         edited.insert(ndfp);
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            ndfs->clear();
         }
      }
   }


   CommonTime MultiFormatNavDataFactory ::
   getInitialTime() const
   {
         // always use basic time system converter because
         // 1) We don't need nanosecond precision for getInitialTime
         // 2) We don't want to spend a lot of time searching nav data
      BasicTimeSystemConverter btsc;
      CommonTime rv = CommonTime::END_OF_TIME;
      rv.setTimeSystem(TimeSystem::Any);
         // use a set to make sure we only process a factory once
      std::set<NavDataFactory*> used;
      for (auto& i : factories())
      {
         NavDataFactory *ndfp = i.second.get();
         if (used.count(ndfp))
            continue; // already processed
         used.insert(ndfp);
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            CommonTime t = ndfs->getInitialTime();
            if ((rv.getTimeSystem() == TimeSystem::Any) ||
                (t.getTimeSystem() == TimeSystem::Any) ||
                (t.getTimeSystem() == rv.getTimeSystem()))
            {
               rv = std::min(rv,t);
            }
            else
            {
               t.changeTimeSystem(TimeSystem::UTC, &btsc);
               rv.changeTimeSystem(TimeSystem::UTC, &btsc);
               rv = std::min(rv,t);
            }
         }
      }
      return rv;
   }


   CommonTime MultiFormatNavDataFactory ::
   getFinalTime() const
   {
         // always use basic time system converter because
         // 1) We don't need nanosecond precision for getFinalTime
         // 2) We don't want to spend a lot of time searching nav data
      BasicTimeSystemConverter btsc;
      CommonTime rv = CommonTime::BEGINNING_OF_TIME;
      rv.setTimeSystem(TimeSystem::Any);
         // use a set to make sure we only process a factory once
      std::set<NavDataFactory*> used;
      for (auto& i : factories())
      {
         NavDataFactory *ndfp = i.second.get();
         if (used.count(ndfp))
            continue; // already processed
         used.insert(ndfp);
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            CommonTime t = ndfs->getFinalTime();
            if ((rv.getTimeSystem() == TimeSystem::Any) ||
                (t.getTimeSystem() == TimeSystem::Any) ||
                (t.getTimeSystem() == rv.getTimeSystem()))
            {
               rv = std::max(rv,t);
            }
            else
            {
               t.changeTimeSystem(TimeSystem::UTC, &btsc);
               rv.changeTimeSystem(TimeSystem::UTC, &btsc);
               rv = std::max(rv,t);
            }
         }
      }
      return rv;
   }


   size_t MultiFormatNavDataFactory ::
   size() const
   {
         // this one is easy, it's just the sum of each individual
         // factory's size
         // use a set to make sure we only process a factory once
      std::set<NavDataFactory*> uniqueFact;
      size_t rv = 0;
      for (auto& i : factories())
      {
         NavDataFactory *ndfp = i.second.get();
         if (uniqueFact.count(ndfp))
            continue; // already processed
         uniqueFact.insert(ndfp);
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            rv += ndfs->size();
         }
      }
      return rv;
   }


   size_t MultiFormatNavDataFactory ::
   numSignals() const
   {
         // use a set to make sure we only process a factory once
      std::set<NavDataFactory*> uniqueFact;
      std::set<NavSignalID> uniqueSig;
      for (auto& i : factories())
      {
         NavDataFactory *ndfp = i.second.get();
         if (uniqueFact.count(ndfp))
            continue; // already processed
         uniqueFact.insert(ndfp);
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            for (const auto& mti : ndfs->data)
            {
               for (const auto& sigIt : mti.second)
               {
                  uniqueSig.insert(sigIt.first);
               }
            }
         }
      }
      return uniqueSig.size();
   }


   size_t MultiFormatNavDataFactory ::
   numSatellites() const
   {
         // use a set to make sure we only process a factory once
      std::set<NavDataFactory*> uniqueFact;
      std::set<NavSatelliteID> uniqueSat;
      for (auto& i : factories())
      {
         NavDataFactory *ndfp = i.second.get();
         if (uniqueFact.count(ndfp))
            continue; // already processed
         uniqueFact.insert(ndfp);
         NavDataFactoryWithStore *ndfs =
            dynamic_cast<NavDataFactoryWithStore*>(ndfp);
         if (ndfs != nullptr)
         {
            for (const auto& mti : ndfs->data)
            {
               for (const auto& satIt : mti.second)
               {
                  uniqueSat.insert(satIt.first);
               }
            }
         }
      }
      return uniqueSat.size();
   }


   void MultiFormatNavDataFactory ::
   setValidityFilter(NavValidityType nvt)
   {
         // This will end up setting the validity filter multiple
         // times for any factory that has multiple supported signals,
         // but the end result is the same whether we check for
         // duplicates or not.
      for (auto& i : factories())
      {
         i.second->setValidityFilter(nvt);
      }
   }


   void MultiFormatNavDataFactory ::
   setTypeFilter(const NavMessageTypeSet& nmts)
   {
         // This will end up setting the type filter multiple times
         // for any factory that has multiple supported signals, but
         // the end result is the same whether we check for duplicates
         // or not.
      for (auto& i : factories())
      {
         i.second->setTypeFilter(nmts);
      }
   }


   bool MultiFormatNavDataFactory ::
   addFactory(NavDataFactoryPtr& fact)
   {
         // make sure it's a valid class first
      NavDataFactory *ndfp = fact.get();
      // std::cerr << "MultiFormatNavDataFactory::addFactory() ndfp = " << ndfp << std::endl;
      if (dynamic_cast<NavDataFactoryWithStoreFile*>(ndfp) == nullptr)
      {
         return false;
      }
      if (dynamic_cast<MultiFormatNavDataFactory*>(ndfp) != nullptr)
      {
         return false;
      }
         // Yes, we do add multiple copies of the NavDataFactoryPtr to
         // the map, it's a convenience.
      for (const auto& si : fact->supportedSignals)
      {
         factories().insert(NavDataFactoryMap::value_type(si,fact));
      }
      return true;
   }


   bool MultiFormatNavDataFactory ::
   addDataSource(const std::string& source)
   {
         // factories can have multiple copies of a given factory, so
         // keep track of which ones we've checked already.
      std::set<NavDataFactory*> ptrs;
      for (auto& fi : factories())
      {
         NavDataFactory *ptr = fi.second.get();
         if (ptrs.count(ptr) == 0)
         {
            ptrs.insert(ptr);
            NavDataFactoryWithStoreFile *fact =
               dynamic_cast<NavDataFactoryWithStoreFile*>(ptr);
            if (fact != nullptr)
            {
               if (fact->addDataSource(source))
               {
                  return true;
               }
            }
         }
      }
         // none of the existing factories were able to load the data
      return false;
   }


   void MultiFormatNavDataFactory ::
   dump(std::ostream& s, NavData::Detail dl) const
   {
         // factories can have multiple copies of a given factory, so
         // keep track of which ones we've checked already.
      std::set<NavDataFactory*> ptrs;
      for (auto& fi : factories())
      {
         NavDataFactory *ptr = fi.second.get();
         if (ptrs.count(ptr) == 0)
         {
            ptrs.insert(ptr);
            ptr->dump(s,dl);
         }
      }
   }


   std::string MultiFormatNavDataFactory ::
   getFactoryFormats() const
   {
         // factories can have multiple copies of a given factory, so
         // keep track of which ones we've checked already.
      std::set<NavDataFactory*> ptrs;
      std::string rv;
      for (const auto& fi : factories())
      {
         NavDataFactory *ptr = fi.second.get();
         if (ptrs.count(ptr) == 0)
         {
            ptrs.insert(ptr);
            std::string ff(ptr->getFactoryFormats());
            if (!ff.empty())
            {
               if (!rv.empty())
                  rv += ", ";
               rv += ff;
            }
         }
      }
      return rv;
   }


   NavDataFactoryMap& MultiFormatNavDataFactory ::
   factories()
   {
      static NavDataFactoryMap rv;
      return rv;
   }
}