#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <regex>
#include <map>
#include <sstream>

using namespace std;
using namespace std::experimental::filesystem;

void searchFileBySwithes(vector<string> vSwithes, bool& locFolder, path const& folderLo);
void scan(vector<string> vSwithes, path const& folderLo);
void rscan(vector<string> vSwithes, path const& folderLo);

typedef struct {
	string fileExp;
	int countExp; //?
	unsigned long long fileScale;
	string location;
} fileInfo;

fileInfo files;

map<fileInfo, unsigned int> mFiles;

bool operator < (const fileInfo& f1, const fileInfo& f2)
{
	return (f1.fileExp < f2.fileExp);
}

// numbrg format
// c++ map struct as key
// fileusage -vx "\.(txt|docx)" ..
// Error: regular expression must follow -x switch. -- fileusage -x"\.(txt|docx)"
// 'docx)' is not recognized as an internal or external command, operable program or batch file. -- fileusage -x \.(txt|docx)

int main(int argc, char* argv[]) {
	
	cout << "fileusage {v1.0.0} (c) 2019, Jimin Park" << endl;

	vector<string> vSwithes;
	path folderLo;
	bool locFolder = true;
	bool xSwitFind = false;
	string xRex = "";

	if (argc == 1) { // If user donesn't input swithes and folder / current folder and -s?
		//folderLo = L".";
		locFolder = false;
	}
	else {
		regex switchFormat("-.*");
		if (regex_match(argv[1], switchFormat)) { // -switches store in vector?? // second argument is switches...
			string fullSwitches = argv[1];
				
			regex switches("c|j|#|w|s|x|r|R|S|v|h");

			for (int i = 1; i < fullSwitches.size(); i++) {
				string s;
				s.push_back(fullSwitches.at(i));
				if (regex_match(s, switches)) {
					vSwithes.push_back(s);
				}
				else if (s == "+") {
					vSwithes.push_back(s);
				}
				else if (s == "x") { // x switch
					xSwitFind = true;
				}
			}
			if (xSwitFind) {
				xRex = argv[2];
				//get folder
				if (argc == 3) { // if user doesn't input folder argument
					//folderLo = L".";
					locFolder = false;
				}
				else {
					folderLo = canonical(argv[2]);
				}
			}
			else {
				//get folder
				if (argc == 2) { // if user doesn't input folder argument
					//folderLo = L".";
					locFolder = false;
				}
				else {
					folderLo = canonical(argv[2]);
				}
			}

			
		}
		else { // second argument is folder
			folderLo = canonical(argv[1]);
		}
	}

	for (auto s : vSwithes)
		cout << s << endl;

	//cout << "location : " << folderLo << endl;

	searchFileBySwithes(vSwithes, locFolder, folderLo);
}	

void searchFileBySwithes(vector<string> vSwithes, bool& locFolder, path const& folderLo) {
	
	// Check user input folder exists
	if (exists(folderLo)) {
		cout << "Error: folder[" << folderLo << "] doesn't exists." << endl;
		exit(EXIT_FAILURE);
	}

	regex cFileReg(R"reg(\.(c|h)$)reg");
	regex cppFileReg(R"reg(\.(cc|cp|cpp|cxx|hpp|hxx)$)reg"); // c++???
	regex javaFileReg(R"reg(\.(class|j|jad|jar|java|jsp|ser)$)reg");
	regex cSharpFileReg(R"reg(\.(cs|vb|jsl)$)reg");
	regex webFileReg(R"reg(\.(htm|html|html5|js|jse|jsc)$)reg");
	regex xSwitchReg("");

	if (vSwithes.size() == 0) { // If there are no switches 
		if (locFolder == false)
			rscan(vSwithes, current_path());
		else
			rscan(vSwithes, folderLo);

		// exit??
	}

	for (int i = 0; i < vSwithes.size(); i++) {
		if (vSwithes[i] == "h") {
			cout << "\tUsage: fileusage [-hrRsSvc+#jw(x regularexpression)] [folder]" << endl;
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
			exit(EXIT_SUCCESS);
		}
		else if (vSwithes[i] == "c") {
		}
		else if (vSwithes[i] == "+") {
		}
		else if (vSwithes[i] == "j") {
		}
		else if (vSwithes[i] == "#") {
		}
		else if (vSwithes[i] == "w") {
		}
		else if (vSwithes[i] == "x") {
		}
		else if (vSwithes[i] == "s") { // all
		}
		else if (vSwithes[i] == "r") {
			if (locFolder == false)
				scan(vSwithes,current_path());
			else
				scan(vSwithes,folderLo);
		}
		else if (vSwithes[i] == "R") { // order
		}
		else if (vSwithes[i] == "S") { // order
		}
		else if (vSwithes[i] == "v") { // lists a detailed report by extension
		}
	}
}

void scan(vector<string> vSwithes,  path const& folderLo) { // scan a single folder
}

void rscan(vector<string> vSwithes, path const& folderLo) { // scan the current folder and all sub folders
	
	auto err = std::error_code{};
	int countExp = 0;
	unsigned long long capacity = 0;

	for (recursive_directory_iterator d(folderLo), e; d != e; ++d) {
		if (vSwithes.size() == 0) { // if user doesn't input any switches
			stringstream exp, path;
			int cExp = 1;
			unsigned long long capacity = 0;

			exp << d->path().extension();
			path << d->path();
			files.fileExp = exp.str();
			files.fileScale = file_size(d->path(), err);
			files.location = path.str();

			for (map<fileInfo, unsigned int>::iterator it = mFiles.begin(); it != mFiles.end(); it++) {
				cout<<"it->first.fileExp == "<< it->first.fileExp << "files.fileExp === " << files.fileExp << endl;
				if (it->first.fileExp == files.fileExp) {	
					cout << "11111" << endl;
					cExp += 1;
					capacity += files.fileScale;
					cout << "count == " << cExp<<endl;
				}
			}
			files.countExp = cExp;
			files.fileScale = capacity;
			mFiles.insert(pair<fileInfo, unsigned int>(files, 1));
		}

		//cout << d->path() <<
		//	(is_directory(d->status()) ? " [dir]" : "") <<
		//	" ext=" << d->path().extension() << " size = " << file_size(d->path(), err) <<
		//	endl;
	}

	for (auto a : mFiles) {
		cout << "fileExp =  " << a.first.fileExp << endl;
		cout << "count =  " << a.first.countExp << endl;
		cout << "fileScale =  " << a.first.fileScale << endl;
		cout << "location = " << a.first.location << endl;
		cout << "second = " << a.second << endl;
		cout << "------------------------------------" << endl;
	}
}
