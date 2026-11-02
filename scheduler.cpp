// std includes
#include <iostream>

// CP-SAT includes
#include "absl/base/log_severity.h"
#include "absl/log/globals.h"
#include "absl/strings/str_format.h"
#include "ortools/base/init_google.h"
#include "ortools/base/logging.h"
#include "ortools/sat/cp_model.h"
#include "ortools/sat/cp_model.pb.h"
#include "ortools/sat/cp_model_solver.h"
#include "ortools/sat/model.h"
#include "ortools/sat/sat_parameters.pb.h"
#include "ortools/util/time_limit.h"

using namespace operations_research;
using namespace sat;
using namespace std;
struct Course
{
	int courseNumber;
	int durationInMinutes;
};

void runCPSAT(int numFaculty, vector<Course>& courses, int timeSlots) // would also need days yes? Every day has x time slots... 
{
	vector<int> allFaculty(numFaculty);
	for(int i{0}; i < allFaculty.size(); i++)
	{
		allFaculty[i] = i;
	}

	CpModelBuilder model;

}

int main()
{
	int numFaculty = 10;
	int numSlots = 5;
	vector<Course> courses = { {1,50}, {2, 50}, {3, 50}, {4,50}, {5, 50} };
	runCPSAT(numFaculty, courses, numSlots);
}

/* Prototype simplifications: 
 For 1 semester
 Courses can be on any days
 are we provided if the days in which a class occurs
 uniform class durations (no MWF vs TuTh distinction)
 Fixed amount of classes per day.
 faculty can teach 1 course per day.
 No class level based constraints on time slot assignment
 */
