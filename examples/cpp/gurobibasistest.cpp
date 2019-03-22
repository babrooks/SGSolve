#include "gurobi_c++.h"
#include <iostream>

using namespace std;

int main ()
{

  try
    {
      GRBEnv env;
      GRBModel model(env);

      GRBVar * x = model.addVars(2);
      model.update();

      GRBConstr c0 = model.addConstr(x[0]+2*x[1]<=1);
      GRBConstr c1 = model.addConstr(x[1]+2*x[0]<=1);

      GRBLinExpr obj = x[0]+x[1];
      model.setObjective(obj,GRB_MAXIMIZE);
      model.update();


      x[0].set(GRB_IntAttr_VBasis,-1);
      x[1].set(GRB_IntAttr_VBasis,-1);
      c0.set(GRB_IntAttr_CBasis,0);
      c1.set(GRB_IntAttr_CBasis,0);

      model.update();

      model.getEnv().set(GRB_DoubleParam_IterationLimit,0);
      
      model.optimize();
      
      cout << "Obj value: " << obj.getValue() << endl;
      
      x[1].set(GRB_IntAttr_VBasis,-1);
      c1.set(GRB_IntAttr_CBasis,0);
      model.optimize();
      cout << "Obj value: " << obj.getValue() << endl;
      
      // // x[3].set(GRB_IntAttr_VBasis,0);
      // // cout << "Obj value: " << obj.getValue() << endl;

      model.optimize();
      cout << x[0].get(GRB_IntAttr_VBasis) << endl
	   << x[1].get(GRB_IntAttr_VBasis) << endl;
      cout << c0.get(GRB_IntAttr_CBasis) << endl
	   << c1.get(GRB_IntAttr_CBasis) << endl;
      cout << x[0].get(GRB_DoubleAttr_X) << endl
	   << x[1].get(GRB_DoubleAttr_X) << endl;
      delete[] x;
    }
  catch (GRBException &e )
    {
      cout << "An error occurred:" << endl;
      cout << e.getMessage() << endl;
    }
  
  return 0;
}
