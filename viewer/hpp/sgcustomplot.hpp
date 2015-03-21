#ifndef SGCUSTOMPLOT_HPP
#define SGCUSTOMPLOT_HPP

#include "qcustomplot.h"

//! A customized version of QCustomPlot.
/*! This class inherits from QCustomPlot and adds functionality for
  controling ranges to maintain the desired aspect ratio. It also
  adds functionality for saving graphs.

  \ingroup viewer
*/
class SGCustomPlot : public QCustomPlot
{
  Q_OBJECT;
private:
  QCPPlotTitle * title; /*!< The graph title. */
  QCPRange nominalXRange; /*!< Nominal X range. */
  QCPRange nominalYRange; /*!< Nominal Y range. */
  QCPRange realXRange; /*!< Real X range. */
  QCPRange realYRange; /*!< Real Y range. */

  //! Save file path
  /*!< Stores the last location used to save a picture. */
  QString path; 

  //! Pointer to the QAction for saving PNG files.
  QAction * savePNGAction;
  //! Pointer to the QAction for saving PDF files.
  QAction * savePDFAction;

public:
  //! Constructor
  /*! Initializes the plot and connect slots to actions. */
  SGCustomPlot() : QCustomPlot()
  {
    path = QString("./");

    QSizePolicy qsp(QSizePolicy::Preferred,
		    QSizePolicy::Minimum);
    qsp.setHeightForWidth(true);

    setSizePolicy(qsp);

    QFont font = xAxis->labelFont();
    font.setPointSize(10);
    xAxis->setTickLabelFont(font);
    yAxis->setTickLabelFont(font);

    title = new QCPPlotTitle(this,"");
    font = title->font();
    font.setPointSize(12);
    title->setFont(font);
  
    plotLayout()->insertRow(0);
    plotLayout()->addElement(0,0,title);

    setContextMenuPolicy(Qt::CustomContextMenu);

    savePDFAction = new QAction(tr("Save &PDF"), this);
    savePNGAction = new QAction(tr("Save P&NG"), this);

    connect(savePDFAction,SIGNAL(triggered()),
	    this,SLOT(savePDF()));
    connect(savePNGAction,SIGNAL(triggered()),
	    this,SLOT(savePNG()));

    connect(this,SIGNAL(customContextMenuRequested(const QPoint &)),
	    this,SLOT(ShowContextMenu(const QPoint &)));
  }

  
  //! Returns the title.
  QCPPlotTitle * getTitle() {return title;}

  //! Updates ranges when window is resized.
  void adjustRanges();

  //! Sets the nominal ranges.
  void setRanges(const QCPRange & xrange,
		 const QCPRange & yrange);

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
    QPoint globalPos = this->mapToGlobal(pos);

    QMenu contextMenu;
    contextMenu.addAction(savePDFAction);
    contextMenu.addAction(savePNGAction);
    
    contextMenu.exec(globalPos);
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
};

#endif
