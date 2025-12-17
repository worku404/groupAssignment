#include <iostream>
#include <iomanip>
#include <limits>
#include <cstring>

using std::cin;
using std::cout;
using std::endl;

constexpr int MAX_STUDENTS = 50;
constexpr int MAX_TESTS    = 8;
constexpr int ID_LEN       = 16;
constexpr int NAME_LEN     = 32;

static void clearBadInput()
{
    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int readInt(const char* prompt)
{
    int x;
    while (true)
    {
        cout << prompt;
        if (cin >> x) return x;
        clearBadInput();
        cout << "Invalid number. Try again.\n";
    }
}

static int readIntInRange(const char* prompt, int minV, int maxV)
{
    while (true)
    {
        int x = readInt(prompt);
        if (x >= minV && x <= maxV) return x;
        cout << "Value must be in [" << minV << ", " << maxV << "]. Try again.\n";
    }
}

static void readToken(char* out, int outCap, const char* prompt)
{
    while (true)
    {
        cout << prompt;
        cin >> std::setw(outCap) >> out; // reads a single token (no spaces)
        if (cin && std::strlen(out) > 0) return;
        clearBadInput();
        cout << "Invalid input. Try again.\n";
    }
}

static int findStudentById(const char ids[][ID_LEN], int studentCount, const char* id)
{
    for (int i = 0; i < studentCount; ++i)
    {
        if (std::strcmp(ids[i], id) == 0) return i;
    }
    return -1;
}

// Pointer-based row traversal (this is the same memory as marks[row][0..tests-1])
static int sumRow(const int* row, int tests)
{
    int total = 0;
    const int* p = row;
    for (int i = 0; i < tests; ++i)
        total += *(p + i);
    return total;
}

static int minRow(const int* row, int tests)
{
    const int* p = row;
    int mn = *p;
    for (int i = 1; i < tests; ++i)
        if (*(p + i) < mn) mn = *(p + i);
    return mn;
}

static int maxRow(const int* row, int tests)
{
    const int* p = row;
    int mx = *p;
    for (int i = 1; i < tests; ++i)
        if (*(p + i) > mx) mx = *(p + i);
    return mx;
}

static double averageRow(const int* row, int tests)
{
    return tests > 0 ? (double)sumRow(row, tests) / (double)tests : 0.0;
}

static char letterGrade(double avg)
{
    if (avg >= 90.0) return 'A';
    if (avg >= 80.0) return 'B';
    if (avg >= 70.0) return 'C';
    if (avg >= 60.0) return 'D';
    if (avg >= 50.0) return 'E'; // pass threshold example
    return 'F';
}

static void printStudentReport(
    const char ids[][ID_LEN],
    const char names[][NAME_LEN],
    const int marks[][MAX_TESTS],
    int studentCount,
    int testCount)
{
    char id[ID_LEN]{};
    readToken(id, ID_LEN, "Enter student ID: ");

    int idx = findStudentById(ids, studentCount, id);
    if (idx < 0)
    {
        cout << "Student not found.\n";
        return;
    }

    const int* row = marks[idx]; // decays to pointer to first element in that row
    int total = sumRow(row, testCount);
    double avg = averageRow(row, testCount);

    cout << "\n--- Student Report ---\n";
    cout << "ID   : " << ids[idx] << "\n";
    cout << "Name : " << names[idx] << "\n";
    cout << "Marks: ";
    for (int t = 0; t < testCount; ++t) cout << marks[idx][t] << (t + 1 == testCount ? "" : ", ");
    cout << "\nTotal: " << total << "\n";
    cout << "Avg  : " << std::fixed << std::setprecision(2) << avg << "\n";
    cout << "Min  : " << minRow(row, testCount) << "\n";
    cout << "Max  : " << maxRow(row, testCount) << "\n";
    cout << "Grade: " << letterGrade(avg) << "\n";
    cout << "Status: " << (avg >= 50.0 ? "PASS" : "FAIL") << "\n\n";
}

static void listStudents(
    const char ids[][ID_LEN],
    const char names[][NAME_LEN],
    const int marks[][MAX_TESTS],
    int studentCount,
    int testCount)
{
    if (studentCount == 0)
    {
        cout << "No students yet.\n";
        return;
    }

    cout << "\n--- Student List ---\n";
    cout << std::left << std::setw(16) << "ID"
         << std::setw(24) << "Name"
         << std::right << std::setw(10) << "Average"
         << std::setw(8) << "Grade"
         << "\n";

    cout << std::string(58, '-') << "\n";

    for (int i = 0; i < studentCount; ++i)
    {
        double avg = averageRow(marks[i], testCount);
        cout << std::left << std::setw(16) << ids[i]
             << std::setw(24) << names[i]
             << std::right << std::setw(10) << std::fixed << std::setprecision(2) << avg
             << std::setw(8) << letterGrade(avg)
             << "\n";
    }
    cout << "\n";
}

static void addStudent(
    char ids[][ID_LEN],
    char names[][NAME_LEN],
    int marks[][MAX_TESTS],
    int& studentCount,
    int testCount)
{
    if (studentCount >= MAX_STUDENTS)
    {
        cout << "Class is full (MAX_STUDENTS reached).\n";
        return;
    }

    char id[ID_LEN]{};
    char name[NAME_LEN]{};

    readToken(id, ID_LEN, "New student ID (no spaces): ");
    if (findStudentById(ids, studentCount, id) >= 0)
    {
        cout << "That ID already exists.\n";
        return;
    }

    readToken(name, NAME_LEN, "Student name (no spaces): ");

    // Copy into fixed arrays
    std::strncpy(ids[studentCount], id, ID_LEN - 1);
    std::strncpy(names[studentCount], name, NAME_LEN - 1);

    cout << "Enter marks for " << testCount << " test(s), each 0..100.\n";
    for (int t = 0; t < testCount; ++t)
    {
        marks[studentCount][t] = readIntInRange("  Mark: ", 0, 100);
    }

    ++studentCount;
    cout << "Student added.\n";
}

static void updateMarks(
    const char ids[][ID_LEN],
    const char names[][NAME_LEN],
    int marks[][MAX_TESTS],
    int studentCount,
    int testCount)
{
    char id[ID_LEN]{};
    readToken(id, ID_LEN, "Enter student ID: ");

    int idx = findStudentById(ids, studentCount, id);
    if (idx < 0)
    {
        cout << "Student not found.\n";
        return;
    }

    cout << "Updating marks for: " << names[idx] << " (" << ids[idx] << ")\n";
    cout << "Enter which test to update (1.." << testCount << "): ";
    int testNo = readIntInRange("", 1, testCount);
    int newMark = readIntInRange("New mark (0..100): ", 0, 100);

    marks[idx][testNo - 1] = newMark;
    cout << "Updated.\n";
}

static void printClassSummaryAndRanking(
    const char ids[][ID_LEN],
    const char names[][NAME_LEN],
    const int marks[][MAX_TESTS],
    int studentCount,
    int testCount)
{
    if (studentCount == 0)
    {
        cout << "No students yet.\n";
        return;
    }

    // Build an index list so we can "sort" without moving the real arrays.
    int order[MAX_STUDENTS];
    for (int i = 0; i < studentCount; ++i) order[i] = i;

    // Selection sort by average descending (simple, clear, uses loops/conditions)
    for (int i = 0; i < studentCount - 1; ++i)
    {
        int best = i;
        for (int j = i + 1; j < studentCount; ++j)
        {
            double aBest = averageRow(marks[order[best]], testCount);
            double aJ    = averageRow(marks[order[j]], testCount);
            if (aJ > aBest) best = j;
        }
        if (best != i)
        {
            int tmp = order[i];
            order[i] = order[best];
            order[best] = tmp;
        }
    }

    // Class stats
    double classSum = 0.0;
    double bestAvg = -1.0, worstAvg = 101.0;
    int passCount = 0;

    for (int i = 0; i < studentCount; ++i)
    {
        double avg = averageRow(marks[i], testCount);
        classSum += avg;
        if (avg > bestAvg) bestAvg = avg;
        if (avg < worstAvg) worstAvg = avg;
        if (avg >= 50.0) ++passCount;
    }

    cout << "\n--- Class Summary ---\n";
    cout << "Students : " << studentCount << "\n";
    cout << "Tests    : " << testCount << "\n";
    cout << "Class Avg: " << std::fixed << std::setprecision(2) << (classSum / studentCount) << "\n";
    cout << "Best Avg : " << bestAvg << "\n";
    cout << "Worst Avg: " << worstAvg << "\n";
    cout << "Pass Rate: " << std::fixed << std::setprecision(2)
         << (100.0 * passCount / studentCount) << "%\n";

    cout << "\n--- Ranking (High to Low) ---\n";
    cout << std::left << std::setw(5) << "#"
         << std::setw(16) << "ID"
         << std::setw(24) << "Name"
         << std::right << std::setw(10) << "Average"
         << std::setw(8) << "Grade"
         << "\n";

    cout << std::string(63, '-') << "\n";

    for (int rank = 0; rank < studentCount; ++rank)
    {
        int i = order[rank];
        double avg = averageRow(marks[i], testCount);
        cout << std::left << std::setw(5) << (rank + 1)
             << std::setw(16) << ids[i]
             << std::setw(24) << names[i]
             << std::right << std::setw(10) << std::fixed << std::setprecision(2) << avg
             << std::setw(8) << letterGrade(avg)
             << "\n";
    }
    cout << "\n";
}

int main()
{
    cout << "Student Gradebook + Analytics (arrays, loops, conditions, pointers)\n";
    cout << "-------------------------------------------------------------------\n";

    int testCount = readIntInRange("How many tests/exams per student (1..8)? ", 1, MAX_TESTS);

    char ids[MAX_STUDENTS][ID_LEN]{};
    char names[MAX_STUDENTS][NAME_LEN]{};
    int  marks[MAX_STUDENTS][MAX_TESTS]{}; // initialized to 0
    int  studentCount = 0;

    while (true)
    {
        cout << "\nMenu\n";
        cout << " 1) Add student\n";
        cout << " 2) Update a student's mark\n";
        cout << " 3) Print student report\n";
        cout << " 4) Class summary + ranking\n";
        cout << " 5) List all students\n";
        cout << " 0) Exit\n";

        int choice = readIntInRange("Choose: ", 0, 5);

        if (choice == 0) break;

        switch (choice)
        {
            case 1:
                addStudent(ids, names, marks, studentCount, testCount);
                break;
            case 2:
                updateMarks(ids, names, marks, studentCount, testCount);
                break;
            case 3:
                printStudentReport(ids, names, marks, studentCount, testCount);
                break;
            case 4:
                printClassSummaryAndRanking(ids, names, marks, studentCount, testCount);
                break;
            case 5:
                listStudents(ids, names, marks, studentCount, testCount);
                break;
        }
    }

    cout << "Goodbye.\n";
    return 0;
}