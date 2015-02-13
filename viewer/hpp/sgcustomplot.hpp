#ifndef SGCUSTOMPLOT_HPP
#define SGCUSTOMPLOT_HPP

#include "qcustomplot.h"

class SGCustomPlot : public QCustomPlot
{
  Q_OBJECT;
private:
  QCPPlotTitle * title;
  QCPRange nominalXRange;
  QCPRange nominalYRange;
  QCPRange realXRange;
  QCPRange realYRange;

  QString path;

  QAction * savePNGAction;
  QAction * savePDFAction;

public:
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

  

  QCPPlotTitle * getTitle() {return title;}

  void adjustRanges();

  void setRanges(const QCPRange & xrange,
		 const QCPRange & yrange);

protected:
  void resizeEvent(QResizeEvent * event)
  {
    adjustRanges();
    QCustomPlot::resizeEvent(event);
  }

private slots:
  void ShowContextMenu(const QPoint & pos)
  {
    QPoint globalPos = this->mapToGlobal(pos);

    QMenu contextMenu;
    contextMenu.addAction(savePDFAction);
    contextMenu.addAction(savePNGAction);
    
    contextMenu.exec(globalPos);
  }

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
  // Suggest a square aspect ratio.
  virtual int heightForWidth(int w) const {return w;}
  virtual bool hasHeightForWidth() const {return true;}
  virtual QSize minimumSizeHint() const { return QSize(100,100); } 
};

#endif
