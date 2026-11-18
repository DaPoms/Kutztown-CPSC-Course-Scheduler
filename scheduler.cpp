// std includes
#include <iostream>
#include <utility>
#include <map>
#include <tuple>

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
};

struct Faculty 
{
	string name;
};

struct TimeSlot
{
	pair<int, int> timeRange;
};

struct Day 
{
	string name;
	vector<TimeSlot> availableSlots;
};

void runCPSAT(vector<Course>& courses, vector<Faculty>& faculty, vector<Day>& days) // would also need days yes? Every day has x time slots... 
{
	CpModelBuilder model;
	map<tuple<int, int, int, int>, BoolVar> possibleAssignments;
	for(int f{0}; f < faculty.size(); f++)
	{
		for(int c{0}; c < courses.size(); c++)
		{
			for(int d{0}; d < days.size(); d++)
			{
				for(int s{0}; s < days[d].availableSlots.size(); s++)
				{
					tuple<int, int, int, int> key = {f,c,d,s};
					possibleAssignments[key] = model.NewBoolVar().WithName(absl::StrFormat("assignment %d%d%d%d", f,c,d,s));
				}
			}
		}
	}

	// A course can only be taught by 1 professor at a given time (not allowed multiple professors teaching a course)
	for(int d{0}; d < days.size(); d++)
	{
		for(int s{0}; s < days[d].availableSlots.size(); s++)
		{
			for(int c{0}; c < courses.size(); c++)
			{
				vector<BoolVar> AllProfsForACourse; // On a given day and time
				for(int f{0}; f < faculty.size(); f++) 
				{
					tuple<int, int, int, int> key = {f,c,d,s};
					AllProfsForACourse.push_back(possibleAssignments[key]);
				}
				model.AddAtMostOne(AllProfsForACourse); // TO understand what this constraint actually entails, 
				//you must walk though the nested for loops. For each day, we go through all possible time slots and courses. For each course, we get all decision vals of each faculty member for that given course, and state that for that course at the assigned day and time slot, only 1 faculty member can be assigned to it. Thus this constraint is that only 1 professor is involved with teaching a class.
			}
		}
	}

	// Professors teach at most 1 class on a given day, which is actually 2-3, but I simplified the problem by stating if assigned to Monday, then it also is Wednesday Friday, and If Tuesday, then also Thursday 
	for(int f{0}; f < faculty.size(); f++)
	{
		for(int d{0}; d < days.size(); d++)
		{
			vector<BoolVar> courseForAProfOnDay; // all variables associated to the same course for a given prof
			for(int c{0}; c < courses.size(); c++)
			{
				for(int s{0}; s < days[d].availableSlots.size(); s++)
				{
					tuple<int, int, int, int> key = {f,c,d,s};
					courseForAProfOnDay.push_back(possibleAssignments[key]);
				}
			}
			model.AddAtMostOne(courseForAProfOnDay); // will be at most 4 (with some exceptions) in the real implementation. Lookup linear sum for how to do this later.
		}
	}

	

	// TODO constraint: All classes must be assigned EXACTLY once (once again, if on Monday, then MWF, otherwise TuesThurs)

	


	// running solver
	const CpSolverResponse result = Solve(model.Build()); // Theres also a Solve with param function for when we get to it (for adding time limits and various settings)
	if(result.status() == CpSolverStatus::FEASIBLE || result.status() == CpSolverStatus::OPTIMAL) // Optimal is for when/if we add an objective (something to maximize/minimize, such as deviation from the original schedule with schedule updating)
	{
		// Extract the solution results here
	}
	else
	{
		cout << "PLACEHOLDER: This was an invalid problem, or a solution simply was not found in the available time";
	}
}

int main()
{
	vector<Course> courses = { {1}, {2}, {3}, {4}, {5} };
	vector<Faculty> faculty = {{"Mr.Bob"}, {"Dr.Tim"}, {""} };
	vector<TimeSlot> mondayAvailability{{{800,850}}, {{900,950}}, {{1000,1050}}, {{1100, 1150}}, {{1200,1250}}, {{100,150}}, {{200,250}}, {{300,350}}, {{400,450}}};
	vector<TimeSlot> tuesdayAvailability{ {{800,915}}, {{930, 1045}}, {{1200,115}}, {{130, 245}}, {{300, 415}}};
	vector<Day> days = { {"Monday Wednesday Friday", {mondayAvailability} }, {"Tuesday Thursday", {mondayAvailability} } };
	runCPSAT(courses, faculty, days);
}


// Tuple of: course, faculty, day, time slot,  (A decision tuple)

/* Prototype simplifications: 
 For 1 semester
 Courses can be on any days (M / T)
 //are we provided if the days in which a class occurs
 uniform class durations (no MWF vs TuTh distinction)
 faculty can teach 1 course per day.
 No class level based constraints on time slot assignment (every class is undergraduate)
 Courses are given either on MWF or Tuesday Thursday
 rooms are not considered


 // I
 */

 //15 min break for tuesday
//11-12 on tuesdays


// Issue: If solving whole problem as separate semesters, room numbers would be an issue to account for
// Solution: Solve as two separate semester back to back via warm start (reducing required courses by what was used up)

// In our actual development version, days should be SEPARATE (mon, tues, wed....). Currently it is connected
