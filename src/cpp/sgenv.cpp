#include "sgenv.hpp"

SGEnv::SGEnv()
{
  doubleParams = vector<double>(NUMDOUBLEPARAMS,0);
  boolParams = vector<bool>(NUMBOOLPARAMS,0);
  intParams = vector<int>(NUMINTPARAMS,0);
  restoreDefaults();
}

void SGEnv::restoreDefaults()
{
  intParams[MAXITERATIONS] = 1e6;
  intParams[MAXUPDATEPIVOTPASSES] = 1e8;
  intParams[TUPLERESERVESIZE] = 1e4;
  intParams[STOREITERATIONS] = 2;

  doubleParams[ERRORTOL] = 1e-8;
  doubleParams[DIRECTIONTOL] = 1e-11;
  doubleParams[PASTTHREATTOL] = 1e-10;
  doubleParams[UPDATEPIVOTTOL] = 1e-13;
  doubleParams[ICTOL] = 1e-12;
  doubleParams[NORMTOL] = 1e-12;
  doubleParams[FLATTOL] = 1e-10;
  doubleParams[LEVELTOL] = 1e-12;
  doubleParams[IMPROVETOL] = 1e-13;
  doubleParams[BACKBENDINGTOL] = 1e-6;
  doubleParams[MOVEMENTTOL] = 1e-14; 
  doubleParams[ROUNDTOL] = 0.0;
  doubleParams[INTERSECTTOL] = 1e-10;

  boolParams[MERGETUPLES] = false;
  boolParams[BACKBENDINGWARNING] = false;
  boolParams[PRINTTOCOUT] = true;

  setOStream(cout);
}

void SGEnv::setParam(SGEnv::DBL_PARAM param, double value)
{
  if (value<0)
    throw(SGException(SGException::BAD_PARAM_VALUE));

  if (param < 0 || param > NUMDOUBLEPARAMS)
    throw(SGException(SGException::UNKNOWN_PARAM));

  doubleParams[param] = value;
} // setParam

void SGEnv::setParam(SGEnv::BOOL_PARAM param, bool value)
{
  if (param < 0 || param > NUMBOOLPARAMS)
    throw(SGException(SGException::UNKNOWN_PARAM));
  boolParams[param] = value;
} // setParam

void SGEnv::setParam(SGEnv::INT_PARAM param, int value)
{
  if (value<0)
    throw(SGException(SGException::BAD_PARAM_VALUE));

  if (param < 0 || param > NUMINTPARAMS)
      throw(SGException(SGException::UNKNOWN_PARAM));
  intParams[param] = value;
} // setParam

double SGEnv::getParam(SGEnv::DBL_PARAM param) const
{
  if (param < 0 || param > NUMDOUBLEPARAMS)
    throw(SGException(SGException::UNKNOWN_PARAM));

  return doubleParams[param];
} // getParam

bool SGEnv::getParam(SGEnv::BOOL_PARAM param) const
{
  if (param < 0 || param > NUMBOOLPARAMS)
    throw(SGException(SGException::UNKNOWN_PARAM));
  return boolParams[param];
} // getParam

int SGEnv::getParam(SGEnv::INT_PARAM param) const
{
  if (param < 0 || param > NUMINTPARAMS)
      throw(SGException(SGException::UNKNOWN_PARAM));
  return intParams[param];
} // getParam
