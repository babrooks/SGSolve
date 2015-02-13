#ifndef SGSOLVERWORKER_HPP
#define SGSOLVERWORKER_HPP

#include <QtWidgets>
#include "sg.hpp"
#include "sgapprox.hpp"

using namespace std;

class SGSolverWorker : public QObject
{
  Q_OBJECT;

private:
  SGEnv env;
  SGSolution soln;
  SGApprox approx;
  QTextEdit * logTextEdit;

public:
  enum STATUS
    {
      FAILED,
      CONVERGED,
      NOTCONVERGED
    } status;

  SGSolverWorker(const SGGame & game,
		 QTextEdit * _logTextEdit):
    soln(game), approx(env,game,soln),
    logTextEdit(_logTextEdit)
  {
    env.setParam(SGEnv::PRINTTOCOUT,false);

    approx.initialize();
  }

  STATUS getStatus() const { return status; }

public slots:

  void iterate()
  {
    try
      {
	if (approx.generate() > env.errorTol
	    && approx.numIterations < env.maxIterations)
	  {
	    status = NOTCONVERGED;
	    emit resultReady(false);
	  }
	else
	  {
    
	    // Add the extreme tuples array to soln.
	    for (vector<SGTuple>::const_iterator tuple
		   = approx.extremeTuples.begin();
		 tuple != approx.extremeTuples.end();
		 ++tuple)
	      soln.push_back(*tuple);

	    approx.end();
	    
	    status = CONVERGED;
	    emit resultReady(true);
	  }
      }
    catch (exception & e)
      {
	// Add the extreme tuples array to soln.
	qDebug() << "solve failed" << endl;

	for (vector<SGTuple>::const_iterator tuple
	       = approx.extremeTuples.begin();
	     tuple != approx.extremeTuples.end();
	     ++tuple)
	  soln.push_back(*tuple);

	approx.end();
	
	status = FAILED;
	emit resultReady(true);
	
	// emit exceptionCaught();
      }
  }

  const SGSolution & getSolution() const
  { return soln; }

  const SGApprox & getApprox() const
  { return approx; }
  
signals:
  void resultReady(bool);
  void iterationFinished();
  void exceptionCaught();  
};

#endif
