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

#ifndef SGTABLEVIEW_HPP
#define SGTABLEVIEW_HPP

#include <QtWidgets>
#include <QAbstractItemView>

//! Specialized table view for SGViewer
/*! Reimplements various sizing methods and configures the size policy
  and selection mode. 

  \ingroup viewer
*/
class SGTableView : public QTableView
{
  Q_OBJECT

public:
  //! Constructor
  SGTableView ()
  {
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    // horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  }

  //! Reimplement column size to be slightly smaller
  int sizeHintForColumn(int col) const
  {
    return 65;
  }

  //! Reimplement minimumSizeHint 
  /*! Recommends a size so that the table is just larger than what is
      required to show all of the cells. */
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

    hint.setHeight(newHeight);
    hint.setWidth(newWidth);
    
    return hint;
  }

  //! Hint is equal to minimum size so table wont grow.
  QSize sizeHint() const
  {
    return minimumSizeHint();
  }

};

#endif
