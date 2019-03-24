/* 
 * author: pedro.borges.melo@gmail.com
 * date: March/2018
 */

#include "integerBilevelSolver.h"

IntegerBilevelSolver::IntegerBilevelSolver(
		std::vector<int> upperLevelIdx_, std::vector<int> lowerLevelIdx_,
		std::string pathMpsDeltaEmbedding_,
		std::function<double(IloNumArray)> getOptObjValLowerLevelProb_,
		std::function<double(IloNumArray)> getObjValLowerLevelProb_,
		std::function<bool(IloNumArray)> isFeasibleForAbstractConstraintOpt_,
		std::function<bool(IloNumArray)> isFeasibleForAbstractConstraintNonOpt_) {
	// set path
	this->pathMpsDeltaEmbedding = pathMpsDeltaEmbedding_;
	// set counters
	this->totalMipsSolved = 0;
	this->totOptObjQueries = 0;
	// set precision
	this->precisionToUse = 0.001;
	// set idx
	this->upperLevelIdx = upperLevelIdx_;
	this->lowerLevelIdx = lowerLevelIdx_;
	// callbacks
	this->getOptObjValLowerLevelProb = getOptObjValLowerLevelProb_;
	this->getObjValLowerLevelProb = getObjValLowerLevelProb_;
	this->isFeasibleForAbstractConstraintOpt = isFeasibleForAbstractConstraintOpt_;
	this->isFeasibleForAbstractConstraintNonOpt = isFeasibleForAbstractConstraintNonOpt_;	
}

int IntegerBilevelSolver::getSizeUpperIdx() {
	return this->upperLevelIdx.size();
}

int IntegerBilevelSolver::getSizeLowerIdx() {
	return this->lowerLevelIdx.size();
}

int IntegerBilevelSolver::getElementUpperIdx(int i) {
	return this->upperLevelIdx[i];
}

int IntegerBilevelSolver::getElementLowerIdx(int i) {
	return this->lowerLevelIdx[i];
}

double IntegerBilevelSolver::getPrecisionToUse() {
	return this->precisionToUse;
}

double IntegerBilevelSolver::getLowerLevelOptObjVal(
		std::vector<int> upperLevelIncumbent, IloNumArray wholeIncumbent) {
	// try to find
	std::map<std::vector<int>,double>::iterator it;
	it = this->upperLevelIncumbentsMap.find(upperLevelIncumbent);
	// update counter
	this->totOptObjQueries += 1;
	// get value
	double value = 0.0;
	if (it != this->upperLevelIncumbentsMap.end()) {
		value = it->second;
	} else {
		// determine value for key
		value = this->getOptObjValLowerLevelProb(wholeIncumbent);
		// add key and value
		this->upperLevelIncumbentsMap[upperLevelIncumbent] = value;
		// update counter
		this->totalMipsSolved += 1;
	}
	// return
	return value;
}

double IntegerBilevelSolver::callCbkGetOptObjValLowerLevelProb(IloNumArray wholeIncumbentVec) {
	return this->getOptObjValLowerLevelProb(wholeIncumbentVec);
}

double IntegerBilevelSolver::callCbkGetObjValLowerLevelProb(IloNumArray wholeIncumbentVec) {
	return this->getObjValLowerLevelProb(wholeIncumbentVec);
}

bool IntegerBilevelSolver::callCbkIsFeasibleForAbstractConstraintOpt(IloNumArray wholeIncumbentVec) {
	return this->isFeasibleForAbstractConstraintOpt(wholeIncumbentVec);
}

bool IntegerBilevelSolver::callCbkIsFeasibleForAbstractConstraintNonOpt(IloNumArray wholeIncumbentVec) {
	return this->isFeasibleForAbstractConstraintNonOpt(wholeIncumbentVec);
}

std::vector<double> IntegerBilevelSolver::fromIloNumArraytoVector(IloNumArray vec) {
	// init
	std::vector<double> out;
	// copy
	for(int i=0;i<vec.getSize();++i) {
		out.push_back(vec[i]);
	}	
	// return
	return out;
}

double IntegerBilevelSolver::norm(std::vector<double> vec1, std::vector<double> vec2) {
	// init
	int size = vec1.size();
	double norm = 0.0;
	// get norm squared
	for(int i=0;i < size; ++i) {
		norm += (vec1[i]-vec2[i])*(vec1[i]-vec2[i]);
	}
	// sqrt
	norm = sqrt(norm);
	// return
	return norm;
}

void IntegerBilevelSolver::checkSol(std::vector<double> solFound, std::vector<double> sol) {
	// norm
	double norm = this->norm(solFound, sol);
	// test
	if(norm > 0.0001) {
		std::cout << std::endl;
		std::cout << "WRONG SOLUTION." << std::endl;
		std::cout << std::endl;
		throw std::exception();
	}
}

ILOINCUMBENTCALLBACK2(myIncumbentCbk,
		IntegerBilevelSolver*, myIntegerBilevelSolver,
		IloNumVarArray, myVars) {
	// get incumbent val
	IloNumArray wholeIncumbent(getEnv());
	getValues(wholeIncumbent, myVars);
	// get upper level incumbent and round it
	int sizeUpperIdx = myIntegerBilevelSolver->getSizeUpperIdx();
	std::vector<int> upperIncumbentRounded;
	IloNumArray upperIncumbent(getEnv());
	for(int i=0;i<sizeUpperIdx;++i) {
		int pos = myIntegerBilevelSolver->getElementUpperIdx(i);
		int roundedVal = round(wholeIncumbent[pos]);
		upperIncumbent.add(roundedVal);
		upperIncumbentRounded.push_back(roundedVal);
	}
	// get opt value of lower level problem
	double valToCompare = myIntegerBilevelSolver->getLowerLevelOptObjVal(
		upperIncumbentRounded, upperIncumbent);
	// get non opt value obj lower level
	double objValSubProb = myIntegerBilevelSolver->callCbkGetObjValLowerLevelProb(wholeIncumbent);
	// pre calcs
	double precision = myIntegerBilevelSolver->getPrecisionToUse();
	double diff = fabs(objValSubProb-valToCompare);
	bool isFeasOptAbsConstr = myIntegerBilevelSolver->callCbkIsFeasibleForAbstractConstraintOpt(wholeIncumbent);
	bool isFeasNonOptAbsConstr = myIntegerBilevelSolver->callCbkIsFeasibleForAbstractConstraintNonOpt(wholeIncumbent);
	// decide if rejects
	if(diff > precision || isFeasOptAbsConstr == false) {
		reject();
	} else if(isFeasNonOptAbsConstr == false) {
		reject();
	}
	// finish
	wholeIncumbent.end();
	upperIncumbent.end();
}

IloNumArray IntegerBilevelSolver::solve() {
	// init
	IloEnv env;
	IloModel model(env);
	IloCplex cplex(env);
	IloObjective obj;
	IloNumVarArray var(env);
	IloRangeArray rng(env);
	IloSOS1Array sos1(env);
	IloSOS2Array sos2(env);
	IloRangeArray lazy(env);
	IloRangeArray cuts(env);
	// import
	std::string pathMps = this->pathMpsDeltaEmbedding;
	cplex.importModel(model, pathMps.c_str(), 
		obj, var, rng, sos1, 
		sos2, lazy, cuts);
	// extract
	cplex.extract(model);
	// set params
	cplex.setParam(IloCplex::Param::Preprocessing::Reduce, 0);
	// add incumbent callback
	cplex.use(myIncumbentCbk(env, this, var));
	// solve
	cplex.solve();
	// get sol	
	IloNumArray sol(env);
	cplex.getValues(sol, var);
	// finish
	cuts.end();
	lazy.end();
	sos2.end();
	sos1.end();
	rng.end();
	var.end();
	model.end();
	cplex.end();
	env.end();
	// return
	return sol;
}
