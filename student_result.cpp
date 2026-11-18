#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <numeric>
using namespace std;

class Student {
protected:
    int rollNo;
    string name;
public:
    Student(int roll, const string& studentName) : rollNo(roll), name(studentName) {}
    virtual ~Student() = default;
    virtual void display() const {
        cout << "Roll No: " << rollNo << " | Name: " << name;
    }
};

class Result : public Student {
    vector<int> marks;
public:
    Result(int roll, const string& studentName, const vector<int>& scores)
        : Student(roll, studentName), marks(scores) {
        if (marks.empty()) throw invalid_argument("At least one subject mark is required");
        for (int mark : marks)
            if (mark < 0 || mark > 100)
                throw out_of_range("Marks must be between 0 and 100");
    }
    int total() const { return accumulate(marks.begin(), marks.end(), 0); }
    double average() const { return static_cast<double>(total()) / marks.size(); }
    string status() const {
        return all_of(marks.begin(), marks.end(), [](int m) { return m >= 40; }) ? "PASS" : "FAIL";
    }
    void display() const override {
        Student::display();
        cout << " | Total: " << total() << " | Average: " << fixed
             << setprecision(2) << average() << " | Result: " << status() << '\n';
    }
    void save(ofstream& out) const {
        out << rollNo << ',' << name;
        for (int mark : marks) out << ',' << mark;
        out << '\n';
    }
};

template <typename T>
void showAbove(const vector<T>& students, double threshold) {
    cout << "\nStudents with average above " << threshold << ":\n";
    bool found = false;
    for (const auto& s : students) {
        if (s.average() > threshold) { s.display(); found = true; }
    }
    if (!found) cout << "None\n";
}

int main() {
    try {
        vector<Result> students;
        students.emplace_back(101, "Aisha", vector<int>{88, 92, 85});
        students.emplace_back(102, "Rahul", vector<int>{76, 81, 79});
        students.emplace_back(103, "Sara", vector<int>{95, 91, 94});
        students.emplace_back(104, "Imran", vector<int>{67, 35, 72});

        ofstream out("student_results.csv");
        if (!out) throw runtime_error("Unable to open output file");
        for (const auto& s : students) s.save(out);
        out.close();

        ifstream in("student_results.csv");
        if (!in) throw runtime_error("Unable to read saved file");
        string line;
        int records = 0;
        while (getline(in, line)) ++records;
        cout << "STUDENT RESULT MANAGEMENT SYSTEM\n";
        cout << "Records saved and retrieved: " << records << "\n\n";
        for (const Student& student : students) student.display();
        showAbove(students, 80.0);
        cout << "\nStudents passing all subjects: "
             << count_if(students.begin(), students.end(),
                 [](const Result& s) { return s.status() == "PASS"; }) << '\n';
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
