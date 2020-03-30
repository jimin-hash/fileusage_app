/*!	\file		fileusage_main.cpp
\author		Jimin Park
\date		2019-04-12
\version	0.1

 C++ Windows console application that lists file usage. Files will be counted and their sizes totaled and be grouped by file extension.
 command-line interface: fileusage [-switches] [folder]

 switches
 - c (c files)
 - + (c++ files)
 - j (java files)
 - # (c# files)
 - w (web files)
 - s (summary)
 - x (regex)
 - r (suppress recursive)
 - R (reverse order of listing)
 - S (sort by size)
 - v (verbose)
 - h (help)
*/
#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <regex>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;
using namespace std::experimental::filesystem;

typedef struct {
	unsigned long long	countExp;  // file exp count
	unsigned long long	fileSize;  // file size
	string				location;  // file path
} fileInfo;
fileInfo files;

/*!	 \fn reverseOrder
	 \return bool
	 \param pair<string, fileInfo>

	 Switch R -  list the files in reverse order */
bool reverseOrder(pair<string, fileInfo>& x, pair<string, fileInfo>& y) {
	return x.first > y.first;
}

/*!	 \fn sizeAOrder
	 \return bool
	 \param pair<string, fileInfo>

	 Switch S - sort the files by ascending file size */
bool sizeAOrder(pair<string, fileInfo>& x, pair<string, fileInfo>& y) {
	return x.second.fileSize < y.second.fileSize;
}

/*!	 \fn sizeAOrder
	 \return bool
	 \param pair<string, fileInfo>

	 Switch RS - sort the files by descending file size */
bool sizeDOrder(pair<string, fileInfo>& x, pair<string, fileInfo>& y) {
	return x.second.fileSize > y.second.fileSize;
}

void searchFileBySwithes(vector<string> vSwithes, map<string, fileInfo> mFiles, bool& locFolder, path const& folderLo, string& xRex);
void scan(vector<string> vSwithes, string sswitch, map<string, fileInfo> mFiles, path const& folderLo, regex findExpReg, string& xRex);
void rscan(vector<string> vSwithes, string sswitch, map<string, fileInfo> mFiles, path const& folderLo, regex findExpReg, string& xRex);
void scanV(string sswitch, map<pair<string, string>, unsigned int> vFileDetaile, path const& folderLo, regex findExpReg, string& xRex);
void rscanV(string sswitch, map<pair<string, string>, unsigned int> vFileDetaile, path const& folderLo, regex findExpReg, string& xRex);
void printTable(string sswitch, vector<pair<string, fileInfo>> vFiles, path const& folderLo, string& xRex);
void filesInfoOrder(vector<string> vSwithes, map<string, fileInfo>& mFiles, string sswitch, path const& folderLo, string& xRex);
void vSwitchSaveMap(vector<string> vSwithes, map<string, fileInfo> mFiles, bool& locFolder, path const& folderLo, string& xRex);
int vSwithFileDetaile(string sswitch, map<pair<string, string>, unsigned int> vFileDetaile, string& xRex);
void hSwitchPrint();

map<string, fileInfo> mFilesS; // Switch s - summary version

int main(int argc, char* argv[]) {
	cout << "fileusage {v1.0.0} (c) 2019, Jimin Park" << endl;

	map<string, fileInfo> mFiles;
	vector<string> vSwithes;

	path folderLo;
	bool locFolder = true;
	bool xSwitFind = false;
	string xRex = ""; // Switch 'x' regex

	if (argc == 1) { // If user donesn't input swithes and folder / current folder and -s?
		locFolder = false;
	}
	else {
		regex switchFormat("-.*");
		if (regex_match(argv[1], switchFormat)) { // -switches store in vector
			string fullSwitches = argv[1];
			regex switches("c|j|#|w|s|x|r|R|S|v|h|[/++/]");

			for (size_t i = 1; i < fullSwitches.size(); i++) {
				string s;
				s.push_back(fullSwitches.at(i));
				if (regex_match(s, switches))
					vSwithes.push_back(s);
				if (s == "x") // x switch
					xSwitFind = true;
			}
			if (xSwitFind == true) {// input could be 3 or 4
				if (argc < 3) { // if user didn't input regex for x switch
					cout << "Error: regular expression must follow -x switch." << endl;
					exit(EXIT_FAILURE);
				}
				else {
					xRex = argv[2];

					//get folder
					if (argc == 3) // if user doesn't input folder argument
						locFolder = false;
					else
						folderLo = canonical(argv[3]);
				}
			}
			else {
				//get folder
				if (argc == 2)  // if user doesn't input folder argument
					locFolder = false;
				else
					folderLo = canonical(argv[2]);
			}
		}
		else // second argument is folder
			folderLo = canonical(argv[1]);
	}
	searchFileBySwithes(vSwithes, mFiles, locFolder, folderLo, xRex);
}

/*!	 \fn searchFileBySwithes
	 \return number
	 \param vector<string> vSwithes, map<string, fileInfo> mFiles, bool& locFolder, path const& folderLo, string& xRex

	 depending switch , path() what user input or not and processing folders recursively or not
	 \note if regex for x switch is nor correct expression, this program will show error message and terminate*/
void searchFileBySwithes(vector<string> vSwithes, map<string, fileInfo> mFiles, bool& locFolder, path const& folderLo, string& xRex) {
	// Check user input folder exists
	if (locFolder != false && exists(folderLo) == false) {
		cout << "Error: folder[" << folderLo << "] doesn't exists." << endl;
		exit(EXIT_FAILURE);
	}

	try {
		regex allFileReg(".*");
		regex cFileReg(R"reg(\.(c|h)$)reg");
		regex cppFileReg(R"reg(\.(cc|cp|cpp|cxx|hpp|hxx|c[/++/][/++/])$)reg");
		regex javaFileReg(R"reg(\.(class|j|jad|jar|java|jsp|ser)$)reg");
		regex cSharpFileReg(R"reg(\.(cs|vb|jsl)$)reg");
		regex webFileReg(R"reg(\.(htm|html|html5|js|jse|jsc)$)reg");
		regex xSwitchReg(xRex);

		if (vSwithes.size() == 0) { // If there are no switches 
			if (locFolder == false)
				rscan(vSwithes, "A", mFiles, current_path(), allFileReg, xRex);
			else
				rscan(vSwithes, "A", mFiles, folderLo, allFileReg, xRex);
		}

		// if there is 'h' in switches vector, show the message then terminate.
		vector<string>::iterator it = find(vSwithes.begin(), vSwithes.end(), "h");
		if (it != vSwithes.end()) {
			hSwitchPrint();
			exit(EXIT_SUCCESS);
		}

		// if there is 'v' in switches vector, execute vSwitchSaveMap() to insert  lists a detailed report by extension 
		it = find(vSwithes.begin(), vSwithes.end(), "v"); //directory detaile
		if (it != vSwithes.end()) {
			vSwitchSaveMap(vSwithes, mFiles, locFolder, folderLo, xRex);
		}

		// if there is 'r' in switches vector, call scan()- suppress processing folders recursively
		it = find(vSwithes.begin(), vSwithes.end(), "r");
		if (it != vSwithes.end()) {
			if (vSwithes.size() == 1) {
				if (locFolder == false)
					scan(vSwithes, "A", mFiles, current_path(), allFileReg, xRex);
				else
					scan(vSwithes, "A", mFiles, folderLo, allFileReg, xRex);
			}
			else {
				if (locFolder == false) {
					it = find(vSwithes.begin(), vSwithes.end(), "c");
					if (it != vSwithes.end()) {
						scan(vSwithes, "c", mFiles, current_path(), cFileReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "+");
					if (it != vSwithes.end()) {
						scan(vSwithes, "+", mFiles, current_path(), cppFileReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "#");
					if (it != vSwithes.end()) {
						scan(vSwithes, "#", mFiles, current_path(), cSharpFileReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "j");
					if (it != vSwithes.end()) {
						scan(vSwithes, "j", mFiles, current_path(), javaFileReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "w");
					if (it != vSwithes.end()) {
						scan(vSwithes, "w", mFiles, current_path(), webFileReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "x");
					if (it != vSwithes.end()) {
						scan(vSwithes, "x", mFiles, current_path(), xSwitchReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "s");
					if (it != vSwithes.end()) {
						bool checkSwitch = false;

						for (size_t j = 0; j < vSwithes.size(); j++) {
							if (vSwithes[j] == "c") {
								scan(vSwithes, "cs", mFilesS, current_path(), cFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "+") {
								scan(vSwithes, "+s", mFilesS, current_path(), cppFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "j") {
								scan(vSwithes, "js", mFilesS, current_path(), javaFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "#") {
								scan(vSwithes, "#s", mFilesS, current_path(), cSharpFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "w") {
								scan(vSwithes, "ws", mFilesS, current_path(), webFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "x") {
								scan(vSwithes, "xs", mFilesS, current_path(), xSwitchReg, xRex);
								checkSwitch = true;
							}
						}

						if (checkSwitch == false) {
							scan(vSwithes, "A", mFiles, current_path(), allFileReg, xRex);
						}
					}
				}
				else { // folderLo
					it = find(vSwithes.begin(), vSwithes.end(), "c");
					if (it != vSwithes.end()) {
						scan(vSwithes, "c", mFiles, folderLo, cFileReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "+");
					if (it != vSwithes.end()) {
						scan(vSwithes, "+", mFiles, folderLo, cppFileReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "#");
					if (it != vSwithes.end()) {
						scan(vSwithes, "#", mFiles, folderLo, cSharpFileReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "j");
					if (it != vSwithes.end()) {
						scan(vSwithes, "j", mFiles, folderLo, javaFileReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "w");
					if (it != vSwithes.end()) {
						scan(vSwithes, "w", mFiles, folderLo, webFileReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "x");
					if (it != vSwithes.end()) {
						scan(vSwithes, "x", mFiles, folderLo, xSwitchReg, xRex);
					}

					it = find(vSwithes.begin(), vSwithes.end(), "s");
					if (it != vSwithes.end()) {
						bool checkSwitch = false;

						for (size_t j = 0; j < vSwithes.size(); j++) {
							if (vSwithes[j] == "c") {
								scan(vSwithes, "cs", mFilesS, folderLo, cFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "+") {
								scan(vSwithes, "+s", mFilesS, folderLo, cppFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "j") {
								scan(vSwithes, "js", mFilesS, folderLo, javaFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "#") {
								scan(vSwithes, "#s", mFilesS, folderLo, cSharpFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "w") {
								scan(vSwithes, "ws", mFilesS, folderLo, webFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "x") {
								scan(vSwithes, "xs", mFilesS, folderLo, xSwitchReg, xRex);
								checkSwitch = true;
							}
						}

						if (checkSwitch == false) {
							scan(vSwithes, "A", mFiles, folderLo, allFileReg, xRex);
						}
					}
				}
			}
		}
		else { // if there isn't 'r' in switches vector, call rscan() - recursive scan
			if (locFolder == false) {
				it = find(vSwithes.begin(), vSwithes.end(), "c");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "c", mFiles, current_path(), cFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "+");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "+", mFiles, current_path(), cppFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "#");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "#", mFiles, current_path(), cSharpFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "j");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "j", mFiles, current_path(), javaFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "w");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "w", mFiles, current_path(), webFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "x");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "x", mFiles, current_path(), xSwitchReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "s");
				if (it != vSwithes.end()) {
					bool checkSwitch = false;

					for (size_t j = 0; j < vSwithes.size(); j++) {
						if (vSwithes[j] == "c") {
							rscan(vSwithes, "cs", mFilesS, current_path(), cFileReg, xRex);
							checkSwitch = true;
						}
						else if (vSwithes[j] == "+") {
							rscan(vSwithes, "+s", mFilesS, current_path(), cppFileReg, xRex);
							checkSwitch = true;
						}
						else if (vSwithes[j] == "j") {
							rscan(vSwithes, "js", mFilesS, current_path(), javaFileReg, xRex);
							checkSwitch = true;
						}
						else if (vSwithes[j] == "#") {
							rscan(vSwithes, "#s", mFilesS, current_path(), cSharpFileReg, xRex);
							checkSwitch = true;
						}
						else if (vSwithes[j] == "w") {
							rscan(vSwithes, "ws", mFilesS, current_path(), webFileReg, xRex);
							checkSwitch = true;
						}
						else if (vSwithes[j] == "x") {
							rscan(vSwithes, "xs", mFilesS, current_path(), xSwitchReg, xRex);
							checkSwitch = true;
						}
					}

					if (checkSwitch == false) {
						rscan(vSwithes, "A", mFiles, current_path(), allFileReg, xRex);
					}
				}
			}
			else { // folderLo
				it = find(vSwithes.begin(), vSwithes.end(), "c");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "c", mFiles, folderLo, cFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "+");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "+", mFiles, folderLo, cppFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "#");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "#", mFiles, folderLo, cSharpFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "j");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "j", mFiles, folderLo, javaFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "w");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "w", mFiles, folderLo, webFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "x");
				if (it != vSwithes.end()) {
					rscan(vSwithes, "x", mFiles, folderLo, xSwitchReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "s");
				if (it != vSwithes.end()) {
					bool checkSwitch = false;
					for (size_t j = 0; j < vSwithes.size(); j++) {
						if (vSwithes[j] == "c") {
							rscan(vSwithes, "cs", mFilesS, folderLo, cFileReg, xRex);
							checkSwitch = true;
						}
						else if (vSwithes[j] == "+") {
							rscan(vSwithes, "+s", mFilesS, folderLo, cppFileReg, xRex);
							checkSwitch = true;
						}
						else if (vSwithes[j] == "j") {
							rscan(vSwithes, "js", mFilesS, folderLo, javaFileReg, xRex);
							checkSwitch = true;
						}
						else if (vSwithes[j] == "#") {
							rscan(vSwithes, "#s", mFilesS, folderLo, cSharpFileReg, xRex);
							checkSwitch = true;
						}
						else if (vSwithes[j] == "w") {
							rscan(vSwithes, "ws", mFilesS, folderLo, webFileReg, xRex);
							checkSwitch = true;
						}
						else if (vSwithes[j] == "x") {
							rscan(vSwithes, "xs", mFilesS, folderLo, xSwitchReg, xRex);
							checkSwitch = true;
						}
					}

					if (checkSwitch == false) {
						rscan(vSwithes, "A", mFiles, folderLo, allFileReg, xRex);
					}
				}
			}
		}

		bool checkRS = false; // cheak in vSwithes, there are only exist 'R' OT 'S'
		bool checkRecur = false;
		for (auto s : vSwithes) {
			if (s != "R" && s != "S")
				checkRS = true;
			if (s == "r")
				checkRecur = true;
		}

		// if in switches vector has only order switches such as 'R' ,'S' , it call scan() or rscan() depending existed 'r' as search all files
		vector<string>::iterator it1 = find(vSwithes.begin(), vSwithes.end(), "R");  // reverse order
		vector<string>::iterator it2 = find(vSwithes.begin(), vSwithes.end(), "S");
		if (it1 != vSwithes.end() || it2 != vSwithes.end()) {
			if (checkRS == false) { // only R or S or both
				if (checkRecur == true) {
					if (locFolder == false)
						scan(vSwithes, "A", mFiles, current_path(), allFileReg, xRex);
					else
						scan(vSwithes, "A", mFiles, folderLo, allFileReg, xRex);
				}
				else {
					if (locFolder == false)
						rscan(vSwithes, "A", mFiles, current_path(), allFileReg, xRex);
					else
						rscan(vSwithes, "A", mFiles, folderLo, allFileReg, xRex);
				}
			}
		}

		// call filesInfoOrder() for order and print, if swithes vector has 'v' 
		if (mFilesS.size() > 0) {
			filesInfoOrder(vSwithes, mFilesS, "A", folderLo, xRex);
		}
	}
	catch (std::regex_error& e) {
		cout << e.what() << endl;
		hSwitchPrint();
		exit(EXIT_FAILURE);
	}
}

/*!	 \fn vSwitchSaveMap
	 \return none
	 \param vector<string> vSwithes, map<string, fileInfo> mFiles, bool& locFolder, path const& folderLo, string& xRex

	 Each directory should only be listed once and the number of files found in that directory and the total number of folders found for each extension
	 Insert lists a detailed report by extension*/
void vSwitchSaveMap(vector<string> vSwithes, map<string, fileInfo> mFiles, bool& locFolder, path const& folderLo, string& xRex) {
	regex allFileReg(".*");
	regex cFileReg(R"reg(\.(c|h)$)reg");
	regex cppFileReg(R"reg(\.(cc|cp|cpp|cxx|hpp|hxx|c[/++/][/++/])$)reg");
	regex javaFileReg(R"reg(\.(class|j|jad|jar|java|jsp|ser)$)reg");
	regex cSharpFileReg(R"reg(\.(cs|vb|jsl)$)reg");
	regex webFileReg(R"reg(\.(htm|html|html5|js|jse|jsc)$)reg");
	regex xSwitchReg(xRex);

	map<pair<string, string>, unsigned int> vFileDetaile;

	vector<string>::iterator it = find(vSwithes.begin(), vSwithes.end(), "r");
	if (it != vSwithes.end()) {
		if (vSwithes.size() == 2) { // -rv
			if (locFolder == false) {
				scanV("A", vFileDetaile, current_path(), allFileReg, xRex);
			}
			else {
				scanV("A", vFileDetaile, folderLo, allFileReg, xRex);
			}
		}
		else {
			if (locFolder == false) {
				it = find(vSwithes.begin(), vSwithes.end(), "c");
				if (it != vSwithes.end()) {
					scanV("c", vFileDetaile, current_path(), cFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "+");
				if (it != vSwithes.end()) {
					scanV("+", vFileDetaile, current_path(), cppFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "#");
				if (it != vSwithes.end()) {
					scanV("#", vFileDetaile, current_path(), cSharpFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "j");
				if (it != vSwithes.end()) {
					scanV("j", vFileDetaile, current_path(), javaFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "w");
				if (it != vSwithes.end()) {
					scanV("w", vFileDetaile, current_path(), webFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "x");
				if (it != vSwithes.end()) {
					scanV("x", vFileDetaile, current_path(), xSwitchReg, xRex);
				}
			}
			else { // folderLo
				it = find(vSwithes.begin(), vSwithes.end(), "c");
				if (it != vSwithes.end()) {
					scanV("c", vFileDetaile, folderLo, cFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "+");
				if (it != vSwithes.end()) {
					scanV("+", vFileDetaile, folderLo, cppFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "#");
				if (it != vSwithes.end()) {
					scanV("#", vFileDetaile, folderLo, cSharpFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "j");
				if (it != vSwithes.end()) {
					scanV("j", vFileDetaile, folderLo, javaFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "w");
				if (it != vSwithes.end()) {
					scanV("w", vFileDetaile, folderLo, webFileReg, xRex);
				}

				it = find(vSwithes.begin(), vSwithes.end(), "x");
				if (it != vSwithes.end()) {
					scanV("x", vFileDetaile, folderLo, xSwitchReg, xRex);
				}
			}
		}
	} // r switch
	else {
		if (locFolder == false) {
			it = find(vSwithes.begin(), vSwithes.end(), "c");
			if (it != vSwithes.end()) {
				rscanV("c", vFileDetaile, current_path(), cFileReg, xRex);
			}

			it = find(vSwithes.begin(), vSwithes.end(), "+");
			if (it != vSwithes.end()) {
				rscanV("+", vFileDetaile, current_path(), cppFileReg, xRex);
			}

			it = find(vSwithes.begin(), vSwithes.end(), "#");
			if (it != vSwithes.end()) {
				rscanV("#", vFileDetaile, current_path(), cSharpFileReg, xRex);
			}

			it = find(vSwithes.begin(), vSwithes.end(), "j");
			if (it != vSwithes.end()) {
				rscanV("j", vFileDetaile, current_path(), javaFileReg, xRex);
			}

			it = find(vSwithes.begin(), vSwithes.end(), "w");
			if (it != vSwithes.end()) {
				rscanV("w", vFileDetaile, current_path(), webFileReg, xRex);
			}

			it = find(vSwithes.begin(), vSwithes.end(), "x");
			if (it != vSwithes.end()) {
				rscanV("x", vFileDetaile, current_path(), xSwitchReg, xRex);
			}
		}
		else { // folderLo
			it = find(vSwithes.begin(), vSwithes.end(), "c");
			if (it != vSwithes.end()) {
				rscanV("c", vFileDetaile, folderLo, cFileReg, xRex);
			}

			it = find(vSwithes.begin(), vSwithes.end(), "+");
			if (it != vSwithes.end()) {
				rscanV("+", vFileDetaile, folderLo, cppFileReg, xRex);
			}

			it = find(vSwithes.begin(), vSwithes.end(), "#");
			if (it != vSwithes.end()) {
				rscanV("#", vFileDetaile, folderLo, cSharpFileReg, xRex);
			}

			it = find(vSwithes.begin(), vSwithes.end(), "j");
			if (it != vSwithes.end()) {
				rscanV("j", vFileDetaile, folderLo, javaFileReg, xRex);
			}

			it = find(vSwithes.begin(), vSwithes.end(), "w");
			if (it != vSwithes.end()) {
				rscanV("w", vFileDetaile, folderLo, webFileReg, xRex);
			}

			it = find(vSwithes.begin(), vSwithes.end(), "x");
			if (it != vSwithes.end()) {
				rscanV("x", vFileDetaile, folderLo, xSwitchReg, xRex);
			}
		}
	}
}

/*!	 \fn vSwithFileDetaile
	 \return none
	 \param string sswitch, map<pair<string, string>, unsigned int> vFileDetaile, string& xRex

	  This function will print lists a detailed report by extension.  Each directory should only be listed once and the number of files found in that directory and the total number of folders found for each extension. */
int vSwithFileDetaile(string sswitch, map<pair<string, string>, unsigned int> vFileDetaile, string& xRex) {
	unsigned int countFold = 1;

	// declare vdetails and push_back values what from vFileDetaile map
	vector<pair<pair<string, string>, unsigned int>> vdetails;
	for (map<pair<string, string>, unsigned int>::iterator iter = vFileDetaile.begin(); iter != vFileDetaile.end(); iter++)
		vdetails.push_back(make_pair(iter->first, iter->second));

	cout << "\n";
	if (sswitch == "c") {
		cout << "C files\n" << endl;
	}
	else if (sswitch == "+") {
		cout << "C++ files\n" << endl;
	}
	else if (sswitch == "j") {
		cout << "Java files\n" << endl;
	}
	else if (sswitch == "#") {
		cout << "C# files\n" << endl;
	}
	else if (sswitch == "w") {
		cout << "Web files\n" << endl;
	}
	else if (sswitch == "A") {
		cout << "All files\n" << endl;
	}
	else {
		return EXIT_SUCCESS;
	}

	string beforeExp;
	vector<pair<unsigned int, string>> vTemp; // for order path()

	for (size_t i = 0; i < vdetails.size(); i++) { // check there are same exp in vdetails
		if (i != 0)
			beforeExp = vdetails[i - 1].first.first;
		else
			beforeExp = vdetails[i].first.first;

		// there are more than one exp
		if ((beforeExp == vdetails[i].first.first) && (i != 0)) {
			countFold++;
			vTemp.push_back(make_pair(vdetails[i].second, vdetails[i].first.second));
		}
		else {
			cout << vdetails[i].first.first << " (" << countFold << " folder)" << endl;

			if (countFold > 1) {
				vTemp.push_back(make_pair(vdetails[i].second, vdetails[i].first.second));
				sort(vTemp.begin(), vTemp.end());

				for (auto l : vTemp)
					cout << "\t" << l.first << ":\t" << l.second << endl;
			}
			else {
				cout << "\t" << vdetails[i].second << ":\t" << vdetails[i].first.second << "\n\n";
			}
			countFold = 1;
		}
	}

	return 0;
}
/*!	 \fn scanV
	 \return none
	 \param string sswitch, map<pair<string, string>, unsigned int> vFileDetaile, path const& folderLo, regex findExpReg, string& xRex

	 scan folders not recursively for detailed report by extension */
void scanV(string sswitch, map<pair<string, string>, unsigned int> vFileDetaile, path const& folderLo, regex findExpReg, string& xRex) {
	stringstream loc;
	loc << folderLo;
	auto err = std::error_code{};
	directory_iterator d(folderLo);		// first entry of folder 'f'
	directory_iterator e;				// virtual match to the 'end' of any folder

	while (d != e) {
		stringstream exp, path;
		exp << d->path().extension();
		path << d->path().parent_path();

		size_t found = path.str().find(loc.str());
		if (found != string::npos && regex_match(exp.str(), findExpReg)) {
			if (!is_directory(d->status())) {
				map<pair<string, string>, unsigned int>::iterator iter = vFileDetaile.find(pair<string, string>(exp.str(), path.str()));
				if (iter == vFileDetaile.end()) // not found
					vFileDetaile.insert(make_pair(pair<string, string>(exp.str(), path.str()), 1));
				else
					iter->second++;
			}
		}
		++d;
	}
	// print report by extension
	vSwithFileDetaile(sswitch, vFileDetaile, xRex);
}
/*!	 \fn rscanV
	 \return none
	 \param string sswitch, map<pair<string, string>, unsigned int> vFileDetaile, path const& folderLo, regex findExpReg, string& xRex

	 scan folders recursively for detailed report by extension */
void rscanV(string sswitch, map<pair<string, string>, unsigned int> vFileDetaile, path const& folderLo, regex findExpReg, string& xRex) {
	stringstream loc;
	loc << folderLo;
	auto err = std::error_code{};

	for (recursive_directory_iterator d(folderLo), e; d != e; ++d) {
		stringstream exp, path;
		exp << d->path().extension();
		path << d->path().parent_path();

		size_t found = path.str().find(loc.str());
		if (found != string::npos && regex_match(exp.str(), findExpReg)) {
			if (!is_directory(d->status())) {
				map<pair<string, string>, unsigned int>::iterator iter = vFileDetaile.find(pair<string, string>(exp.str(), path.str()));
				if (iter == vFileDetaile.end())
					vFileDetaile.insert(make_pair(pair<string, string>(exp.str(), path.str()), 1));
				else
					iter->second++;
			}
		}
	}
	// print report by extension
	vSwithFileDetaile(sswitch, vFileDetaile, xRex);
}

/*!	 \fn scan
	 \return none
	 \param vector<string> vSwithes, string sswitch, map<string, fileInfo> mFiles, path const& folderLo, regex findExpReg, string& xRex

	 scan folders not recursively*/
void scan(vector<string> vSwithes, string sswitch, map<string, fileInfo> mFiles, path const& folderLo, regex findExpReg, string& xRex) { // scanV a single folder
	stringstream loc;
	loc << folderLo;
	auto err = std::error_code{};
	string expName;
	unsigned long long expCount = 1;
	unsigned long long expSize = 0;
	vector<string>::iterator it2 = find(vSwithes.begin(), vSwithes.end(), "s");
	bool sumFind = false;
	regex sumRex(".s"); // summary regex 

	directory_iterator d(folderLo);		// first entry of folder 'f'
	directory_iterator e;				// virtual match to the 'end' of any folder

	while (d != e) {
		stringstream exp, path;
		exp << d->path().extension();
		path << d->path().parent_path();

		size_t found = path.str().find(loc.str());
		if (found != string::npos && regex_match(exp.str(), findExpReg)) {
			if (!is_directory(d->status())) {
				expName = exp.str();
				files.location = path.str();
				expSize = file_size(d->path(), err);

				map<string, fileInfo>::iterator it = mFiles.find(expName);
				if (it != mFiles.end()) { // if expName is already in mFiles, change countExp and filesize
					it->second.countExp += 1;
					it->second.fileSize += expSize;

					files.countExp = it->second.countExp;
					files.fileSize = it->second.fileSize;
				}

				files.countExp = expCount;
				files.fileSize = expSize;

				mFiles.insert(pair<string, fileInfo>(expName, files));

				if (it2 != vSwithes.end() && regex_match(sswitch, sumRex)) { // if 's' is in switch vector
					string sSwitchExp;
					if (sswitch == "cs" || sswitch == "c")
						sSwitchExp = "C files"; // sSwitchExp will be display intead of file Exp
					else if (sswitch == "+s" || sswitch == "+")
						sSwitchExp = "C++ files";
					else if (sswitch == "js" || sswitch == "j")
						sSwitchExp = "Java files";
					else if (sswitch == "#s" || sswitch == "#")
						sSwitchExp = "C# files";
					else if (sswitch == "ws" || sswitch == "w")
						sSwitchExp = "Web files";
					else if (sswitch == "xs" || sswitch == "x")
						sSwitchExp = xRex;

					it = mFilesS.find(sSwitchExp);
					if (it != mFilesS.end()) { // if expName is already in mFiles, change countExp and filesize
						it->second.countExp += 1;
						it->second.fileSize += expSize;

						files.countExp = it->second.countExp;
						files.fileSize = it->second.fileSize;
					}

					files.countExp = expCount;
					files.fileSize = expSize;

					mFilesS.insert(pair<string, fileInfo>(sSwitchExp, files));
					sumFind = true;
				}
			}
		}
		++d;
		expCount = 1;
		expSize = 0;
	} // while

	// if didn't find by passed regex expression, but it still insert in mFiles
	if (it2 != vSwithes.end() && regex_match(sswitch, sumRex)) {
		if (sumFind == false) {
			string sSwitchExp;
			if (sswitch == "cs")
				sSwitchExp = "C files";
			else if (sswitch == "+s")
				sSwitchExp = "C++ files";
			else if (sswitch == "js")
				sSwitchExp = "Java files";
			else if (sswitch == "#s")
				sSwitchExp = "C# files";
			else if (sswitch == "ws")
				sSwitchExp = "Web files";
			else if (sswitch == "xs")
				sSwitchExp = xRex;

			files.countExp = 0;
			files.fileSize = 0;
			files.location = "";
			mFilesS.insert(pair<string, fileInfo>(sSwitchExp, files));
		}
	}

	// 's' will not print in here 
	if (!regex_match(sswitch, sumRex))
		filesInfoOrder(vSwithes, mFiles, sswitch, folderLo, xRex);
}

/*!	 \fn rscan
	 \return none
	 \param vector<string> vSwithes, string sswitch, map<string, fileInfo> mFiles, path const& folderLo, regex findExpReg, string& xRex

	 scan folders recursively*/
void rscan(vector<string> vSwithes, string sswitch, map<string, fileInfo> mFiles, path const& folderLo, regex findExpReg, string& xRex) { // scanV the current folder and all sub folders
	stringstream loc;
	loc << folderLo;
	string expName;
	unsigned long long expCount = 1;
	unsigned long long expSize = 0;
	auto err = std::error_code{};
	vector<string>::iterator it2 = find(vSwithes.begin(), vSwithes.end(), "s");
	bool sumFind = false;
	regex sumRex(".s");// summary regex 

	for (recursive_directory_iterator d(folderLo), e; d != e; ++d) {
		stringstream exp, path;
		exp << d->path().extension();
		path << d->path().parent_path();

		size_t found = path.str().find(loc.str());
		if (found != string::npos && regex_match(exp.str(), findExpReg)) {
			if (!is_directory(d->status())) {
				expCount = 1;
				expSize = 0;
				expName = exp.str();
				files.location = path.str();
				expSize = file_size(d->path(), err);

				map<string, fileInfo>::iterator it = mFiles.find(expName);
				if (it != mFiles.end()) { // if expName is already in mFiles, change countExp and filesize
					it->second.countExp += 1;
					it->second.fileSize += expSize;

					files.countExp = it->second.countExp;
					files.fileSize = it->second.fileSize;
				}

				files.countExp = expCount;
				files.fileSize = expSize;

				mFiles.insert(pair<string, fileInfo>(expName, files));

				if (it2 != vSwithes.end() && regex_match(sswitch, sumRex)) { // if 's' is in switch vector
					string sSwitchExp;
					if (sswitch == "cs")
						sSwitchExp = "C files";  // sSwitchExp will be display intead of file Exp
					else if (sswitch == "+s")
						sSwitchExp = "C++ files";
					else if (sswitch == "js")
						sSwitchExp = "Java files";
					else if (sswitch == "#s")
						sSwitchExp = "C# files";
					else if (sswitch == "ws")
						sSwitchExp = "Web files";
					else if (sswitch == "xs")
						sSwitchExp = xRex;

					it = mFilesS.find(sSwitchExp);
					if (it != mFilesS.end()) { // if expName is already in mFiles, change countExp and filesize
						it->second.countExp += 1;
						it->second.fileSize += expSize;

						files.countExp = it->second.countExp;
						files.fileSize = it->second.fileSize;
					}

					files.countExp = expCount;
					files.fileSize = expSize;

					mFilesS.insert(pair<string, fileInfo>(sSwitchExp, files));
					sumFind = true;
				}
			}
		}
	} // for

	// if didn't find by passed regex expression, but it still insert in mFiles
	if (it2 != vSwithes.end() && regex_match(sswitch, sumRex)) {
		if (sumFind == false) {
			string sSwitchExp;
			if (sswitch == "cs")
				sSwitchExp = "C files";
			else if (sswitch == "+s")
				sSwitchExp = "C++ files";
			else if (sswitch == "js")
				sSwitchExp = "Java files";
			else if (sswitch == "#s")
				sSwitchExp = "C# files";
			else if (sswitch == "ws")
				sSwitchExp = "Web files";
			else if (sswitch == "xs")
				sSwitchExp = xRex;

			files.countExp = 0;
			files.fileSize = 0;
			files.location = "";
			mFilesS.insert(pair<string, fileInfo>(sSwitchExp, files));
		}
	}

	if (!regex_match(sswitch, sumRex))
		filesInfoOrder(vSwithes, mFiles, sswitch, folderLo, xRex);
}

/*!	 \fn filesInfoOrder
	 \return none
	 \param vector<string> vSwithes, map<string, fileInfo>& mFiles, string sswitch, path const& folderLo, string& xRex

	 sort vector for print of console application depending on switch vector has 'R' , 'S' or both*/
void filesInfoOrder(vector<string> vSwithes, map<string, fileInfo>& mFiles, string sswitch, path const& folderLo, string& xRex) {
	vector<pair<string, fileInfo>> vFiles;
	for (map<string, fileInfo>::iterator iter = mFiles.begin(); iter != mFiles.end(); iter++)
		vFiles.push_back(make_pair(iter->first, iter->second));

	vector<string>::iterator it1 = find(vSwithes.begin(), vSwithes.end(), "R");  // reverse order
	vector<string>::iterator it2 = find(vSwithes.begin(), vSwithes.end(), "S");

	if (it1 != vSwithes.end() && it2 != vSwithes.end()) { // both 'R' 'S'
		sort(vFiles.begin(), vFiles.end(), sizeDOrder);
	}
	else {
		if (it1 != vSwithes.end()) { //  list the files in reverse order
			sort(vFiles.begin(), vFiles.end(), reverseOrder);
		}

		if (it2 != vSwithes.end()) { // sort the files by ascending file size – or descending if the R switch is provided
			sort(vFiles.begin(), vFiles.end(), sizeAOrder);
		}
	}
	printTable(sswitch, vFiles, folderLo, xRex);
}

/*!	 \fn printTable
	 \return none
	 \param string sswitch, vector<pair<string, fileInfo>> vFiles, path const& folderLo, string& xRex

	 print fails info as table with number format(9,999,999)*/
void printTable(string sswitch, vector<pair<string, fileInfo>> vFiles, path const& folderLo, string& xRex) {
	unsigned int maxExpLen = 0;
	unsigned int totalFileLen = 0;
	unsigned int totalSizeLen = 0;
	unsigned long long printValLen = 0;
	unsigned long long totalExpCount = 0;
	unsigned long long totalFile = 0;
	unsigned long long totalFileSize = 0;
	stringstream strChange;
	ostringstream numberF;
	locale here("");
	numberF.imbue(here);

	if (sswitch == "A")
		cout << "\nAll files: " << folderLo << endl;
	else if (sswitch == "c")
		cout << "\nC files: " << folderLo << endl;
	else if (sswitch == "+")
		cout << "\nC++ files: " << folderLo << endl;
	else if (sswitch == "j")
		cout << "\nJava files: " << folderLo << endl;
	else if (sswitch == "#")
		cout << "\nC# files: " << folderLo << endl;
	else if (sswitch == "w")
		cout << "\nWeb files: " << folderLo << endl;
	else if (sswitch == "x")
		cout << "\n" << xRex << " files: " << folderLo << endl;

	if (vFiles.size() == 0) { // map has no data 
		cout << "\nExt :   # : Total" << endl;
		cout << "- : --- : ---" << endl;
		cout << "- : --- : ---" << endl;
		cout << "0 :   0 :   0" << endl << endl;
	}
	else {
		for (auto a : vFiles) { // calculate max length of total size and total files
			totalExpCount++;
			totalFile += a.second.countExp;
			totalFileSize += a.second.fileSize;

			strChange.str("");
			strChange << a.first;
			if (maxExpLen < strChange.str().length())
				maxExpLen = strChange.str().length();

			strChange.str("");
			strChange << totalFile;
			if (totalFileLen < (strChange.str().length()))
				totalFileLen = strChange.str().length();

			numberF.str("");
			numberF << totalFileSize;
			if (totalSizeLen < (numberF.str().length()))
				totalSizeLen = numberF.str().length();
		}

		// set string for ' ' and '-' depending max length
		string indent_exp1((maxExpLen + 1) - 3, ' ');
		string indent_exp2((maxExpLen + 1), '-');

		string indent_cnt1((totalFileLen + 2) - 1, ' ');
		string indent_cnt2((totalFileLen + 2), '-');

		string indent_size(0, ' ');
		string indent_size2((totalSizeLen + 2), '-');

		// print head(title)
		if (totalSizeLen > 3) {
			string indent_size1((totalSizeLen + 2 - 5), ' ');
			cout << "\n" << indent_exp1 << "Ext : " << indent_cnt1 << "# : " << indent_size1 << "Total" << endl;
		}
		else
			cout << "\n" << indent_exp1 << "Ext : " << indent_cnt1 << "# : " << indent_size << "Total" << endl;

		cout << indent_exp2 << " : " << indent_cnt2 << " : " << indent_size2 << endl;

		// print body(file info)
		for (auto a : vFiles) {
			string indent_expVal1((maxExpLen + 1) - a.first.length(), ' ');
			strChange.str("");
			strChange << a.second.countExp;

			string indent_cntVal1((totalFileLen + 2) - strChange.str().length(), ' ');
			numberF.str("");
			numberF << a.second.fileSize;

			string indent_sizeVal1((totalSizeLen + 2) - numberF.str().length(), ' ');

			cout << indent_expVal1 << a.first << " : " << indent_cntVal1 << a.second.countExp << " : " << indent_sizeVal1 << numberF.str() << endl;
		}

		// print total number and size
		strChange.str("");
		strChange << totalExpCount;
		string indent_totalExp((maxExpLen + 1) - strChange.str().length(), ' ');
		string indent_totalCnt(2, ' ');
		string indent_totalSize(2, ' ');
		numberF.str("");
		numberF << totalFileSize;

		cout << indent_exp2 << " : " << indent_cnt2 << " : " << indent_size2 << endl;
		cout << indent_totalExp << totalExpCount << " : " << indent_totalCnt << totalFile << " : " << indent_totalSize << numberF.str() << endl << endl;
	}
}

/*!	 \fn hSwitchPrint
	 \return none
	 \param none

	 There is 'h' in switches vector, show the message then terminate.*/
void hSwitchPrint() {
	cout << "\n\tUsage: fileusage [-hrRsSvc+#jw(x regularexpression)] [folder]" << endl;
	cout << "\t\tswitches:" << endl;
	cout << "\t\tc\tC files" << endl;
	cout << "\t\t+\tC++ files" << endl;
	cout << "\t\tj\tJava files" << endl;
	cout << "\t\t#\tC# files" << endl;
	cout << "\t\tw\tWeb files" << endl;
	cout << "\t\tx\tfilter with a regular expression" << endl;
	cout << "\t\ts\tsummary of the different categories" << endl;
	cout << "\t\tr\tsuppress recursive processing of the folder" << endl;
	cout << "\t\tR\treverse the order of the listing" << endl;
	cout << "\t\tS\tsort by file sizes" << endl;
	cout << "\t\tv\tverdose - list all files being scanVned" << endl;
	cout << "\t\th\thelp" << endl;
	cout << "\tfolder" << endl;
	cout << "\t\tstarting folder or current folder if not provided" << endl;
}