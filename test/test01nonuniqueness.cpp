/* 
 * author: pedro.borges.melo@gmail.com
 * date: March/2018
 * 
 * the model is:
 *
 * min   x_1
 * s.t.  x_1 \in N, x_1 >= 2.2
 *       x_2 \in \arg \min {0 s.t. x_2 <= 10}
 *       x_2 >= 5
 * 
 * solution is: x_1 = 3, x_2 \in [5, 10]
 */

#include <iostream>
#include <functional>
#include <ilcplex/ilocplex.h>
#include <exception>

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
	model.add(x2 <= 10.0);
	// add objective
	IloObjective obj = IloMinimize(env , x1);
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
	return 0.0;
}

double myGetObjValLowerLevelProb(IloNumArray wholeIncumbent) {
	return 0.0;
}

// opt abstract constraints
bool myIsFeasibleForAbstractConstraintOpt(IloNumArray wholeIncumbent) {
	return wholeIncumbent[1] >= 5.0;
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
	// test sol
	if (fabs(solFoundVec[0] - 3.0) <= 0.001 && solFoundVec[1] >= 5.0 && solFoundVec[1] <= 10.0) {
		std::cout << std::endl;
		std::cout << "CORRECT SOLUTION." << std::endl;
		std::cout << std::endl;
	} else {
		std::cout << std::endl;
		std::cout << "WRONG SOLUTION." << std::endl;
		std::cout << std::endl;
		throw std::exception();
	}
	// return
	return 0;
}
