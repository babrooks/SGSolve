#ifndef _SGENV_HPP
#define _SGENV_HPP

#include "sgexception.hpp"

//! Manages parameters for algorithm behavior
/*!  This class contains parameters for the algorithm.

  \ingroup src
 */
class SGEnv
{
private:
  // Parameters

  // Double parameters
  double errorTol; /*!< Level at which algorithm converges. */
  double directionTol; /*!< Threshhold for changing cardinal directions. */
  double updatePivotTol; /*!< Threshhold for pivot update. */
  double pastThreatTol; /*!< Threshhold for updating the threat tuple. */
  double ICTol; /*!< Threshold for violation of IC constraints. */
  double normTol; /*!< Minimum norm for admissible directions. */
  double flatTol; /*!< Direction changes less than this will be merged. */
  double levelTol; /*!< Closeness to non-binding direction to test
		     feasibility. */
  double improveTol; /*!< Minimum improvement in direction to replace
		       best direction. */
  double roundTol; /*!< Pivot coordinates are rounded off to this tol. */
  double backBendingTol; /*!< Threshold improvement for considering a
			   direction "back-bending". */
  double movementTol; /*!< Movements below this will replace the
			previous pivot. */
  double intersectTol; /*!< Threshold difference in levels above which
                          the SGAction class will consider two points
                          to be distinct. */

  // Bool parameters
  bool backBendingWarning; /*!< Flag to issue warnings when back-bending
			     detected. */
  bool mergeTuples; /*!< Flag to control whether or not flats are merged. */
  bool storeIterations; /*!< If true, store iterations in data. */
  bool printToLog; /*!< If true, prints progress to a log file. */
  bool printToCout; /*!< If true, prints progress to cout. */
  
  // Int parameters
  long int maxIterations; /*!< Maximum number of iterations. */
  long int maxUpdatePivotPasses; /*!< Maximum number of passes to update
			      pivot. */
  int tupleReserveSize; /*!< Increment the extremeTuples array by this
			  amount when capacity reached. */

  //! Output stream. Not currently used.
  ofstream os; 
  
public:

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
      MOVEMENTTOL /*!< If the movement of the pivot in multiples of
                     the current direction is below this threshold,
                     the new pivot will replace the current last
                     extreme point. */
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
      STOREITERATIONS, /*!< If true, the algorithm will store
                         information about the pivot, best action, and
                         best direction at each iteration. */
      PRINTTOLOG, /*!< If true, the algorithm will print progress to a
                    log file. */
      PRINTTOCOUT /*!< If true, the algorithm will print progress to
                     cout. */
    };

  //! Integer parameters
  /*! An enumeration type for int parameters. Used with
      SGEnv::setParam to set integer parameter values. */
  enum INT_PARAM
    {
      MAXITERATIONS,/*!< The algorithm terminates after this number of
                       iterations has been reached. */
      MAXUPDATEPIVOTPASSES, /*!< The algorithm stops updating the
                               pivot after this many updates. */
      TUPLERESERVESIZE /*!< The amount by which the extremeTuples
                          member of SGApproximation is incremented
                          when the capacity is reached. */
    };

  //! Constructor
  /*! Creates a new SGEnv object and initializes with default
      parameter values. */
  SGEnv();

  ~SGEnv() {}

  //! Method for setting double parameters.
  void setParam(SGEnv::DBL_PARAM param, double value);

  //! Method for setting boolean parameters.
  void setParam(SGEnv::BOOL_PARAM param, bool value);

  //! Method for setting integer parameters.
  void setParam(SGEnv::INT_PARAM param, int value);

  //! Method for getting double parameters.
  double getParam(SGEnv::DBL_PARAM param) const;

  //! Method for getting boolean parameters.
  bool getParam(SGEnv::BOOL_PARAM param) const;

  //! Method for getting integer parameters.
  int getParam(SGEnv::INT_PARAM param) const;

  //! Method for redirecting output
  void setOStream(ostream & newOS)
  {
    os.basic_ios<char>::rdbuf(newOS.rdbuf());
  }

  //! Return reference to the output stream.
  ofstream & getOS() { return os; }
  
  //! Method for restoring default values for all parameters.
  void restoreDefaults();

  friend class SGApprox;
  friend class SGAction;
  friend class SGSolver;
  friend class SGMainWindow;
  friend class SGSolverWorker;
};

#endif
