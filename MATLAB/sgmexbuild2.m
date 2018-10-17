% This file is part of the SGSolve library for stochastic games
% Copyright (C) 2016 Benjamin A. Brooks
% 
% SGSolve free software: you can redistribute it and/or modify it
% under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
% 
% SGSolve is distributed in the hope that it will be useful, but
% WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
% General Public License for more details.
% 
% You should have received a copy of the GNU General Public License
% along with this program.  If not, see
% <http://www.gnu.org/licenses/>.

% File for building the sgmex executable
function sgmexbuild2

if strcmp(computer,'PCWIN')
    boostlibdir = '-L./libs/w32';
    boostincldir = '-I..\..\..\boost\boost_1_49_0';
    compilerflags = 'COMPFLAGS=$COMPFLAGS /MT';
    boostlib = '-lboost_serialization-vc100-mt-s-1_49';
elseif strcmp(computer,'PCWIN64')
    boostlibdir = '-L./libs/w64';
    boostincldir = '-I..\..\..\boost\boost_1_49_0';
    compilerflags = 'COMPFLAGS=$COMPFLAGS /MT';
    boostlib = '-lboost_serialization-vc100-mt-s-1_49';
elseif strcmp(computer,'GLNXA64')
    boostlibdir = '-L.';
    boostincldir = '-I/usr/local/include/boost';
    compilerflags = 'CXXFLAGS="\$CXXFLAGS -std=c++11"';
    LDFLAGS = 'LDFLAGS="\$LDFLAGS"';
    MATLABINCL = '-I/usr/local/MATLAB/R2014a/extern/include';
    % boostlib = '/tmp/boost_1_51_0/stage/lib/libboost_serialization.a';
    boostlib = [];
elseif strcmp(computer,'MACI64')
    boostlibdir = '-L.';
    boostincldir = '-I/usr/local/include/';
    compilerflags = 'CFLAGS="\$CFLAGS"';
    LDFLAGS = 'LDFLAGS="\$LDFLAGS"';
    MATLABINCL = '-I/Applications/MATLAB_R2018b.app/sys/extern/maci64/clang/lib/clang/3.2/include';
    % boostlib = '/tmp/boost_1_51_0/stage/lib/libboost_serialization.a';
    boostlib = [];
end

mex(compilerflags,...
	'-I../src/hpp','-I../src/cpp','-v',boostincldir,...
    boostlibdir, ...
    'sgmex2.cpp','../lib/libsg.a',...
    '/usr/local/lib/libboost_serialization.a', ...
	'-g',...
    LDFLAGS);
end % sgmexbuild
