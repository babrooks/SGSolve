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

//! One state prisoner's dilemma
//! @example

#include "sg.hpp"
#include "sgsolver_maxminmax_3player.hpp"

int main ()
{
  SGEnv env;
  env.setParam(SG::DIRECTIONTOL,1e-12); 
  env.setParam(SG::NORMTOL,1e-12); 
  env.setParam(SG::LEVELTOL,1e-12); 
  env.setParam(SG::IMPROVETOL,1e-13); 
  env.setParam(SG::MAXITERATIONS,20); 

  env.setParam(SG::ICTOL,1e-9);

  cout << "Setting up action..." << endl;
  
  SGAction_MaxMinMax action (env,3,0,0);
  action.setMinICPayoffs(SGPoint(3,0.0));
  action.resetTrimmedPoints(SGPoint(3,1.0));

  cout << "Performing intersections..." << endl;
  
  SGPoint normal(3,-1.0);
  action.trim(normal,-0.5);

  normal[1] = -0.5; normal[2]=-2;
  action.trim(normal,-0.5);
  normal[1] = -2; normal[2]=-0.5;
  action.trim(normal,-0.5);

  // normal[0]=-0.5; normal[1]=-3;
  // action.trim(normal,-0.5);

  // normal[0]=-0.25; normal[1]=-2;
  // action.trim(normal,-0.5);

  normal[0] = 0.5; normal[1] = -1; normal[2] = 1;
  action.trim(normal,1);

  normal[0] = 0.1; normal[1] = -1; normal[2] = 0.1;
  action.trim(normal,0.1);

  normal[0] = 0.4; normal[1] = -1; normal[2] = 0.6;
  action.trim(normal,normal[2]);

  normal[0] = 0.3; normal[1] = -1; normal[2] = 0.35;
  action.trim(normal,normal[2]);
  

  normal = SGPoint (3,1.0);
  action.trim(normal,1.75);



  cout << "Printing data... " << endl;
  
  const vector<SGTuple> & points = action.getTrimmedPoints();
  const vector<SGTuple> & dirs = action.getTrimmedBndryDirs();

  ofstream ofs("threeplayertest.log",std::ofstream::out);

  assert(points.size() == dirs.size());
  for (int pp = 0; pp < 3; pp++)
    {
      for (int k = 0; k < points[pp].size(); k++)
	{
	  for (int p = 0; p < 3; p++)
	    {
	      assert(!isnan(points[pp][k][p]));
	      ofs << points[pp][k][p] << " ";
	    }
	  for (int p = 0; p < 3; p++)
	    ofs << dirs[pp][k][p] << " ";
	  ofs << endl;
	}
      ofs << 99 << endl;
    }
  ofs.close();
  
  
  cout << "Done!" << endl;
  
  
  return 0;
}
