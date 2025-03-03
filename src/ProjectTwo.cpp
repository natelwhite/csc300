#include <string>
#include <cmath>
#include <fstream>
#include <set>
#include <vector>
#include <iostream>
#include <climits>

struct Course {
	std::string number;
	std::string title;
	std::vector<std::string> prerequisites;
	friend std::ostream &operator<<(std::ostream &os, const Course &course) {
		std::cout << "Number: " << course.number << std::endl << "Title: " << course.title << std::endl;
		std::cout << "Prerequisites: ";
		// iter over prerequisites and print them
		for (int i = 0; i < course.prerequisites.size(); ++i) {
			// if last prerequisite, don't print comma
			if (i == course.prerequisites.size() - 1) {
				std::cout << course.prerequisites.at(i);
			} else {
				std::cout << course.prerequisites.at(i) << ", ";
			}
		}
		return os;
	}
};

class HashTable {
	private:
		// Define structures to hold courses
		struct Node {
			Course course;
			unsigned int key;
			Node *next;
			// default constructor
			Node() {
				key = UINT_MAX;
				next = nullptr;
			}
			// initialize with a bid
			Node(Course t_course) : Node() {
				course = t_course;
			}
			// initialize with a bid and a key
			Node(Course t_course, unsigned int t_key) : Node(t_course) {
				key = t_key;
			}
		};
	std::vector<Node> m_nodes;
	unsigned int m_size { 179 };
	unsigned int hash(std::string course_number) const;

	public:
		HashTable();
		HashTable(const unsigned int &t_size);
		~HashTable();
		void loadFromCSV(const std::string &file_path);
		void insert(const Course &course);
		void remove(const std::string &course_number);
		Course search(const std::string &course_number) const;
		std::vector<Course> toVector() const;
};

/**
 * Default constructor
 */
HashTable::HashTable() {
	// Initalize node structure by resizing tableSize
	m_nodes.resize(m_size);
}

/**
 * Constructor for specifying size of the table
 * Use to improve efficiency of hashing algorithm
 * by reducing collisions without wasting memory.
 */
HashTable::HashTable(const unsigned int &t_size) : m_size(t_size) {
	m_nodes.resize(m_size);
}

/**
 * Destructor
 */
HashTable::~HashTable() {
	// traverse vector
	for (Node head : m_nodes) {
		// if key exists, traverse SSL and remove all nodes
		if (head.key != UINT_MAX) {
			// traverse SSL (heads will be removed later)
			if (head.next != nullptr) {
				Node *cur_node { head.next };
				while (cur_node->next != nullptr) {
					Node *temp = cur_node; // make temp node to delete later
					cur_node = cur_node->next; // move cur_node forward
					delete temp;
				}
				delete cur_node; // delete tail of SSL
			}
		}
	}
	m_nodes.clear(); // remove all head nodes
}

/*
 * Load Courses from Comma Separated List
 *
 * @param const std::string path to file
*/
void HashTable::loadFromCSV(const std::string &file_path) {
	// set up vars for reading file
	std::ifstream csv { file_path };
	std::string row;
	const char ROW_DEL { '\n' }, COL_DEL { ',' };
	// iter by row
	while (getline(csv, row, ROW_DEL)) {
		std::string field;
		int field_count { };
		int start { };
		int end { };
		std::string number, title;
		std::vector<std::string> prerequisites;
		// iter by column
		while ((end = row.find(COL_DEL, start)) != std::string::npos) {
			field = row.substr(start, end - start);
			// skip empty fields
			if (end - start == 0) {
				start = end + 1;
				continue;
			}
			// determine field type
			switch(field_count) {
			case 0: // course number
				number = field;
				break;
			case 1: // course title
				title = field;
				break;
			default: // course prerequisite
				prerequisites.push_back(field);
				break;
			}
			++field_count;
			start = end + 1;
		}
		// if there's still more data but no more commas:
		// add it to prerequisites
		if (start < row.length() - 1) {
			prerequisites.push_back(row.substr(start, row.length() - start - 1).data());
		}
		// insert data into data struct
		insert({number, title, prerequisites});
	}
	csv.close();
}

/**
 * Calculate the hash value of a given key.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(std::string key) const {
	size_t sum;
	for (int i = 0; i < key.length(); i++) {
		sum += (key[i] * static_cast<int>(pow(31, i))) % m_size;
	}
	return sum % m_size;
}

/**
 * Insert a bid
 *
 * @param Course The course to insert
 */
void HashTable::insert(const Course &course) {
	// create the key for the given bid
	unsigned int key { hash(course.number) };
	// if key doesn't exist yet
	if (m_nodes.at(key).key == UINT_MAX) {
		m_nodes.at(key) = Node { course, key }; // assign this node to the key position
	// if key exists
	} else {
		// traverse SSL and append new node to end
		Node *cur_node { &m_nodes.at(key) };
		while (cur_node->next != nullptr) {
			cur_node = cur_node->next; // move forward in SSL
		}
		cur_node->next = new Node {course, key};
	}
}

/**
 * Get Vector representation of table
 *
 * @return a vector of all courses within table
 */
std::vector<Course> HashTable::toVector() const {
	// traverse vector
	std::vector<Course> courses;
	for (const Node head : m_nodes) {
		// if key exists
		if (head.key != UINT_MAX) {
			courses.push_back(head.course);
			// print head first, then print all nodes head is connected to
			Node *cur_node { head.next };
			// traverse SSL
			while (cur_node != nullptr) {
				courses.push_back(cur_node->course);
				cur_node = cur_node->next; // move forward in SSL
			}
		}
	}
	return courses;
}

/**
 * Remove a bid
 *
 * @param bidId The bid id to search for
 */
void HashTable::remove(const std::string &course_number) {
	// get key and node at key pos
	unsigned int key { hash(course_number) };
	Node *head { &m_nodes.at(key) };
	// if key exists
	if (head->key != UINT_MAX) {
		// if head is bid, put head->next at head if it exists, otherwise make it an empty node
		if (head->course.number == course_number) {
			if (head->next != nullptr) {
				m_nodes.at(key) = *head->next;
			} else {
				m_nodes.at(key) = Node {};
			}
			return; // early return to avoid SSL traversal
		}
		// traverse SSL
		Node *cur_node { head };
		while (cur_node->next != nullptr) {
			// if next node is match
			if (cur_node->next->course.number == course_number) {
				Node *temp = cur_node->next; // make temp to delete later
				cur_node->next = temp->next; // make cur_node jump over temp
				delete temp; // free memory
			}
			cur_node = cur_node->next; // move forward in SSL
		}
	}
}

/**
 * Search for the specified bidId
 *
 * @param std::string The course number to search for
 */
Course HashTable::search(const std::string &course_number) const {
	// get key and node at key pos
	unsigned int key { hash(course_number) };
	Node head { m_nodes.at(key) };
	// if key exists
	if (head.key != UINT_MAX) {
		// if head is course, return it
		if (head.course.number == course_number) {
			return head.course; // early return to avoid SSl traversal
		}
		// traverse SSL and check other bids share the same key
		Node *cur_node { head.next };
		while (cur_node != nullptr) {
			// if cur_node is course, return it
			if (cur_node->course.number == course_number) {
				return cur_node->course;
			}
			cur_node = cur_node->next; // move forward in SSL
		}
	}
	// couldn't find course, return an empty one
	return Course {};
}

void Quicksort(std::vector<Course> *courses, int lowIndex, int highIndex) {
	auto partition = [courses](int low, int high) -> int {
		// Pick middle element as pivot
		int midpoint = low + (high - low) / 2;
		std::string pivot = courses->at(midpoint).number;
		bool done = false;
		while (!done) {
			// Increment lowIndex while courses.at(lowIndex) < pivot
			while (courses->at(low).number.compare(pivot) < 0) {
				low += 1;
			}
			// Decrement highIndex while pivot < courses.at(highIndex)
			while (pivot.compare(courses->at(high).number) < 0) {
				high -= 1;
			}
			// If zero or one elements remain, then all numbers are partitioned. retun highIndex
			if (low >= high) {
				done = true;
			} else {
				// Swap low and high
				Course temp = courses->at(low);
				courses->at(low) = courses->at(high);
				courses->at(high) = temp;
				// Update lowIndex and highIndex
				low += 1;
				high -= 1;
			}
		}
		return high;
	};
	// Base case: if the partition size is 1 or zero elements, then the partition is already sorted
	if (lowIndex >= highIndex) {
		return;
	}
	// Partition the data within the array. Value lowEndIndex returned from partitioning is the index of hte low partition's last element.
	int lowEndIndex = partition(lowIndex, highIndex);
	// Recursively sort low partition (lowIndex to lowEndIndex)
	// and high partition (lowEndIndex + 1 to highIndex)
	Quicksort(courses, lowIndex, lowEndIndex);
	Quicksort(courses, lowEndIndex + 1, highIndex);
}

/*
 * Validate the formatting of a comma separated list of courses
 *
 * @return int if bad formatting: -1, otherwise: the number of rows in the CSV
*/
int ValidateFile(const std::string &file_path) {
	std::set<std::string> courses; // store course titles
	std::set<std::string> prerequisites; // store prerequisite course titles
	// set up vars for reading file
	std::ifstream csv { file_path };
	if (!csv.is_open()) {
		std::cout << "Failed to open file: " << file_path << std::endl;
	}
	std::string row;
	const char ROW_DEL { '\n' }, COL_DEL { ',' };
	// row description: number,title,prerequisite 1,prerequisite 2,etc.
	// store all course numbers and prerequisites numbers
	// iter by row
	int row_count { };
	while (getline(csv, row, ROW_DEL)) {
		std::string field;
		int cur_field { };
		int start { }; // start index of field
		int end { }; // end index of field
		// iter by column
		while ((end = row.find(COL_DEL, start)) != std::string::npos) {
			field = row.substr(start, end - start);
			// skip empty fields
			if (end - start == 0) {
				start = end + 1;
				continue;
			}
			switch(cur_field) {
			case 0: // course number
				courses.insert(field);
				break;
			case 1: // course title
				break;
			default: // course prerequisite
				prerequisites.insert(field);
				break;
			}
			++cur_field;
			start = end + 1; // move forward
		}
		// if there's still more data but no more commas:
		// add it to prerequisites
		if (start < row.length() - 1) {
			prerequisites.insert(row.substr(start, row.length() - start - 1));
		}
		// check for min number of fields
		if (cur_field < 2) {
			std::cout << "There must be at minimum a course number and title, but only " << cur_field << " values were found." << std::endl;
			return -1;
		}
		row_count++;
	}
	csv.close();
	for (std::string course : prerequisites) {
		if (courses.count(course) == 0) {
			std::cout << "No entry found for listed prerequisite: " << course << std::endl;
			return -1;
		}
	}
	return row_count;
}

/*
 * The Application's main function
*/
int main(int argc, char *argv[]) {
	// set path to default path unless a path was passed as argument
	std::string path { argc > 1 ? argv[1] : "./CS 300 ABCU_Advising_Program_Input.csv" };
	int num_courses { };
	// if bad formatting
	if ((num_courses = ValidateFile(path)) < 0) {
		std::cout << "Could not validate data in file: " << path << std::endl;
		return -1;
	}
	// reserve memory for data
	HashTable data (num_courses);
	int choice { };
	const char *MENU { "Menu:\n\t1. Load Courses\n\t2. Print Courses in Order\n\t3. Find and Print Course\n\t9. Exit\nSelection: " };
	while (choice != 9) {
		std::cout << MENU;
		std::cin >> choice;
		// bad input check
		if (std::cin.fail()) {
			std::cout << "Menu option unknown. Please select a valid option (1, 2, 3, 9)." << std::endl;
			std::cin.clear();
			std::cin.ignore(UINT_MAX);
			continue;
		}
		switch(choice) {
		case 1: // load data from file
			data.loadFromCSV(path);
			break;
		case 2: { // print ordered data
			std::vector<Course> courses { data.toVector() };
			Quicksort(&courses, 0, courses.size() - 1);
			for (Course course : courses) {
				std::cout << course << std::endl;
			}
			break;
		}
		case 3: { // find and print course
			std::string course_number;
			std::cout << "Course number: ";
			std::cin >> course_number;
			Course result { data.search(course_number) };
			if (result.title.compare("") == 0) {
				std::cout << "Could not find course with number: " << course_number << std::endl;
			} else {
				std::cout << result << std::endl;
			}
			break;
		}
		case 9: // quit
			break;
		default: // unkown input
			std::cout << "Menu option unknown. Please select a valid option (1, 2, 3, 9)." << std::endl;
		}
	}
}

