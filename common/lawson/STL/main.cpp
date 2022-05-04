#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <string>


using namespace std;

vector<int> promptForNums()
{
	const int RANGE = 100;
	vector<int> theNums;

	cout << "How many numbers to you want?\n";
	int num;
	cin >> num;
	if (num > 0)
	{
		for (int i = 0; i < num; i++)
		{
			int randNum = rand() % RANGE;
			theNums.push_back(randNum);
		}
	}
	return theNums;
}

void reportNums(const vector<int>& nums)
{
	cout << "\n\nThe numbers are:";
	for (unsigned int i = 0; i < nums.size(); i++)
	{
		cout << nums[i] << " ";
	}
	cout << endl;
}

vector<int> sortNums(const vector<int>& nums)
{
	vector<int> newNums(nums);

	sort(newNums.begin(), newNums.end());
	return newNums;
}

vector<int> removeDuplicateNums(const vector<int>& nums)
{
	vector<int> newNums(nums);

	vector<int>::iterator it = unique(newNums.begin(), newNums.end());
	newNums.resize(it - newNums.begin());

	return newNums;
}


map<string, string> loadLanguage(const string& filename)
{
	map<string, string> table;

	ifstream fileStream(filename);
	if (fileStream.good())
	{
		while (!fileStream.eof())
		{
			string key;
			string value;
			getline(fileStream,key);
			getline(fileStream,value);
			if (key != "")
			{
				table[key] = value;
			}
		}
	}
	return table;
}

int main()
{
	vector<int> someNums = promptForNums();
	reportNums(someNums);

	someNums = sortNums(someNums);
	cout << "\n\nSorted:";
	reportNums(someNums);

	someNums = removeDuplicateNums(someNums);
	cout << "\n\nDuplicates removed:";
	reportNums(someNums);

	map<string, string> languageTable;
	
	char choice='e';

	cout << endl << "Which language do you want e - english, g - german\n\n";
	cin >> choice;

	if (choice == 'e' || choice == 'E')
	{
		languageTable = loadLanguage("EnglishText.txt");
	}
	else
	{
		languageTable = loadLanguage("GermanText.txt");
	}

	cout << endl << languageTable["HELLO"] << " and " << languageTable["WELCOME"] << " " << languageTable["GOODDAY"] << " " << languageTable["BYE"] << endl;

	system("pause");
	return 0;
}