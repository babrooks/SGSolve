function sgmexbuild

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
    compilerflags = 'CFLAGS="\$CFLAGS"';
    LDFLAGS = 'LDFLAGS="\$LDFLAGS"';
    MATLABINCL = '-I/usr/local/MATLAB/R2014a/extern/include';
    % boostlib = '/tmp/boost_1_51_0/stage/lib/libboost_serialization.a';
    boostlib = [];
elseif strcmp(computer,'MACI64')
    boostlibdir = '-L.';
    boostincldir = '-I/usr/local/include/';
    compilerflags = 'CFLAGS="\$CFLAGS"';
    LDFLAGS = 'LDFLAGS="\$LDFLAGS"';
    MATLABINCL = '-I/Applications/MATLAB_R2014b.app/sys/extern/maci64/clang/lib/clang/3.2/include';
    % boostlib = '/tmp/boost_1_51_0/stage/lib/libboost_serialization.a';
    boostlib = [];
end

mex('-I../hpp','-I../cpp','-v',boostincldir,...
    boostlibdir, ...
    'sgmex.cpp','../../lib/libsg.a',...
    '/usr/local/lib/libboost_serialization.a', ...
	'-g',...
    compilerflags,...
    LDFLAGS);
end % sgmexbuild