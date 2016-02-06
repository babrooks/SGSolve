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

// Definitions for doxygen to make relation graphs
namespace std
{
  class vector<SGPoint> { public: SGPoint element; };
  class vector<vector<SGPoint> > { public: vector<SGPoint> element; };
  class vector<SGTuple> { public: SGTuple element; };
  class vector<SGAction*> { public: SGAction* element; };
  class vector<const SGAction*> { public: const SGAction* element; };
  class vector<list<SGAction> > { public: list<SGAction> element; };
  class list<SGAction> { public: SGAction element; };
  class list<SGTuple> { public: SGTuple element; };
  class list<SGIteration> { public: SGIteration element; };
  // class vector<SGTuple> { public: SGTuple element; };
}

