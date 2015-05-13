#ifndef SGSOLUTIONHANDLER_HPP
#define SGSOLUTIONHANDLER_HPP

#include "sgsolution.hpp"
#include "sgcustomplot.hpp"

//! Handles the widgets for displaying the solution
/*! This class contains all of the widgets on the solution tab. It
  contains methods for constructing plots.

  \ingroup viewer
*/
class SGSolutionHandler : public QObject
{
  Q_OBJECT

private:
  //! Solution
  /*! Stores all of the information related to the result of the
      computation. */
  SGSolution soln;

  //! A bound on the size of payoffs
  double payoffBound;

  //! Points to the first iteration from which to plot.
  list<SGIteration>::const_iterator startIter;
  //! Points to the last iteration to which to plot.
  list<SGIteration>::const_iterator endIter;
  //! Points to the pivot iteration.
  list<SGIteration>::const_iterator pivotIter;
  //! Points to the pivot iteration.
  list<SGIteration>::const_iterator startOfLastRev;

  //! True if a solution has been loaded.
  bool solnLoaded;
  
  //! Solution mode
  enum SolutionMode {Progress, Final} mode;
  
  // Plots
  //! Left hand plot that shows construction of a point
  SGCustomPlot * detailPlot;
  //! Vector of state plots
  /*! One plot for each state, shows the trajectory of the pivot for
      that state. */
  vector<SGCustomPlot *> statePlots;

  //! Master layout for the solution handler. 
  QVBoxLayout * layout;

  //! Layout holding the state plots.
  QGridLayout * statePlotsLayout;

  //! Combo controlling the solution mode
  QComboBox * solutionModeCombo;
  
  // Sliders
  //! Slider for the last iteration
  QScrollBar * iterSlider;
  //! Slider for the first iteration
  QScrollBar * startSlider;
  //! Action slider. Not currently used.
  QScrollBar * actionSlider;
  //! State slider. Not currently used.
  QScrollBar * stateSlider;

  //! Action to toggle detailed titles
  QAction * detailedTitlesAction;

  //! Action for equalizing axes
  QAction* equalizeAxesAction;

  // Methods
  //! Plots the solution from start to end
  void plotSolution(int state);
  //! Plots the solution for a particular state.
  void plotSolution(SGCustomPlot * plot, int state,
		    bool addSquares);
  
  //! Gets bounds for the plots.
  QCPRange getBounds(const QVector<double> & x) const;

  //! Sets slider ranges
  void setSliderRanges(int start, int end);
public:
  //! Constructor
  /*! Constructs the widgets and the layout, connects signals and
      slots. */
  SGSolutionHandler();

  //! Sets the solution to newSoln.
  void setSolution(const SGSolution & newSoln);
  //! Returns a const reference to the solution.
  const SGSolution & getSolution() const
  {return soln;}

  //! Returns the layout
  QVBoxLayout * getLayout() const
  { return layout; }
  
  //! Returns the detailed titles action
  QAction * getDetailedTitlesAction() const
  { return detailedTitlesAction; }
  
  //! Returns the equalize axes action
  QAction * getEqualizeAxesAction() const
  { return equalizeAxesAction; }

  //! Increments iter by one.
  void moveForwards();
  //! Decrements iter by one.
  void moveBackwards();

public slots:
  //! iterSlider was updated.
  void iterSliderUpdate(int value);
  //! actionSlider was updated.
  void actionUpdate(int value);
  //! Toggles the solution mode
  void changeMode(int newMode);

  //! Inspect point method
  void inspectPoint(SGPoint point,
		    int state, bool isDetailPlot);
  
  
};

#endif
