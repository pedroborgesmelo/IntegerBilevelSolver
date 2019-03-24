/* 
 * author: pedro.borges.melo@gmail.com
 * date: March/2018
 */

#ifndef _INTEGER_BILEVEL_SOLVER_
#define _INTEGER_BILEVEL_SOLVER_

#include <iostream>
#include <functional>
#include <vector>
#include <map>
#include <exception>
#include <ilcplex/ilocplex.h>

class IntegerBilevelSolver {

	private:

		// map to keep opt values of lower level problems
		std::map<std::vector<int>,double> upperLevelIncumbentsMap;
		// path delta embedding mps file
		std::string pathMpsDeltaEmbedding;
		// counters
		int totalMipsSolved;
		int totOptObjQueries;
		// precision
		double precisionToUse;
		// idx for upper and lower level decisions
		std::vector<int> upperLevelIdx;
		std::vector<int> lowerLevelIdx;
		// callbacks
		std::function<double(IloNumArray)> getOptObjValLowerLevelProb;
		std::function<double(IloNumArray)> getObjValLowerLevelProb;
		std::function<bool(IloNumArray)> isFeasibleForAbstractConstraintOpt;
		std::function<bool(IloNumArray)> isFeasibleForAbstractConstraintNonOpt;
	
	public:

		IntegerBilevelSolver(std::vector<int> upperLevelIdx_, std::vector<int> lowerLevelIdx_,
			std::string pathMpsDeltaEmbedding_,
			std::function<double(IloNumArray)> getOptObjValLowerLevelProb_, 
			std::function<double(IloNumArray)> getObjValLowerLevelProb_,
			std::function<bool(IloNumArray)> isFeasibleForAbstractConstraintOpt_,
			std::function<bool(IloNumArray)> isFeasibleForAbstractConstraintNonOpt_);

		IloNumArray solve();
		int getSizeUpperIdx();
		int getSizeLowerIdx();
		int getElementUpperIdx(int i);
		int getElementLowerIdx(int i);
		double getPrecisionToUse();

		double getLowerLevelOptObjVal(std::vector<int> upperLevelIncumbent, IloNumArray wholeIncumbent);
		double callCbkGetOptObjValLowerLevelProb(IloNumArray wholeIncumbentVec);
		double callCbkGetObjValLowerLevelProb(IloNumArray wholeIncumbentVec);
		bool callCbkIsFeasibleForAbstractConstraintOpt(IloNumArray wholeIncumbentVec);
		bool callCbkIsFeasibleForAbstractConstraintNonOpt(IloNumArray wholeIncumbentVec);
		
		std::vector<double> fromIloNumArraytoVector(IloNumArray vec);
		double norm(std::vector<double> vec1, std::vector<double> vec2);
		void checkSol(std::vector<double> solFound, std::vector<double> sol);
};

#endif // _INTEGER_BILEVEL_SOLVER_
