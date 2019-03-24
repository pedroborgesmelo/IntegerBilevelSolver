/* 
 * author: pedro.borges.melo@gmail.com
 * date: March/2018
 * 
 * the model is:
 *
 * min   \sum_i x_i
 * s.t.  x_i \in N, x_i >= a_i
 *       y_i \in \arg \min {y_i s.t. y_i >= 2*x_i}
 *       y_i >= b_i
 * 
 * for a_i and b_i selected randomly
 */

#include <iostream>
#include <functional>
#include <ilcplex/ilocplex.h>

#include "integerBilevelSolver.h"

std::vector<double> aVec;
std::vector<double> bVec;

#define MAX_VAL 30

std::string generateMpsDeltaEmbedding(double delta) {
	// get prob size
	int probSize = aVec.size();
	// path
	std::string mpsDeltaEmbedding = "prob.mps";
	// create env
	IloEnv env;
	// create model
	IloModel model(env);
	// create vars
	IloNumVarArray x(env, probSize, 0, MAX_VAL, ILOINT);
	IloNumVarArray y(env, probSize, 0, MAX_VAL, ILOINT);
	// add constraints
	for(int i=0;i<probSize; ++i) {
		model.add(x[i] >= aVec[i]);
		model.add(y[i] >= 0);
		model.add(y[i] >= 2*x[i]);
	}
	// add objective
	IloExpr objExpr(env);
	for(int i=0; i<probSize; ++i) {
		objExpr += x[i] + delta*y[i];
	}
	IloObjective obj = IloMinimize(env, objExpr);
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
	int probSize = upperIncumbent.getSize();

/*
std::cout << std::endl;
std::cout << "upperIncumbent: " << upperIncumbent << std::endl;
std::cout << std::endl;
*/

	double optObj = 0.0;
	for(int i=0;i<probSize; ++i) {
		optObj += 2*upperIncumbent[i];
	}
	// return
	return optObj;
}

double myGetObjValLowerLevelProb(IloNumArray wholeIncumbent) {

/*
std::cout << std::endl;
std::cout << "wholeIncumbent: " << wholeIncumbent << std::endl;
std::cout << std::endl;
exit(0);
*/

	int probSize = wholeIncumbent.getSize()/2;
	double obj = 0.0;
	for(int i=0;i<probSize; ++i) {
		obj += wholeIncumbent[2*i + 1];
	}
	// return
	return obj;
}

// opt abstract constraints
bool myIsFeasibleForAbstractConstraintOpt(IloNumArray wholeIncumbent) {
	int probSize = wholeIncumbent.getSize()/2;
	for(int i=0;i<probSize; ++i) {
		if(wholeIncumbent[2*i + 1] < bVec[i]) {
			return false;
		}
	}
	// return
	return true;
}

// non opt abstract constraints
bool myIsFeasibleForAbstractConstraintNonOpt(IloNumArray wholeIncumbent) {
	return true;
}

void solveStackedProblem(int problemSize) {
	// seed
	srand(time(NULL));
	// sample a_i and b_i
	for(int i=0; i<problemSize; ++i) {
		double aTmp = rand() % 3;
		double bTmp = rand() % 6;
		// push back
		aVec.push_back(aTmp);
		bVec.push_back(bTmp);
	}
	// calc delta
	double delta = 0.5;
	// generate delta embedding
	std::cout << "creating delta embedding file..." << std::endl;
	std::string mpsDeltaEmbedding = generateMpsDeltaEmbedding(delta);
	// level idx
	std::vector<int> upperLevelIdx;
	std::vector<int> lowerLevelIdx;
	for(int i=0; i<problemSize; ++i) {
		upperLevelIdx.push_back(2*i);
		lowerLevelIdx.push_back(2*i+1);
	}
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
	// compute sol manually
	std::vector<double> sol;
	for(int i=0; i<problemSize; ++i) {
		// pre calc
		int aVecTmp = ceil(aVec[i]);
		int bVecTmp = ceil(bVec[i]);
		// gen sol
		if(2*aVecTmp >= bVecTmp) {
			sol.push_back(aVecTmp);
			sol.push_back(2*aVecTmp);	
		} else if(bVecTmp % 2 == 0){
			sol.push_back(bVecTmp/2);
			sol.push_back(bVecTmp);
		} else {
			bVecTmp += 1;
			sol.push_back(bVecTmp/2);
			sol.push_back(bVecTmp);			
		}
	}
	// test sol, solution is unique
	for(int i=0; i<2*problemSize; ++i) {
		if(fabs(solFoundVec[i] - sol[i]) >= 0.0001) {
			// error output
			std::cout << std::endl;
			std::cout << "WRONG SOLUTION." << std::endl;
			std::cout << std::endl;
			throw std::exception();
		}
	}
	// final output
	std::cout << std::endl;
	std::cout << "CORRECT SOLUTION." << std::endl;
	std::cout << std::endl;
}

int main(int argc, char* argv[]) {
	int problemSize = 10;
	// solve problem size
	solveStackedProblem(problemSize);
	// return
	return 0;
}
