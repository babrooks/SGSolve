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

/*!
  \mainpage SGSolve Documentation

  \section introsec Introduction

  SGSolve is a software package for representing, solving, and
  analyzing the equilibria of infinitely repeated stochastic games
  with two players, perfect monitoring, and a public randomization
  device. The library implements algorithms for computing the subgame
  perfect equilibrium payoff correspondence that was invented by Dilip
  Abreu, Ben Brooks, and Yuliy Sannikov (Abreu, Brooks, and Sannikov,
  2016, 2019).

  The package consists of two main components. The first piece is a
  library of C++ classes for representing and solving stochastic
  games, and the second is a graphical user interface (SGViewer) for
  using and interacting with the solver library. The purpose of this
  guide is to give advanced users an overview of how the library and
  graphical interface are structured. 

  SGSolve makes use of external software packages: the Boost libraries
  are used for serialalization of data relating to stochastic games,
  which are represented with the SGGame class and are saved as .sgm
  files, and the solutions of stochastic games that are generated by
  the algorithmm, which are represented with the SGSolution and
  SGIteration classes and are saved as .sln files. The graphical
  interface uses the Qt framework and the Qt plotting library
  QCustomPlot. Gurobi is used for various extensions using linear
  programming.

  \section installsec Installation

  To use this software, you can directly download the SGViewer
  binaries, which are precompiled for Linux and OS X. The source code
  is also available at www.benjaminbrooks.net/research.shtml. The code
  has most recently been compiled on OS X using LLVM version 10.0.0
  and Boost 1.69. The SGViewer was compiled using Qt 5.4. The class
  SGJYCSolver uses Gurobi for linear programming, and the code was
  compiled with Gurobi 8.00. To compile the code yourself, you need to
  change the relevant variables in localsettings.mk in the root
  directory. You can then build the desired examples by calling "make"
  in either the src or examples directories. After you build the
  source, you can build SGViewer program from the viewer directory by
  first calling "qmake" and then "make".

  \section solversec Overview of the solver library

  The machinery underlying the SGSolve package is a library of
  routines for specifying and solving stochastic games. For examples
  of how to use the library, see risksharing.cpp and
  risksharing.hpp. The main classes are SGGame, which is used to
  specify a game, and the solver class SGSolver_MaxMinMax, whose
  routine SGSolver_MaxMinMax::solve() implements the max-min-max
  algorithm of Abreu, Brooks, and Sannikov (2019). The SGSolver class
  implements the deprecated pencil-sharpening algorithm of Abreu,
  Brooks, and Sannikov (2016).

  An SGGame can be constructed in one of two ways. First, the user can
  specify payoffs and transition probabilities as arrays and pass them
  to the SGGame constructor. Alternatively, the user can create a
  class that derives from SGAbstractGame, and pass an object of the
  derived class to the constructor for SGGame. SGAbstractGame contains
  virtual methods for retrieving the payoffs and transition
  probabilities that can be defined by the user. For an example of how
  to derive from SGAbstractGame, see risksharing.hpp. Once the game is
  constructed, the user can construct an SGSolver for that
  game. SGSolver_MaxMinMax. 

  \section viewersec Overview of the graphical interface

  In addition to using the object model directly, the user can also
  interact with the SGSolve library through the SGViewer graphical
  interface. This interface was constructed using the Qt framework and
  the QCustomPlot plotting library (http://www.qcustomplot.com/). 

  The interface consists of three tabs. The "game tab" is for viewing
  and specifying a stochastic game. It contains tables that display,
  for one state at a time, the players' payoffs and the transition
  probabilities for each pair of actions. The user can edit payoffs,
  probabilities, and the discount factor, as well as add and delete
  actions and states. 

  From the game tab, the user can invoke the solve routine. The
  progress of the algorithm is displayed on the "log tab". 

  Once the algorithm finishes, the output is displayed in a "solution
  tab". There are two tabs, one for the deprecated pencil-sharpening
  algorithm, and the other for the more powerful max-min-max
  algorithm. On the right-hand side of these tabs are a series of
  plots that display payoffs state-by-state. On the left-hand side are
  more detailed plots that decompose how payoffs are generated in one
  of the states.

  Right-clicking on a plot brings up additional options for the
  user. The first option, "inspect point", will show how a given
  payoff is decomposed into flow payoffs and continuation
  utilities. The second option, "simulate", brings up another window
  for forward simulating the equilibrium that generates the given
  payoff.
    
  \section examplesec Examples

  For the benefit of the user, we have included several examples of
  how to use the SGSolve package. The file pd_twostate.cpp is an
  example of a two-state game, where the stage game in each state
  takes the form of a prisoner's dilemma. This file shows how to
  construct an SGGame by specifying the payoffs and transition
  probabilities as arrays.

  The second example is risksharing.cpp, which constructs a risk
  sharing game a la Kocherlakota (1996), in which the two players have
  stochastic endowments and concave utility, and can insure one
  another against income shocks with transfers. The file
  risksharing.hpp constructs a risk sharing game by deriving from the
  SGAbstractGame class, and risksharing.cpp uses that class to solve
  for a variety of parameter values. 

  These examples are reported in Abreu, Brooks, and Sannikov (2019).

  \section conclusionsec Final thoughts

  The package has many more features that the user will hopefully
  discover. Within the src folder is a src/MATLAB subfolder, that
  contains tools for interfacing between MATLAB and SGSolve. In
  particular, sgmex2.cpp is a mex program that can be used to specify,
  solve, and analyze games from within MATLAB. There is also a matlab
  m-file sgmexbuild2.m that was used to build sgmex on OS X with
  MATLAB R2018a.

  This program would not have been possible without the support of
  numerous groups and the contributions of others, and it is entirely
  fitting and appropriate that their contributions should be
  acknowledged. In particular, this program was developed with and
  incorporates elements of a number of other open source projects,
  including the Qt application framework (www.qt.io), Emmanuel
  Eichhammer's QCustomPlot (www.qcustomplot.com), the Boost libraries
  (www.boost.org), and the GNU project (www.gnu.org). I would also
  like to gratefully acknowledge support from the Becker Friedman
  Institute, the University of Chicago, and the National Science
  Foundation.

  Finally, it should go without saying that this program is a work in
  progress. Feedback, bug reports, and contributions are much
  appreciated.

  Enjoy!

  Ben Brooks
  Chicago, IL
  ben@benjaminbrooks.net
*/

/** \example pd_twostate.cpp 
    \example risksharing_maxminmax.cpp
 */
