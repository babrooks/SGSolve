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

#ifndef SGCUSTOMPLOT_HPP
#define SGCUSTOMPLOT_HPP

#include "qcustomplot.h"
#include "sgpoint.hpp"

//! A customized version of QCustomPlot.
/*! This class inherits from QCustomPlot and adds functionality for
  controling ranges to maintain the desired aspect ratio. It also adds
  functionality for right-clicking on the plot to bring up a context
  menu. This menu has various options, some of which pertain to the
  payoffs closest to where the user clicked. In particular, there are
  signals for saving copys of the plots, for "inspecting" a given
  payoff vector, and for initializing the SGSimulationHandler to
  forward simulate the equilibrium that generates the given payoffs.

  \ingroup viewer
*/
class SGCustomPlot : public QCustomPlot
{
  Q_OBJECT;
private:
  //! Indicates the state that this plot is associated with.
  int state;
  //! Indicates if this is the detail plot.
  bool isDetailPlot;

  QCPPlotTitle * title; /*!< The graph title. */
  QCPRange nominalXRange; /*!< Nominal X range. */
  QCPRange nominalYRange; /*!< Nominal Y range. */
  QCPRange realXRange; /*!< Real X range. */
  QCPRange realYRange; /*!< Real Y range. */

  //! Save file path
  /*!< Stores the last location used to save a picture. */
  QString path; 

  //! Inspect a point
  QAction * inspectPointAction;

  //! Action for forward simulating
  QAction* simulateAction;

  //! Pointer to the QAction for saving PNG files.
  QAction * savePNGAction;
  //! Pointer to the QAction for saving PDF files.
  QAction * savePDFAction;

  //! Stores the last location at which a context menu was requested.
  QPoint lastContextPos;

public:
  //! Constructor
  SGCustomPlot(): SGCustomPlot(0,false) {}

  //! Constructor 
  /*! Sets the state and whether or not is the detailPlot. Also
      initializes the plot and connect slots to actions. */
  SGCustomPlot(int _state, bool _isDetailPlot);

  //! Returns the title.
  QCPPlotTitle * getTitle() {return title;}

  //! Updates ranges when window is resized.
  void adjustRanges();

  //! Sets the nominal ranges.
  void setRanges(const QCPRange & xrange,
		 const QCPRange & yrange);

  //! Returns the nominal X range
  const QCPRange & getNominalXRange() const
  { return nominalXRange; }

  //! Returns the nominal Y range
  const QCPRange & getNominalYRange() const
  { return nominalYRange; }
  
  //! Normalize ranges
  void equalizeAxesScales();

  //! Sets the state associated with the plot
  void setState(int newState)
  { assert(state>=0); state = newState; }
  //! Gets the state associated with the plot
  int getState() const
  { return state; }

protected:
  //! Reimplement resizeEvent
  /*! Calls adjust ranges before calling QCustomPlot::resizeEvent. */
  void resizeEvent(QResizeEvent * event)
  {
    adjustRanges();
    QCustomPlot::resizeEvent(event);
  }

private slots:
  //! Slot for showing context menu. 
  /*! Creates a context menu and shows the actions for saving PDF/PNG
      files. */
  void ShowContextMenu(const QPoint & pos)
  {
    lastContextPos = pos;

    QPoint globalPos = this->mapToGlobal(pos);
    
    QMenu contextMenu;
    contextMenu.addAction(inspectPointAction);
    contextMenu.addAction(simulateAction);
    contextMenu.addSeparator();
    contextMenu.addAction(savePDFAction);
    contextMenu.addAction(savePNGAction);
    
    contextMenu.exec(globalPos);
  }

  //! Point inspected
  void pointInspected()
  {
    emit inspectPoint(SGPoint(xAxis->pixelToCoord(lastContextPos.x()),
			      yAxis->pixelToCoord(lastContextPos.y())),
		      state,isDetailPlot);
  }

  void simulationRequested()
  {
    emit simulateEquilibrium(SGPoint(xAxis->pixelToCoord(lastContextPos.x()),
			      yAxis->pixelToCoord(lastContextPos.y())),
			     state,isDetailPlot); 

  }

  //! Saves graph as a PDF.
  void savePDF()
  {
    QString newPath = QFileDialog::getSaveFileName(this, tr("Save PDF"),
						   path, tr("PDF files (*.pdf)"));
  
    if (newPath.isEmpty())
      return;

    QFileInfo fi(newPath);
    path = fi.canonicalPath();

    savePdf(newPath);
  }

  //! Saves graph as a PNG.
  void savePNG()
  {
    QString newPath = QFileDialog::getSaveFileName(this, tr("Save PNG"),
						   path, tr("PNG files (*.png)"));
  
    if (newPath.isEmpty())
      return;

    QFileInfo fi(newPath);
    path = fi.canonicalPath();

    savePng(newPath);
  }


private:
  //! Reimplement heightForWidth 
  /*! Suggests a square aspect ratio. */
  virtual int heightForWidth(int w) const {return w;}

  //! Reimplement hasHeighForWidth
  /*! Indicates that heightForWidth has been reimplemented. */
  virtual bool hasHeightForWidth() const {return true;}

  //! Custom minimum size.
  virtual QSize minimumSizeHint() const { return QSize(100,100); } 

signals:
  //! Signal to inspect the given point
  void inspectPoint(SGPoint point, 
		    int state, 
		    bool isDetailPlot);

  //! Signal to simulate the given equilibrium forwards
  void simulateEquilibrium(SGPoint point,
			   int state,
			   bool isDetailPlot); 

};

#endif
