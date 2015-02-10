#ifndef _SGEXCEPTION_HPP
#define _SGEXCEPTION_HPP

#include "sgcommon.hpp"

//! SGSolve specific exceptions
/*! This class is derived from std::exception and handles special
    messages that are informative about abnormal behavior in SGSolver
    and associated classes. */
class SGException : public std::exception
{
public:
  enum EXCEPTION_TYPE 
    { 
      FAILED_OPEN, /*!< SGData was not able to open the given file. */
      TOO_MANY_PIVOT_UPDATES, /*!< Pivot did not converge within the
                                 allowed number of updates. */
      NO_ADMISSIBLE_DIRECTION, /*!< SGApprox::findBestDirection() was
                                  not able to find an admissible
                                  direction. */
      UNKNOWN_PARAM, /*!< Parameter name does not correspond to any
                        in SGEnv. */
      BAD_PARAM_VALUE, /*!< Parameter value is not admissible. */
      TUPLE_SIZE_MISMATCH, /*!< Tried to perform arithmetic operation
                             on SGTuples or vectors of different
                             sizes. */
      EMPTY_TUPLE, /*!< Tried to perform arithmetic operation on an
                     empty tuple. */
      OUT_OF_BOUNDS, /*!< Passed an argument that was out of bounds,
                       e.g., player < 0 or state >= numStates. */
      DIVIDE_BY_ZERO, /*!< Encountered an attempt to divide by zero. */
      INCONSISTENT_INPUTS /*!< Inputs are incompatible or inconsistent
			    with one another, e.g., arrays that should
			    be the same size are not. */
    };

  //! Constructor for new SGException.
  SGException(EXCEPTION_TYPE _type):type(_type) {}

  //! Returns the message corresponding to the ExceptionType
  virtual const char* what() const throw()
  {
    switch (type)
      {
      case TOO_MANY_PIVOT_UPDATES:
	return "Too many calls to SGSolver::updatePivot() in SGSolver::solve()";
      case NO_ADMISSIBLE_DIRECTION:
	return "bestAction==NULL. Could not find an admissible direction.";
      case FAILED_OPEN:
	return "Unable to open file.";
      case UNKNOWN_PARAM:
	return "Could not recognize parameter name.";
      case TUPLE_SIZE_MISMATCH:
	return "You tried to perform an arithmetic operation on tuples of different sizes";
      case OUT_OF_BOUNDS:
	return "You passed an argument that did not satisfy bound restrictions.";
      case DIVIDE_BY_ZERO:
	return "You cannot divide by zero.";
      case INCONSISTENT_INPUTS:
	return "Inputs are incompatible or inconsistent with one another, e.g., arrays that should be the same size are not.";

      default:
	return "No message specified.";
      }
  }

  //! Returns the type of the exception.
  EXCEPTION_TYPE getType()
  {
    return type;
  }
 
private:
  EXCEPTION_TYPE type; /*!< Enumeration indicate the type of error
                         encountered. */
};

#endif
