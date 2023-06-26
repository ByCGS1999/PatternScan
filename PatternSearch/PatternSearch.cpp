#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <stdexcept>
#include <vector>
#include <fstream>
#include <ostream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

typedef enum VerboseLevel { None, Single, Double };

VerboseLevel verboseLevel = VerboseLevel::None;

string fileDump = "PtrDump.txt";

string logDump = "";

bool dump = false;
bool decompileFile = false;
bool recursive = false;

int main(int argc, char* argv[])
{
	ofstream file(fileDump);

	vector<string> launchArgs = vector<string>();
	vector<string> relatives = vector<string>();

	if (argc > 2) {

		string targetPath = "";
		string pattern = "";

		for (int x = 0; x < argc; x++)
		{
			string argument = string(argv[x]);

			launchArgs.push_back(argument);
		}

		int currentIdx = 0;

		for (string s : launchArgs)
		{
			if (currentIdx == 1)
			{
				targetPath = s;
			}
			else if (currentIdx == 2)
			{
				pattern = s;
			}
			else
			{
				if (s == "-V")
				{
					if (verboseLevel != VerboseLevel::Double)
					{
						verboseLevel = VerboseLevel::Single;
					}
				}

				if (s == "-P")
				{
					decompileFile = true;
				}

				if (s == "-R")
				{
					recursive = true;
				}

				if (s == "-VV")
				{
					verboseLevel = VerboseLevel::Double;
				}

				if (s == "-O")
				{
					dump = true;
				}
			}

			currentIdx++;
		}


		if (verboseLevel != VerboseLevel::None)
		{
			cout << " -- [PtrScan] -- " << endl;
		}

		if (dump)
		{
			logDump += "-- [PtrScan] -- \n";
		}

		if (verboseLevel != VerboseLevel::None)
		{
			if (recursive)
			{
				cout << "Searching on all the files from '" << targetPath << "' if contains the pattern '" << pattern << "'" << endl;
			}
			else
			{
				cout << "Searching in the file '" << targetPath << "' if contains the pattern '" << pattern << "'" << endl;
			}
		}
		if (dump)
		{
			if (recursive)
			{
				logDump += "Searching on all the files from '" + targetPath + "' if contains the pattern '" + pattern + "'\n";
			}
			else
			{
				logDump += "Searching in the file '" + targetPath + "' if contains the pattern '" + pattern + "'\n";
			}
		}

		if (recursive) {
			for (const auto& entry : fs::directory_iterator(targetPath))
			{
				if (verboseLevel != VerboseLevel::None)
				{
					cout << " ------------------------------------------ \n";
				}

				if (dump)
				{
					logDump += "------------------------------------------ \n";
				}

				stringstream tempData;

				bool relative = false;

				if (entry.file_size() > (uintmax_t)(0))
				{
					if (verboseLevel == VerboseLevel::Double)
					{
						cout << "[PtrScan]: Analyzing file for relatives.\n";
					}

					if (dump)
					{
						logDump += "[PtrScan]: Analyzing file for relatives.\n";
					}


					fstream readStream(entry.path().generic_string());

					if (readStream.is_open())
					{
						if (verboseLevel == VerboseLevel::Double)
						{
							cout << "[PtrScan]: Filestream open successfully. Reading...\n";
						}

						if (dump)
						{
							logDump += "[PtrScan]: Filestream open successfully. Reading...\n";
						}

						tempData = stringstream();

						tempData << readStream.rdbuf();

						string tempStr = tempData.str();

						if (tempStr.find(pattern) != string::npos)
						{
							relative = true;
						}

						readStream.close();
					}
					else
					{
						if (verboseLevel == VerboseLevel::Double)
						{
							cout << "[PtrScan]: Stream failed to open or closed alone, please report this.\n";
						}

						if (dump)
						{
							logDump += "[PtrScan]: Analyzing file for relativesStream failed to open or closed alone, please report this.\n";
						}
					}

					if (relative)
					{
						if (verboseLevel == VerboseLevel::Double)
						{
							cout << "----------------RELATIVE DATA---------------- \n";
							cout << "[PtrScan]: Relative Found.\n";
							cout << "[Relative Info]: File Path: " + entry.path().string() + "\n";
							cout << "[Relative Info]: Match With: " + pattern + "\n";
							if (decompileFile)
							{
								cout << "[Relative Info]: File Content: \n" + tempData.str() + "\n";
							}
							cout << "-------------- END RELATIVE DATA -------------- \n";
						}

						if (dump)
						{
							logDump += "---------------- RELATIVE DATA ---------------- \n";
							logDump += "[PtrScan]: Relative Found.\n";
							logDump += "[Relative Info]: File Path: " + entry.path().string() + "\n";
							logDump += "[Relative Info]: Match With: " + pattern + "\n"; 
							if (decompileFile)
							{
								logDump += "[Relative Info]: File Content: \n" + tempData.str() + "\n";
							}
							logDump += "-------------- END RELATIVE DATA -------------- \n";
						}

						relatives.push_back(entry.path().string());
					}

				}
				else
				{
					if (verboseLevel != VerboseLevel::None)
					{
						cout << "File: " << entry.path() << " size is 0kb, skipping file." << endl;
					}

					if (dump)
					{
						logDump += "File: " + entry.path().generic_string() + " size is 0kb, skipping file.\n";
					}
				}
			}
		}
		else
		{

		}
	}
	else
	{
		cout << "[ERROR] Invalid Arguments" << endl;
		cout << "[PtrScan Usage] -> PtrScan 'path' 'pattern' 'options'" << endl;
		cout << "[PtrScan Options]" << endl;
		cout << "-R -- Recursive" << endl;
		cout << "-O -- Outputs to a file called PtrDump.txt" << endl;
		cout << "-V -- Verbose, prints out on the console the matches" << endl;
		cout << "-P -- Preview, prints out on the console and in the dump file the file contents" << endl;
		cout << "-VV -- Double Verbose, prints out on the console everything, from scanning files to matches." << endl;

		return 1;
	}

	if (verboseLevel != VerboseLevel::None)
	{
		cout << "--------- RELATIVES --------- \n";
		for (string s : relatives)
		{
			cout << s << endl;
		}
	}

	if (dump)
	{
		logDump += "--------- RELATIVES --------- \n";
		for (string s : relatives)
		{
			logDump += s + "\n";
		}
	}



	file << logDump;
	file.close();

	return 0;
}