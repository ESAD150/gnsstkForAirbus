#!/usr/bin/env python

"""
A GNSSTk example featuring some file input and processing to
create a plot with matplotlib.

Usage:

  python sem_plot.py

==============================================================================

  This file is part of GNSSTk, the GNSS Toolkit.

  The GNSSTk is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 3.0 of the License, or
  any later version.

  The GNSSTk is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with GNSSTk; if not, write to the Free Software Foundation,
  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA

  This software was developed by Applied Research Laboratories at the
  University of Texas at Austin.
  Copyright 2004-2021, The Board of Regents of The University of Texas System

==============================================================================

==============================================================================

  This software was developed by Applied Research Laboratories at the
  University of Texas at Austin, under contract to an agency or agencies
  within the U.S. Department of Defense. The U.S. Government retains all
  rights to use, duplicate, distribute, disclose, or release this software.

  Pursuant to DoD Directive 523024

  DISTRIBUTION STATEMENT A: This software has been approved for public
                            release, distribution is unlimited.

==============================================================================
"""

import gnsstk
import matplotlib.pyplot as plt


def main():
    # Read in data, strict=True makes dataSets a list rather than a generator:
    header, dataSets = gnsstk.readSEM( gnsstk.data.full_path('sem_data.txt'), strict=True)

    # Write the data back to a different file (their contents should match):
    gnsstk.writeSEM( gnsstk.data.full_path('sem_data.txt.new'), header, dataSets)

    # Read the orbit out of the data:
    orbit = dataSets[0].toAlmOrbit()  # alm orbit of first data point

    austin = gnsstk.Position(30, 97, 0, gnsstk.Position.Geodetic)  # Austin, TX

    starttime = gnsstk.CommonTime()    # iterator time to start at
    starttime.setTimeSystem(gnsstk.TimeSystem('GPS'))
    endtime = gnsstk.CommonTime()  # end time, 1 day later (see below)
    endtime.setTimeSystem(gnsstk.TimeSystem('GPS'))
    endtime.addDays(1)

    X = []
    Y = []

    # Step through a day, adding plot points:
    for t in gnsstk.times(starttime, endtime, seconds=1000):
        xvt = orbit.svXvt(t)
        location = gnsstk.Position(xvt.x)
        elevation = austin.elevation(location)
        X.append(t.getDays())
        Y.append(elevation)

    # Make the plot
    fig = plt.figure()
    fig.suptitle('Elevation of a GPS satellite throughout the day',
                 fontsize=14, fontweight='bold')
    ax = fig.add_subplot(111)
    ax.set_xlabel('Time (days)')
    ax.set_ylabel('Elevation (degrees)')
    plt.plot(X, Y, 'r')
    plt.show()


if __name__ == '__main__':
    main()
