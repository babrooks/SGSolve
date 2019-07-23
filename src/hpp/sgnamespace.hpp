// This file is part of the SGSolve library for stochastic games
// Copyright (C) 2019 Benjamin A. Brooks
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

#ifndef SGNAMESPACE_HPP
#define SGNAMESPACE_HPP

//! Namespace for enumerations associated with SGSolve
namespace SG
{
  //! Code for the type of error.
  enum EXCEPTION_TYPE 
    {
      DEFAULT, /*!< No specific error */
      FAILED_OPEN, /*!< SGData was not able to open the given file. */
      PROB_SUM_NOT1, /*!< Probabilities did not sum to 1. */
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
      INCONSISTENT_INPUTS, /*!< Inputs are incompatible or inconsistent
			    with one another, e.g., arrays that should
			    be the same size are not. */
      SIMERROR, /*!< Simulator error. */
      NOFEASIBLETUPLE, /*!< Could not find a feasible tuple for APS. */
      NOEDGES, /*!< Could not find an edge in a product policy. */
      NOACTIONS, /*!< Could not find an action. */
      NOOPTIMALPOLICIES, /*!< Could not find an optimal policy for some state. */
      WRONG_NUMBER_OF_PLAYERS, /*!< Number of players in incomaptible with viewer */
    };

  //! Double parameters
  /*! An enumeration type for parameters of type double. Pass this
    argment to SGEnv::setParam, along with the new value, to
    change the parameter. */
  enum DBL_PARAM
    {
      ERRORTOL,/*!< The algorithm stops when the distance between
		 successive revolutions is below this level. */
      DIRECTIONTOL,/*!< Threshold for changing cardinal directions. */
      PASTTHREATTOL,/*!< Minimum increase in the threat tuple at which
		      the minimum IC continuation values are
		      rebuilt. */
      UPDATEPIVOTTOL,/*!< The algorithm stops updating the pivot when
		       successive updates are below this level. */
      POLICYITERTOL, /*! Tolerance for convergence in policy function iteration */
      ICTOL, /*!< Maximum allowable violation of IC constraints. */
      NORMTOL, /*!< Vectors with a Euclidean norm below this level are
		 considered zero. */
      FLATTOL, /*!< When the cosine of an angle is below this level,
		 it is considered to be zero. */
      LEVELTOL, /*!< Maximum cosine for a binding direction to be
		  considered below the non-binding direction. */
      IMPROVETOL, /*!< Minimum cosine relative to the current best
		    direction for the new direction to be considered
		    an improvement. */
      ROUNDTOL, /*!< When positive, the algorithm will round off the
		  pivot coordinates to this degree of accuracy. */
      BACKBENDINGTOL, /*!< When the cosine of a direction relative to
			the current direction is above this
			threshold, the direction is considered
			"back-bending". */
      MOVEMENTTOL, /*!< If the movement of the pivot in multiples of
                     the current direction is below this threshold,
                     the new pivot will replace the current last
                     extreme point. */
      INTERSECTTOL, /*! Controls the length of segment that is
		      sufficient to take an intersection in
		      SGAction. */
      BNDRYPNTTOL, /*! Distance below which extreme binding
                       continuation values are considered equal . */
      SUBGENFACTOR, /*! Scaling of the hyperplanes to test for supergeneration. */
      NUMDOUBLEPARAMS /*!< Used internally to indicate the number of
			enumerated double parameters. */
    };

  //! Boolean parameters
  /*! An enumeration type for boolean parameters. Used with
    SGEnv::setParam to set boolean parameter values. */
  enum BOOL_PARAM
    {
      BACKBENDINGWARNING, /*!< If true, the algorithm will issue a
			    warning to stdout when a back-bending
			    direction is detected. */
      MERGETUPLES, /*!< If true, the algorithm will replace the last
		     extreme tuple with the new pivot when either a
		     flat has been detected or the movement in the
		     pivot is too small. */
      PRINTTOLOG, /*!< If true, the algorithm will print progress to a
                    log file. */
      PRINTTOCOUT, /*!< If true, the algorithm will print progress to
                     cout. */
      STOREACTIONS, /*!< If true, the algorithm will store all
                       actions available at the given iteration. */
      CHECKSUFFICIENT, /*!< If true, the algorithm will check a
                          sufficient condition for the pivot to not
                          cut into the equilibrium payoff
                          correspondence. */
      NUMBOOLPARAMS /*!< Used internally to indicate the number of
		      enumerated bool parameters. */
    };

  //! Integer parameters
  /*! An enumeration type for int parameters. Used with
    SGEnv::setParam to set integer parameter values. */
  enum INT_PARAM
    {
      MAXITERATIONS,/*!< The algorithm terminates after this number of
		      iterations has been reached. */
      MAXPOLICYITERATIONS,/*!< Maximum number of policy iterations.*/
      MAXUPDATEPIVOTPASSES, /*!< The algorithm stops updating the
			      pivot after this many updates. */
      STOREITERATIONS, /*!< Controlls whether or not the algorithm
                         stores information about the pivot, best
                         action, and best direction at each
                         iteration. */
      TUPLERESERVESIZE, /*!< The amount by which the extremeTuples
                          member of SGApproximation is incremented
                          when the capacity is reached. */
      NUMINTPARAMS /*!< Used internally to indicate the number of
		     enumerated int parameters. */
    };

  //! Indicates which incentive constraints are binding
  enum Regime {NonBinding, Binding, Binding0, Binding1, Binding01};

}


#endif
