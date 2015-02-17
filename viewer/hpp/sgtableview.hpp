#ifndef SGTABLEVIEW_HPP
#define SGTABLEVIEW_HPP

#include <QtWidgets>
#include <QAbstractItemView>

class SGTableView : public QTableView
{
  Q_OBJECT

public:
  SGTableView ()
  {
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    // horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  }

  int sizeHintForColumn(int col) const
  {
    return 65;
  }

  QSize minimumSizeHint() const
  {
    QSize hint(QTableView::sizeHint());

    double newWidth = 2; // Start with a little buffer
    for (int col = 0; col < model()->columnCount(); col++)
      newWidth += columnWidth(col);
    newWidth += verticalHeader()->sizeHint().width();
    
    double newHeight = 2; // Start with a little buffer
    for (int row = 0; row < model()->rowCount(); row++)
      newHeight += rowHeight(row);
    
    newHeight += horizontalHeader()->sizeHint().height();

    qDebug() << "new height = " << newHeight << endl;

    qDebug() << "number of rows = " << model()->rowCount() << endl;
    hint.setHeight(newHeight);
    hint.setWidth(newWidth);
    
    return hint;
  }

  QSize sizeHint() const
  {
    return minimumSizeHint();
  }

  void resizeEvent(QResizeEvent * event)
  {
    QAbstractItemView::resizeEvent(event);
    // resizeColumnsToContents();
  }
  
};

#endif
