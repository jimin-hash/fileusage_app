#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <regex>
using namespace std;
using namespace std::experimental::filesystem;

// fileusage -vx "\.(txt|docx)" ..
// Error: regular expression must follow -x switch. -- fileusage -x"\.(txt|docx)"
// 'docx)' is not recognized as an internal or external command, operable program or batch file. -- fileusage -x \.(txt|docx)

int main(int argc, char* argv[]) {
	
	cout << "fileusage {v1.0.0} (c) 2019, Jimin Park" << endl;

	vector<char> vSwithes;
	path folderLo;

	if (argc == 1) { // If user donesn't input swithes and folder / current folder and -s?
		folderLo = L".";
	}
	else {
		regex switchFormat("-.*");
		if (regex_match(argv[1], switchFormat)) { // -switches store in vector?? // second argument is switches...
			string fullSwitches = argv[1];
			//char * chArr = new char[fullSwitches.size() + 1];
			//strcpy(chArr, fullSwitches.c_str());
				
			regex switches("c|j|#|w|s|x|r|R|S|v|h");
			//for (int i = 1; i < fullSwitches.size(); i++) {
			//	string s;
			//	s.push_back(chArr[i]);
			//	if (regex_match(s, switches)) {
			//		vSwithes.push_back(chArr[i]);
			//	}
			//	else if(s == "+"){
			//		vSwithes.push_back(chArr[i]);
			//	}
			//}

			for (int i = 1; i < fullSwitches.size(); i++) {
				string s;
				s.push_back(fullSwitches.at(i));
				if (regex_match(s, switches)) {
					vSwithes.push_back(fullSwitches.at(i));
				}
				else if (s == "+") {
					vSwithes.push_back(fullSwitches.at(i));
				}
			}

			for (auto s : vSwithes)
				cout << s << endl;
			
			//get folder
			if (argc == 2) { // if user doesn't input folder argument
				folderLo = L".";
			}
			else{
				folderLo = canonical(argv[2]);
			}
		}
		else { // second argument is folder
		}
	}
	
}	
