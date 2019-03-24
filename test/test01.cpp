/* 
 * author: pedro.borges.melo@gmail.com
 * date: March/2018
 * 
 * the model is:
 *
 * min   x_1
 * s.t.  x_1 \in N, x_1 >= 2.2
 *       x_2 \in \arg \min {x_2 s.t. x_2 >= 2*x_1}
 *       x_2 >= 7
 *
 * solution is: x_1 = 4, x_2 = 8
 */

#include <iostream>
#include <functional>
#include <ilcplex/ilocplex.h>

#include "integerBilevelSolver.h"

std::string generateMpsDeltaEmbedding(double delta) {
	// path
	std::string mpsDeltaEmbedding = "prob.mps";
	// create env
	IloEnv env;
	// create model
	IloModel model(env);
	// create vars
	IloNumVar x1(env, 0, IloInfinity, ILOINT);
	IloNumVar x2(env, 0, IloInfinity, ILOINT);
	// add constraints
	model.add(x1 >= 2.2);
	model.add(x2 >= 0);
	model.add(x2 >= 2*x1);
	// add objective
	IloObjective obj = IloMinimize(env , x1 + delta*x2);
	model.add(obj);
	// create cplex model
	IloCplex cplex(model);
	// export model
	cplex.exportModel(mpsDeltaEmbedding.c_str());
	// finish
	model.end();
	cplex.end();
	env.end();
	// return
	return mpsDeltaEmbedding;
}

double myGetOptObjValLowerLevelProb(IloNumArray upperIncumbent) {
	return 2*upperIncumbent[0];
}

double myGetObjValLowerLevelProb(IloNumArray wholeIncumbent) {
	return wholeIncumbent[1];
}

// opt abstract constraints
bool myIsFeasibleForAbstractConstraintOpt(IloNumArray wholeIncumbent) {
	if(wholeIncumbent[1] >= 7.0) {
		return true;
	} else {
		return false;
	}
}

// non opt abstract constraints
bool myIsFeasibleForAbstractConstraintNonOpt(IloNumArray wholeIncumbent) {
	return true;
}

int main(int argc, char* argv[]) {
	// calc delta
	double delta = 0.5;
	// generate delta embedding
	std::cout << "creating delta embedding file..." << std::endl;
	std::string mpsDeltaEmbedding = generateMpsDeltaEmbedding(delta);
	// upper level idx
	std::vector<int> upperLevelIdx;
	upperLevelIdx.push_back(0);
	// lower level idx
	std::vector<int> lowerLevelIdx;
	lowerLevelIdx.push_back(1);
	// opt obj cbk
	std::function<double(IloNumArray)> getOptObjValLowerLevelProb_;
	getOptObjValLowerLevelProb_ = myGetOptObjValLowerLevelProb;
	// obj cbk
	std::function<double(IloNumArray)> getObjValLowerLevelProb_;
	getObjValLowerLevelProb_ = myGetObjValLowerLevelProb;
	// opt abstract constraints
	std::function<bool(IloNumArray)> isFeasibleForAbstractConstraintOpt_;
	isFeasibleForAbstractConstraintOpt_ = myIsFeasibleForAbstractConstraintOpt;
	// non opt abstract constraints
	std::function<bool(IloNumArray)> isFeasibleForAbstractConstraintNonOpt_;
	isFeasibleForAbstractConstraintNonOpt_ = myIsFeasibleForAbstractConstraintNonOpt;
	// create solver instance
	IntegerBilevelSolver integerBilevelSolverInst(
		upperLevelIdx, lowerLevelIdx, mpsDeltaEmbedding,
		getOptObjValLowerLevelProb_, 
		getObjValLowerLevelProb_,
		isFeasibleForAbstractConstraintOpt_,
		isFeasibleForAbstractConstraintNonOpt_);
	// solve
	IloNumArray solFound = integerBilevelSolverInst.solve();
	std::vector<double> solFoundVec = integerBilevelSolverInst.fromIloNumArraytoVector(solFound);
	// print sol
	std::cout << std::endl;
	std::cout << "solFound: " << solFound << std::endl;
	std::cout << std::endl;
	// init sol
	std::vector<double> sol;
	sol.push_back(4.0);
	sol.push_back(8.0);
	// test sol
	integerBilevelSolverInst.checkSol(solFoundVec, sol);
	// return
	return 0;
}
