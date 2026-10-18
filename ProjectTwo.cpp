
/*
 * CS 300 Project Two
 * ABCU Advising Assistance Program
 * Author: Hilnecia Larry
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

// Structure to store course information
struct Course {
    string courseNumber;
    string courseName;
    vector<string> prerequisites;
};


// Structure for each node in the binary search tree
struct Node {
    Course course;
    Node* left;
    Node* right;

    Node(Course newCourse) {
        course = newCourse;
        left = nullptr;
        right = nullptr;
    }
};


// Binary search tree to store and organize courses
class BinarySearchTree {
private:
    Node* root;

    void addNode(Node* node, Course course);
    void inOrder(Node* node);
    void destroyTree(Node* node);
    void collectCourses(Node* node, vector<Course>& courses);

public:
    BinarySearchTree();
    ~BinarySearchTree();

    void Insert(Course course);
    void PrintAll();
    Course* Search(string courseNumber);
    vector<Course> GetAllCourses();
};


/**
 * Default constructor
 */
BinarySearchTree::BinarySearchTree() {
    root = nullptr;
}

/**
 * Destructor
 */
BinarySearchTree::~BinarySearchTree() {
    destroyTree(root);
}

/**
 * Recursively delete all nodes in the tree
 */
void BinarySearchTree::destroyTree(Node* node) {
    if (node != nullptr) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}


/**
 * Insert a course into the binary search tree
 */
void BinarySearchTree::Insert(Course course) {
    if (root == nullptr) {
        root = new Node(course);
    }
    else {
        addNode(root, course);
    }
}

/**
 * Add a course to the correct position in the tree
 */
void BinarySearchTree::addNode(Node* node, Course course) {
    if (course.courseNumber < node->course.courseNumber) {
        if (node->left == nullptr) {
            node->left = new Node(course);
        }
        else {
            addNode(node->left, course);
        }
    }
    else {
        if (node->right == nullptr) {
            node->right = new Node(course);
        }
        else {
            addNode(node->right, course);
        }
    }
}



/**
 * Print all courses in alphanumeric order
 */
void BinarySearchTree::PrintAll() {
    inOrder(root);
}

/**
 * Traverse the tree and display each course
 */
void BinarySearchTree::inOrder(Node* node) {
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->course.courseNumber << ", "
            << node->course.courseName << endl;
        inOrder(node->right);
    }
}


/**
 * Collect courses from the binary search tree
 */
void BinarySearchTree::collectCourses(Node* node, vector<Course>& courses) {
    if (node != nullptr) {
        collectCourses(node->left, courses);
        courses.push_back(node->course);
        collectCourses(node->right, courses);
    }
}

/**
 * Return all courses stored in the binary search tree
 */
vector<Course> BinarySearchTree::GetAllCourses() {
    vector<Course> courses;
    collectCourses(root, courses);
    return courses;
}

/**
 * Search for a course by its course number
 */
Course* BinarySearchTree::Search(string courseNumber) {
    Node* current = root;

    while (current != nullptr) {
        if (courseNumber == current->course.courseNumber) {
            return &current->course;
        }
        else if (courseNumber < current->course.courseNumber) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    return nullptr;
}


/**
 * Read course information from a CSV file
 */
bool loadCourses(string fileName, BinarySearchTree& bst) {
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Could not open file." << endl;
        return false;
    }

    string line;
    vector<Course> courses;
    vector<string> courseNumbers;

    // Read each line and create a Course object
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);
        string item;
        Course course;


        int fieldNumber = 0;

        while (getline(ss, item, ',')) {
            if (fieldNumber == 0) {
                course.courseNumber = item;
            }
            else if (fieldNumber == 1) {
                course.courseName = item;
            }
            else if (!item.empty()) {
                course.prerequisites.push_back(item);
            }

            fieldNumber++;
        }

        // Check that the course number and name are present
        if (course.courseNumber.empty() || course.courseName.empty()) {
            cout << "Error: Invalid course information." << endl;
            file.close();
            return false;
        }

        courses.push_back(course);
        courseNumbers.push_back(course.courseNumber);
    }

    file.close();

    // Check that every prerequisite exists in the file
    for (Course course : courses) {
        for (string prerequisite : course.prerequisites) {
            bool found = false;

            for (string number : courseNumbers) {
                if (prerequisite == number) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Error: Prerequisite " << prerequisite
                    << " was not found." << endl;
                return false;
            }
        }
    }

    // Insert courses only after validation is successful
    for (Course course : courses) {
        bst.Insert(course);
    }

    cout << "Courses loaded successfully." << endl;
    return true;
}

/**
 * Print a course schedule with prerequisites first
 */
void printCourseSchedule(BinarySearchTree& bst) {
    vector<Course> courses = bst.GetAllCourses();
    vector<string> completed;
    int totalCourses = courses.size();

    cout << endl;
    cout << "Suggested Course Schedule:" << endl;
    cout << endl;

    while (completed.size() < totalCourses) {
        bool courseAdded = false;

        for (Course course : courses) {
            bool prerequisitesMet = true;

            // Check whether each prerequisite is completed
            for (string prerequisite : course.prerequisites) {
                bool found = false;

                for (string number : completed) {
                    if (prerequisite == number) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    prerequisitesMet = false;
                    break;
                }
            }

            // Check whether this course is already scheduled
            bool alreadyAdded = false;

            for (string number : completed) {
                if (course.courseNumber == number) {
                    alreadyAdded = true;
                    break;
                }
            }

            // Add the course when its prerequisites are met
            if (prerequisitesMet && !alreadyAdded) {
                cout << course.courseNumber << ", "
                    << course.courseName << endl;

                completed.push_back(course.courseNumber);
                courseAdded = true;
            }
        }

        // Stop if no more courses can be scheduled
        if (!courseAdded) {
            cout << "Unable to complete schedule due to prerequisites."
                << endl;
            break;
        }
    }
}


/**
 * Main function
 */
int main() {
    BinarySearchTree bst;
    int choice = 0;
    bool dataLoaded = false;

    cout << "Welcome to the course planner." << endl;

    while (choice != 9) {
        cout << endl;
        cout << "1. Load Data Structure." << endl;
        cout << "2. Print Course List." << endl;
        cout << "3. Print Course." << endl;
        cout << "4. Print Course Schedule (Extra Credit)." << endl;
        cout << "9. Exit" << endl;
        cout << endl;
        cout << "What would you like to do? ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            choice = 0;
        }

        // Option 1: Load course information
        if (choice == 1) {
            if (!dataLoaded) {
                string fileName;

                cout << "Enter the course data filename: ";
                cin >> ws;
                getline(cin, fileName);

                dataLoaded = loadCourses(fileName, bst);
            }
            else {
                cout << "Course data has already been loaded." << endl;
            }
        }

        // Option 2: Print all courses
        else if (choice == 2) {
            if (dataLoaded) {
                cout << endl;
                cout << "Here is a sample schedule:" << endl;
                cout << endl;
                bst.PrintAll();
            }
            else {
                cout << "Please load course data first." << endl;
            }
        }


        // Option 3: Print information for one course
        else if (choice == 3) {
            if (dataLoaded) {
                string courseNumber;

                cout << "What course do you want to know about? ";
                cin >> courseNumber;

                // Convert lowercase letters to uppercase
                for (char& letter : courseNumber) {
                    letter = toupper(letter);
                }

                Course* course = bst.Search(courseNumber);

                if (course != nullptr) {
                    cout << endl;
                    cout << course->courseNumber << ", "
                        << course->courseName << endl;

                    cout << "Prerequisites: ";

                    if (course->prerequisites.empty()) {
                        cout << "None";
                    }
                    else {
                        for (int i = 0; i < course->prerequisites.size(); i++) {
                            Course* prerequisite =
                                bst.Search(course->prerequisites[i]);

                            if (prerequisite != nullptr) {
                                cout << prerequisite->courseNumber << ", "
                                    << prerequisite->courseName;
                            }

                            if (i < course->prerequisites.size() - 1) {
                                cout << "; ";
                            }
                        }
                    }

                    cout << endl;
                }
                else {
                    cout << "Course not found." << endl;
                }
            }
            else {
                cout << "Please load course data first." << endl;
            }
        }

        // Option 4: Print a prerequisite-based course schedule
        else if (choice == 4) {
            if (dataLoaded) {
                printCourseSchedule(bst);
            }
            else {
                cout << "Please load course data first." << endl;
            }
        }

        // Handle invalid menu selections
        else if (choice != 9) {
            cout << choice << " is not a valid option." << endl;
        }
    }

    cout << "Thank you for using the course planner!" << endl;

    return 0;
}
