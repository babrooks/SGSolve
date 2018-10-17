// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2016 Benjamin A. Brooks
// 
// SGSolve free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// SGSolve is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
// 
// Benjamin A. Brooks
// ben@benjaminbrooks.net
// Chicago, IL

#include "sgenv.hpp"

SGEnv::SGEnv()
{
  doubleParams = vector<double>(SG::NUMDOUBLEPARAMS,0);
  boolParams = vector<bool>(SG::NUMBOOLPARAMS,0);
  intParams = vector<int>(SG::NUMINTPARAMS,0);
  restoreDefaults();
}

void SGEnv::restoreDefaults()
{
  intParams[SG::MAXITERATIONS] = 1e6;
  intParams[SG::MAXUPDATEPIVOTPASSES] = 1e8;
  intParams[SG::TUPLERESERVESIZE] = 1e4;
  intParams[SG::MAXPOLICYITERATIONS] = 1e2;
  intParams[SG::STOREITERATIONS] = 2;

  doubleParams[SG::ERRORTOL] = 1e-8;
  doubleParams[SG::DIRECTIONTOL] = 1e-11;
  doubleParams[SG::PASTTHREATTOL] = 1e-10;
  doubleParams[SG::UPDATEPIVOTTOL] = 1e-13;
  doubleParams[SG::POLICYITERTOL] = 1e-9;
  doubleParams[SG::ICTOL] = 1e-10;
  doubleParams[SG::NORMTOL] = 1e-12;
  doubleParams[SG::FLATTOL] = 1e-10;
  doubleParams[SG::LEVELTOL] = 1e-12;
  doubleParams[SG::IMPROVETOL] = 1e-13;
  doubleParams[SG::BACKBENDINGTOL] = 1e-6;
  doubleParams[SG::MOVEMENTTOL] = 1e-14; 
  doubleParams[SG::ROUNDTOL] = 0.0;
  doubleParams[SG::INTERSECTTOL] = 1e-10;

  boolParams[SG::MERGETUPLES] = false;
  boolParams[SG::BACKBENDINGWARNING] = false;
  boolParams[SG::PRINTTOCOUT] = true;
  boolParams[SG::CHECKSUFFICIENT] = true;
  boolParams[SG::STOREACTIONS] = true;

  // setOStream(cout);
}

void SGEnv::setParam(SG::DBL_PARAM param, double value)
{
  if (value<0)
    throw(SGException(SG::BAD_PARAM_VALUE));

  if (param < 0 || param > SG::NUMDOUBLEPARAMS)
    throw(SGException(SG::UNKNOWN_PARAM));

  doubleParams[param] = value;
} // setParam

void SGEnv::setParam(SG::BOOL_PARAM param, bool value)
{
  if (param < 0 || param > SG::NUMBOOLPARAMS)
    throw(SGException(SG::UNKNOWN_PARAM));
  boolParams[param] = value;
} // setParam

void SGEnv::setParam(SG::INT_PARAM param, int value)
{
  if (value<0)
    throw(SGException(SG::BAD_PARAM_VALUE));

  if (param < 0 || param > SG::NUMINTPARAMS)
      throw(SGException(SG::UNKNOWN_PARAM));
  intParams[param] = value;
} // setParam

double SGEnv::getParam(SG::DBL_PARAM param) const
{
  if (param < 0 || param > SG::NUMDOUBLEPARAMS)
    throw(SGException(SG::UNKNOWN_PARAM));

  return doubleParams[param];
} // getParam

bool SGEnv::getParam(SG::BOOL_PARAM param) const
{
  if (param < 0 || param > SG::NUMBOOLPARAMS)
    throw(SGException(SG::UNKNOWN_PARAM));
  return boolParams[param];
} // getParam

int SGEnv::getParam(SG::INT_PARAM param) const
{
  if (param < 0 || param > SG::NUMINTPARAMS)
      throw(SGException(SG::UNKNOWN_PARAM));
  return intParams[param];
} // getParam
