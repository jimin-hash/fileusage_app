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
	//string fileExp;
	unsigned long long countExp;
	unsigned long long fileSize;
	string location;
} fileInfo;
fileInfo files;

void searchFileBySwithes(bool& locFolder, path const& folderLo, string& xRex);
void scan(string sswitch, path const& folderLo, regex findExpReg, string& xRex);
void rscan(string sswitch, path const& folderLo, regex findExpReg, string& xRex);
void printTable(string sswitch, path const& folderLo, string& xRex);
void filesInfoOrder(string sswitch, path const& folderLo, string& xRex);
void hSwitchPrint();
void vSwithFileDetaile();
bool reverseOrder(pair<string, fileInfo>& x, pair<string, fileInfo>& y);
bool sizeAOrder(pair<string, fileInfo>& x, pair<string, fileInfo>& y);
bool sizeDOrder(pair<string, fileInfo>& x, pair<string, fileInfo>& y);

vector<string> vSwithes;
map<string, fileInfo> mFiles;
vector<pair<string, fileInfo>> vFiles;
map<pair<string, string>, unsigned int> vFileDetaile;
int main(int argc, char* argv[]) {
	cout << "fileusage {v1.0.0} (c) 2019, Jimin Park" << endl;

	path folderLo;
	bool locFolder = true;
	bool xSwitFind = false;
	string xRex = "";

	if (argc == 1) { // If user donesn't input swithes and folder / current folder and -s?
		locFolder = false;
	}
	else {
		regex switchFormat("-.*");
		if (regex_match(argv[1], switchFormat)) { // -switches store in vector?? // second argument is switches...
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
				if (argc < 3) { //
					cout << "Error: regular expression must follow -x switch." << endl;
					exit(EXIT_FAILURE);
				}
				else {
					xRex = argv[2];
					//get folder
					if (argc == 3) { // if user doesn't input folder argument
						locFolder = false;
					}
					else {
						folderLo = canonical(argv[3]);
					}
				}
			}
			else {
				//get folder
				if (argc == 2) { // if user doesn't input folder argument
					locFolder = false;
				}
				else {
					folderLo = canonical(argv[2]);
				}
			}
		}
		else // second argument is folder
			folderLo = canonical(argv[1]);
	}
	searchFileBySwithes(locFolder, folderLo, xRex);
}

void searchFileBySwithes(bool& locFolder, path const& folderLo, string& xRex) {
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
				rscan("A", current_path(), allFileReg, xRex);
			else
				rscan("A", folderLo, allFileReg, xRex);
		}

		vector<string>::iterator it = find(vSwithes.begin(), vSwithes.end(), "h");
		if (it != vSwithes.end()) {
			hSwitchPrint();
			exit(EXIT_SUCCESS);
		}
	
		it = find(vSwithes.begin(), vSwithes.end(), "r");
		if (it != vSwithes.end()) {
			if (vSwithes.size() == 1) {
				if (locFolder == false)
					scan("A", current_path(), allFileReg, xRex);
				else
					scan("A", folderLo, allFileReg, xRex);
			}
			else {
				if (locFolder == false) {
					for (size_t i = 0; i < vSwithes.size(); i++) {
						if (vSwithes[i] == "c") {
							scan("c", current_path(), cFileReg, xRex);
						}
						else if (vSwithes[i] == "+") {
							scan("+", current_path(), cppFileReg, xRex);
						}
						else if (vSwithes[i] == "j") {
							scan("j", current_path(), javaFileReg, xRex);
						}
						else if (vSwithes[i] == "#") {
							scan("#", current_path(), cSharpFileReg, xRex);
						}
						else if (vSwithes[i] == "w") {
							scan("w", current_path(), webFileReg, xRex);
						}
						else if (vSwithes[i] == "x") {

							scan("x", current_path(), xSwitchReg, xRex);
						}
						else if (vSwithes[i] == "s") {
							bool checkSwitch = false;

							for (size_t j = 0; j < vSwithes.size(); j++) {
								if (vSwithes[j] == "c") {
									scan("c", current_path(), cFileReg, xRex);
									checkSwitch = true;
								}
								else if (vSwithes[j] == "+") {
									scan("+", current_path(), cppFileReg, xRex);
									checkSwitch = true;
								}
								else if (vSwithes[j] == "j") {
									scan("j", current_path(), javaFileReg, xRex);
									checkSwitch = true;
								}
								else if (vSwithes[j] == "#") {
									scan("#", current_path(), cSharpFileReg, xRex);
									checkSwitch = true;
								}
								else if (vSwithes[j] == "w") {
									scan("w", current_path(), webFileReg, xRex);
									checkSwitch = true;
								}
								else if (vSwithes[j] == "x") {
									scan("x", current_path(), xSwitchReg, xRex);
									checkSwitch = true;
								}
							}

							if (checkSwitch == false){
								scan("A", current_path(), allFileReg, xRex);
							}
						}
					}
				}
				else { // folderLo
					for (size_t i = 0; i < vSwithes.size(); i++) {
						if (vSwithes[i] == "c") {
							scan("c", folderLo, cFileReg, xRex);
						}
						else if (vSwithes[i] == "+") {
							scan("+", folderLo, cppFileReg, xRex);
						}
						else if (vSwithes[i] == "j") {
							scan("j", folderLo, javaFileReg, xRex);
						}
						else if (vSwithes[i] == "#") {
							scan("#", folderLo, cSharpFileReg, xRex);
						}
						else if (vSwithes[i] == "w") {
							scan("w", folderLo, webFileReg, xRex);
						}
						else if (vSwithes[i] == "x") {

							scan("x", folderLo, xSwitchReg, xRex);
						}
						else if (vSwithes[i] == "s") {
							bool checkSwitch = false;

							for (size_t j = 0; j < vSwithes.size(); j++) {
								if (vSwithes[j] == "c") {
									scan("c", folderLo, cFileReg, xRex);
									checkSwitch = true;
								}
								else if (vSwithes[j] == "+") {
									scan("+", folderLo, cppFileReg, xRex);
									checkSwitch = true;
								}
								else if (vSwithes[j] == "j") {
									scan("j", folderLo, javaFileReg, xRex);
									checkSwitch = true;
								}
								else if (vSwithes[j] == "#") {
									scan("#", folderLo, cSharpFileReg, xRex);
									checkSwitch = true;
								}
								else if (vSwithes[j] == "w") {
									scan("w", folderLo, webFileReg, xRex);
									checkSwitch = true;
								}
								else if (vSwithes[j] == "x") {
									scan("x", folderLo, xSwitchReg, xRex);
									checkSwitch = true;
								}
							}

							if (checkSwitch == false) {
								scan("A", folderLo, allFileReg, xRex);
							}
						}
					}
				}
			}
		} // r switch
		else {
			if (locFolder == false) {
				for (size_t i = 0; i < vSwithes.size(); i++) {
					if (vSwithes[i] == "c") {
						rscan("c", current_path(), cFileReg, xRex);
					}
					else if (vSwithes[i] == "+") {
						rscan("+", current_path(), cppFileReg, xRex);
					}
					else if (vSwithes[i] == "j") {
						rscan("j", current_path(), javaFileReg, xRex);
					}
					else if (vSwithes[i] == "#") {
						rscan("#", current_path(), cSharpFileReg, xRex);
					}
					else if (vSwithes[i] == "w") {
						rscan("w", current_path(), webFileReg, xRex);
					}
					else if (vSwithes[i] == "x") {

						rscan("x", current_path(), xSwitchReg, xRex);
					}
					else if (vSwithes[i] == "s") {
						bool checkSwitch = false;

						for (size_t j = 0; j < vSwithes.size(); j++) {
							if (vSwithes[j] == "c") {
								rscan("c", current_path(), cFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "+") {
								rscan("+", current_path(), cppFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "j") {
								rscan("j", current_path(), javaFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "#") {
								rscan("#", current_path(), cSharpFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "w") {
								rscan("w", current_path(), webFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "x") {
								rscan("x", current_path(), xSwitchReg, xRex);
								checkSwitch = true;
							}
						}

						if (checkSwitch == false) {
							rscan("A", current_path(), allFileReg, xRex);
						}
					}
				}
			}
			else { // folderLo
				for (size_t i = 0; i < vSwithes.size(); i++) {
					if (vSwithes[i] == "c") {
						rscan("c", folderLo, cFileReg, xRex);
					}
					else if (vSwithes[i] == "+") {
						rscan("+", folderLo, cppFileReg, xRex);
					}
					else if (vSwithes[i] == "j") {
						rscan("j", folderLo, javaFileReg, xRex);
					}
					else if (vSwithes[i] == "#") {
						rscan("#", folderLo, cSharpFileReg, xRex);
					}
					else if (vSwithes[i] == "w") {
						rscan("w", folderLo, webFileReg, xRex);
					}
					else if (vSwithes[i] == "x") {
						rscan("x", folderLo, xSwitchReg, xRex);
					}
					else if (vSwithes[i] == "s") {
						bool checkSwitch = false;

						for (size_t j = 0; j < vSwithes.size(); j++) {
							if (vSwithes[j] == "c") {
								rscan("c", folderLo, cFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "+") {
								rscan("+", folderLo, cppFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "j") {
								rscan("j", folderLo, javaFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "#") {
								rscan("#", folderLo, cSharpFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "w") {
								rscan("w", folderLo, webFileReg, xRex);
								checkSwitch = true;
							}
							else if (vSwithes[j] == "x") {
								rscan("x", folderLo, xSwitchReg, xRex);
								checkSwitch = true;
							}
						}

						if (checkSwitch == false) {
							rscan("A", folderLo, allFileReg, xRex);
						}
					}
				}
			}
		}

		bool checkRSV = false; // cheak in vSwithes, there are only exist 'R' OT 'S'
		bool checkRecur = false; // cheak in vSwithes, there are 'r'

		for (auto s : vSwithes) {
			if (s != "R" && s != "S" && s != "v") {
				checkRSV = true;
			}

			if (s == "r") {
				checkRecur = true;
			}

		}
		vector<string>::iterator it1 = find(vSwithes.begin(), vSwithes.end(), "R");  // reverse order
		vector<string>::iterator it2 = find(vSwithes.begin(), vSwithes.end(), "S");
		vector<string>::iterator it3 = find(vSwithes.begin(), vSwithes.end(), "v"); // directory detaile
		if (it1 != vSwithes.end() || it2 != vSwithes.end() || it3 != vSwithes.end()) {
			if (checkRSV == false) { // only R or S or both
				if (checkRecur == true) {
					if (locFolder == false)
						scan("A", current_path(), allFileReg, xRex);
					else
						scan("A", folderLo, allFileReg, xRex);
				}
				else {
					if (locFolder == false)
						rscan("A", current_path(), allFileReg, xRex);
					else
						rscan("A", folderLo, allFileReg, xRex);
				}
			}
		}
	}
	catch (std::regex_error& e) {
		cout << e.what() << endl;
		hSwitchPrint();
		exit(EXIT_FAILURE);
	}
}

void filesInfoOrder(string sswitch, path const& folderLo, string& xRex){
	for (map<string, fileInfo>::iterator iter = mFiles.begin(); iter != mFiles.end(); iter++)
		vFiles.push_back(make_pair(iter->first, iter->second));

	vector<string>::iterator it1 = find(vSwithes.begin(), vSwithes.end(), "R");  // reverse order
	vector<string>::iterator it2 = find(vSwithes.begin(), vSwithes.end(), "S");
	
	if (it1 != vSwithes.end() && it2 != vSwithes.end()) {
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

	//for (vector<pair<string, fileInfo>>::iterator iter = vFiles.begin(); iter != vFiles.end(); iter++) {
	//	cout << iter->first << " " << iter->second.fileSize << endl;
	//	cout << iter->second.location << endl;
	//}

	printTable(sswitch, folderLo, xRex);
}

bool reverseOrder(pair<string, fileInfo>& x, pair<string, fileInfo>& y) {
	return x.first > y.first;
}

bool sizeAOrder(pair<string, fileInfo>& x, pair<string, fileInfo>& y) {
	return x.second.fileSize < y.second.fileSize;
}

bool sizeDOrder(pair<string, fileInfo>& x, pair<string, fileInfo>& y) {
	return x.second.fileSize > y.second.fileSize;
}

void scan(string sswitch, path const& folderLo, regex findExpReg, string& xRex) { // scan a single folder
	stringstream loc;
	loc << folderLo;
	auto err = std::error_code{};
	string expName;
	unsigned long long expCount = 1;
	unsigned long long expSize = 0;

	directory_iterator d(folderLo);		// first entry of folder 'f'
	directory_iterator e;				// virtual match to the 'end' of any folder

	while (d != e) {
		stringstream exp, path;
		exp << d->path().extension();
		path << d->path().parent_path();

		size_t found = path.str().find(loc.str());
		if (found != string::npos && regex_match(exp.str(),findExpReg)) {
			if (!is_directory(d->status())) {
				expName = exp.str();
				files.location = path.str();
				expSize = file_size(d->path(), err);

				map<pair<string, string>, unsigned int>::iterator iter = vFileDetaile.find(pair<string, string>(expName, path.str()));
				if (iter == vFileDetaile.end()) { // not found
					vFileDetaile.insert(make_pair(pair<string, string>(expName, files.location), 1));
				}
				else {
					iter->second++;
				}

				map<string, fileInfo>::iterator it = mFiles.find(expName);
				if (it != mFiles.end()) { // found
					it->second.countExp += 1;
					it->second.fileSize += expSize;

					files.countExp = it->second.countExp;
					files.fileSize = it->second.fileSize;
				}

				files.countExp = expCount;
				files.fileSize = expSize;
				
				mFiles.insert(pair<string, fileInfo>(expName, files));
			}
		}
		++d;
		expCount = 1;
		expSize = 0;
	} // while

	filesInfoOrder(sswitch, folderLo, xRex);
}

void rscan(string sswitch, path const& folderLo, regex findExpReg, string& xRex) { // scan the current folder and all sub folders
	stringstream loc;
	loc << folderLo;
	string expName;
	unsigned long long expCount = 1;
	unsigned long long expSize = 0;
	
	auto err = std::error_code{};

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

				map<pair<string, string>, unsigned int>::iterator iter = vFileDetaile.find(pair<string, string>(expName, path.str()));
				if (iter == vFileDetaile.end()) { // not found
					vFileDetaile.insert(make_pair(pair<string, string>(expName, files.location), 1));
				}
				else {
					iter->second++;
				}

				map<string, fileInfo>::iterator it = mFiles.find(expName);
				if (it != mFiles.end()) { // found
					it->second.countExp += 1;
					it->second.fileSize += expSize;

					files.countExp = it->second.countExp;
					files.fileSize = it->second.fileSize;
				}

				files.countExp = expCount;
				files.fileSize = expSize;

				mFiles.insert(pair<string, fileInfo>(expName, files));
			}
		}
	} // for
	
	filesInfoOrder(sswitch, folderLo, xRex);
}

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
	cout << "\t\tv\tverdose - list all files being scanned" << endl;
	cout << "\t\th\thelp" << endl;
	cout << "\tfolder" << endl;
	cout << "\t\tstarting folder or current folder if not provided" << endl;
}
void vSwithFileDetaile() {
	regex allFileReg(".*");
	regex cFileReg(R"reg(\.(c|h)$)reg");
	regex cppFileReg(R"reg(\.(cc|cp|cpp|cxx|hpp|hxx|c[/++/][/++/])$)reg");
	regex javaFileReg(R"reg(\.(class|j|jad|jar|java|jsp|ser)$)reg");
	regex cSharpFileReg(R"reg(\.(cs|vb|jsl)$)reg");
	regex webFileReg(R"reg(\.(htm|html|html5|js|jse|jsc)$)reg");
	//regex xSwitchReg(xRex);
	unsigned int countFold = 1;

	vector<pair<pair<string, string>, unsigned int>> vdetails;
	for (map<pair<string, string>, unsigned int>::iterator iter = vFileDetaile.begin(); iter != vFileDetaile.end(); iter++)
		vdetails.push_back(make_pair(iter->first, iter->second));

	for (size_t j = 0; j < vSwithes.size(); j++) {
		if (vSwithes[j] == "c") {

		}
		else if (vSwithes[j] == "+") {
		}
		else if (vSwithes[j] == "j") {
		}
		else if (vSwithes[j] == "#") {
		}
		else if (vSwithes[j] == "w") {
		}
		else if (vSwithes[j] == "x") {
		}
		else {
			// map<pair<string, string>, unsigned int> vFileDetaile;
			cout << "All files\n" << endl;
			string beforeExp;
			string a;
			vector<pair<unsigned int, string>> locations;

			for (size_t i = 0; i < vdetails.size(); i++) {
				if (i != 0)
					beforeExp = vdetails[i-1].first.first;
				else 
					beforeExp = vdetails[i].first.first;

				if ((beforeExp == vdetails[i].first.first) && (i != 0)) {
					//cout << vdetails[i-1].second << " " << vdetails[i-1].first.second << endl;
					//cout << vdetails[i].second << " " << vdetails[i].first.second << endl;
					countFold++;
					locations.push_back(make_pair(vdetails[i].second, vdetails[i].first.second));
				}
				else {
					cout << vdetails[i].first.first << " (" << countFold << " folder)" << endl;

					if (countFold > 1) {
						locations.push_back(make_pair(vdetails[i].second, vdetails[i].first.second));
						sort(locations.begin(), locations.end());

						for (auto l : locations)
							cout << "\t" << l.first << ":\t" << l.second << endl;
					}
					else {
						//cout << "\t" << vdetails[i].second << ":\t" << vdetails[i].first.second << endl;
						a = ("\t",vdetails[i].second,":\t",vdetails[i].first.second);
						cout << a << endl;
					}
					countFold = 1;
					cout << endl;
					cout << a << endl;
				}
			}
		}
	}
}

void printTable(string sswitch, path const& folderLo, string& xRex) {
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

	vector<string>::iterator ite = find(vSwithes.begin(), vSwithes.end(), "v");
	if (ite != vSwithes.end()) {
		if (sswitch == "A") {
			cout << "\nAll files" << endl;
			vSwithFileDetaile();
		}
		else if (sswitch == "c") {
			cout << "\nC files" << endl;
			vSwithFileDetaile();
		}
		else if (sswitch == "+") {
			cout << "\nC++ files" << endl;
			vSwithFileDetaile();
		}
		else if (sswitch == "j") {
			cout << "\nJava files" << endl;
			vSwithFileDetaile();
		}
		else if (sswitch == "#") {
			cout << "\nC# files" << endl;
			vSwithFileDetaile();
		}
		else if (sswitch == "w") {
			cout << "\nWeb files" << endl;
			vSwithFileDetaile();
		}
		else if (sswitch == "x") {
			cout << "\n" << xRex << " files" << endl;
			vSwithFileDetaile();
		}
	}

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
	else if (sswitch == "x") // fix
		cout << "\n"<< xRex <<" files: " << folderLo << endl;

	if (vFiles.size() == 0) { // map has no data 
		cout << "\nExt :   # : Total" << endl;
		cout << "- : --- : ---" << endl;
		cout << "- : --- : ---" << endl;
		cout << "0 :   0 :   0" << endl << endl;
	}
	else {
		for (auto a : vFiles) {
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
			numberF << a.second.fileSize;
			if (totalSizeLen < (numberF.str().length()))
				totalSizeLen = numberF.str().length();
		}

		string indent_exp1((maxExpLen + 1) - 3, ' ');
		string indent_exp2((maxExpLen + 1), '-');

		string indent_cnt1((totalFileLen + 2) - 1, ' ');
		string indent_cnt2((totalFileLen + 2), '-');

		string indent_size1((totalSizeLen + 2) - 5, ' ');
		string indent_size2((totalSizeLen + 2), '-');

		cout << "\n" << indent_exp1 << "Ext : " << indent_cnt1 << "# : " << indent_size1 << "Total" << endl;
		cout << indent_exp2 << " : " << indent_cnt2 << " : " << indent_size2 << endl;

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

		strChange.str("");
		strChange << totalExpCount;
		string indent_totalExp((maxExpLen + 1) - strChange.str().length(), ' ');
		string indent_totalCnt(2, ' ');
		string indent_totalSize(2, ' ');
		numberF.str("");
		numberF << totalFileSize;

		cout << indent_exp2 << " : " << indent_cnt2 << " : " << indent_size2 << endl;
		cout << indent_totalExp << totalExpCount << " : " << indent_totalCnt << totalFile << " : " << indent_totalSize << numberF.str() << endl <<endl;
	}
}