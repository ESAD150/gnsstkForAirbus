#!/usr/bin/python
"""
==============================================================================

  This file is part of GNSSTk, the ARL:UT GNSS Toolkit.

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
  Copyright 2004-2022, The Board of Regents of The University of Texas System

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
import sys
import os
import argparse
import unittest

import gnsstk

args = None


def run_unit_tests():
    """A function to run unit tests without using the argument parsing of
    unittest.main() """

    parser = argparse.ArgumentParser(description="Test app for ctest")
    parser.add_argument('-v,--verbose', dest='verbose', action='count',
                        help="Increase the amount of output generated by the program")
    parser.add_argument('-o,--output_dir', dest='output_dir', metavar='dir',
                        help="Path to output directory")
    parser.add_argument('-i,--input_dir', dest='input_dir', metavar='dir',
                        help="Path to root of input data directory")
    global args
    args=parser.parse_args()

    runner=unittest.TextTestRunner()

    # Find all Test classes in the parent script
    script=os.path.basename(sys.argv[0])
    dir=os.path.dirname(sys.argv[0])
    isuite = unittest.TestLoader().discover(dir, pattern=script)

    rc = runner.run(isuite)
    sys.exit(len(rc.failures) + len(rc.errors))


def assertSequenceAlmostEqual(test_case, l1, l2, **kwargs):
    """
    Compare two lists of elements where the elements are almost equal to each other

    Uses `assertAlmostEqual` to compare elements between `l1` and `l2`.
    `l1` and `l2` must be equal in length.
    """
    test_case.assertEqual(len(l1), len(l2))
    for l1_i, l2_i in zip(l1, l2):
        test_case.assertAlmostEqual(l1_i, l2_i, **kwargs)


def vec_to_list(s):
    """
    A kludge since many swigged gnsstk objects are not properly iterable
    """
    return [s[i] for i in range(s.size())]

