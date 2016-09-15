/*
PoliticalTwitterTraversal.cpp 
Author: Lane Miller
Used for the main method of PTT

_tmain() - Main method to run all test methods

dayRun() - Example code of how each day was ran in our data mining
hourRun() - A full demonstration of the program running for a single hour
hourSearch() - Searches and populates various TweetLists for a full hour. Used in hourRun().
populateVector() - Searches and populates two TweetLists for 3 minutes. This is iterated in hourSearch()

void csvifiyAll() - Goes through each candidate and csvifies each day up to NUM_DAYS_RAN
csvifiyCandidateDay(string, string) - Returns a string containing
csvifiyCandidate(string) - Returns a string in csv format containing all info from a day's worth of candidate's tweet info
csvifiyTweetInfo(ifstream&) - Returns a string in csv format for a given hour's tweet info

searchTest() - Asks user for a search term and gives back 100 recent tweets sorted chronologically
searchGeoTest() - Asks user for coordinates, a radius, and a search term to search for geolocalized tweets
keywordSearchTest() - Searches a list of a 500 tweets for our list of keywords and prints info about the found tweets
saveTest() - Asks the user for a search term, and saves 100 recent tweets to a file to be loaded later
loadTest() - Loads from the test file saveTest() creates. *Only to be called after saveTest()*

getUserSearchTerm() - Asks the user a search term and returns it
getUserCoordinates() - Asks the user for coordinates and returns it
*/

#include "PoliticalTwitterTraversal.h"

/*
void int _tmain(int, _TCHAR*[])
Author: Lane Miller
Description: Used to call various test cases and run the program
Return: 0 on success
Input: Unused
*/
int _tmain(int argc, _TCHAR* argv[]) {
	bool cont = true;
	while (cont) {
		cout << "Which case would you like to perform? Enter an integer: \n";
		cout << "0 - Quit \n";
		cout << "1 - Simple Search Test \n";
		cout << "2 - Search Around Coordinates \n";
		cout << "3 - Keyword Search \n";
		cout << "4 - Save and Load a List \n";
		string inputS = "";
		getline(cin, inputS);
		int input = stoi(inputS);
		switch (input) {
		case 0:
			cont = false;
			break;
		case 1:
			searchTest();
			break;
		case 2:
			searchGeoTest();
			break;
		case 3:
			keywordSearchTest();
			break;
		case 4:
			saveTest();
			loadTest();
			break;
		default:
			cout << "Input not recognized, please try again.";
			break;
		}
	}
}

/*
void searchTest()
Author: Lane Miller
Description: Populates a TweetList with 100 tweets with a user definied search term, sorts them by date, and calls printTweetInfo()
to display information about these tweets. A simple demonstration of the program.
*/
void searchTest() {
	TweetList tweets; // Begins a new TweetList and starts it up

	tweets.populate(getUserSearchTerm()); // Populates with 100 tweets and gets the head node
	tweets.sortByTweetID(); // Sorts the tweets into chronological order

	tweets.printTweetInfo(); // Prints out a lot of information about these tweets that were read in as examples.
}

/*
void dayRun()
Author: Lane Miller
Description: Day Run is an example of how we ran each day. This code was restarted with all variables updated each day.
*/
void dayRun() {
	for (int i = 1; i <= LAST_HOUR; i++) {
		hourRun(to_string(i));
	}
}

/*
void hourRun(string hour)
Author: Lane Miller
Description: Each hourRun() essentially creates a new data point. It creates 4 new lists, one for each candidate, and
populates them with 1-3 thousand unique and newly posted tweets over the course of an hour.
Input: A string containing an integer of the current hour being run

NOTE - For a ~12 minute demonstration instead, change MAX_HOUR_RUNS in the header to from 5 to 1.
*/
void hourRun(string hour) {
	cout << "Begin hour: " + hour << endl;
	cout << "Creating first list for new TweetID." << endl;
	// Creates a new list to begin in order to find new recent tweets.
	TweetList loadList;
	loadList.populate("Trump");
	// Then find the most recent tweet read in to begin populating each list with tweets more recent that it
	string startId = loadList.getLargestTweetID();
	vector<string> lastIds = { startId, startId, startId, startId, };

	vector<TweetList*> lists = hourSearch(lastIds); // Begins populating the lists

	// ...one ...hour ...later ...print out to files.
	ofstream outputFile;
	
	// Build the correct file names for the saveToFile() and saveTweetInfoToFile() methods
	for (unsigned int i = 0; i < lists.size(); i++) {
		string fileName = OUTPUT_FOLDER;
		switch (i) {
		case 0: 
			fileName += TRUMP_FOLDER;
			break;
		case 1:
			fileName += CRUZ_FOLDER;
			break;
		case 2: 
			fileName += SANDERS_FOLDER;
			break;
		case 3: 
			fileName += CLINTON_FOLDER;
			break;
		}
		fileName += HOUR + hour;
		for (unsigned int j = 0; j < NUM_COUNTRIES; j++) {
			switch (j) {
			case 0: 
				string tweetFileName = fileName + TWEETS + FILETYPE;
				string infoFileName = fileName + INFO + FILETYPE;

				// Save them to the files for the hour
				outputFile.open(tweetFileName, ios::trunc);
				lists[i]->saveToFile(outputFile);
				outputFile.close();
				outputFile.open(infoFileName, ios::trunc);
				lists[i]->saveTweetInfoToFile(outputFile);
				outputFile.close();
				delete lists[i];
				break;
			}
		}
	}
}

/*
vector<TweetList*> hourSearch(vector<string>)
Author: Lane Miller
Description: Searches and populates various TweetLists over a full hour using each candidates name an equal amount of times.
Input: vector containing 4 tweet id's (strings) of the most recently tweeted tweets in each political candidate's current list.
This allows the program to only read in newly tweeted tweets.
Return: A vector of tweetlists, the first containing trump tweets, second cruz, third sanders, and fourth clinton.

NOTE - For a ~12 minute demonstration instead, change MAX_HOUR_RUNS in the header to from 5 to 1.
*/
vector<TweetList*> hourSearch(vector<string> startIds) {
	bool firstRun = true; // Boolean which says if it is the loops first run
	int numRuns = 0; // Counter for how many 12-minute 
	vector<TweetList*> lists = { NULL, NULL, NULL, NULL }; // Creates the object to return
	time_t start, current; // Time objects to track how long has passed

	time(&start);
	time(&current);
	while (true) {
		// If less than an hour has passed and there haven't been more than 5 12-minute iterations,
		// Then populate the lists for for ~12 more minutes.
		if (difftime(current, start) < ONE_HOUR && numRuns < MAX_HOUR_RUNS) {
			// If it is the first run, we need to push_back the first TweetLists into the vector
			if (firstRun) {
				firstRun = false;
				for (int i = 0; i < NUM_CANDIDATES; i++) {
					TweetList* populatedList = NULL;

					// Each case takes ~3 minutes of searching for the given search term
					switch (i) {
					case 0:
						populatedList = populateList(TRUMP, FULL_TRUMP, startIds[i]);
						break;
					case 1:
						populatedList = populateList(CRUZ, FULL_CRUZ, startIds[i]);
						break;
					case 2:
						populatedList = populateList(SANDERS, FULL_SANDERS, startIds[i]);
						break;
					case 3:
						populatedList = populateList(CLINTON, FULL_CLINTON, startIds[i]);
						break;
					}

					lists[i] = populatedList;
					time(&current);
				}
			}
			// Otherwise we just need to append the tweetlists to one another
			else {
				for (int i = 0; i < NUM_CANDIDATES; i++) {
					TweetList* populatedList = NULL;

					switch (i) {
					case 0:
						populatedList = populateList(TRUMP, FULL_TRUMP, lists[i]->getLargestTweetID());
						lists[i]->append(populatedList);
						break;
					case 1:
						populatedList = populateList(CRUZ, FULL_CRUZ, lists[i]->getLargestTweetID());
						lists[i]->append(populatedList);
						break;
					case 2:
						populatedList = populateList(SANDERS, FULL_SANDERS, lists[i]->getLargestTweetID());
						lists[i]->append(populatedList);
						break;
					case 3:
						populatedList = populateList(CLINTON, FULL_CLINTON, lists[i]->getLargestTweetID());
						lists[i]->append(populatedList);
						break;
					}
					time(&current);
				}
			}
			numRuns++; // Increment the number of runs of the loop
		}
		// Otherwise, the hour is up. Break from the loop to return
		else
			break;
	}
	return lists; // Return the full lists object
}

/*
TweetList* populateList(string, string, string)
Author: Lane Miller
Description: populateList runs for ~3 minutes populating a tweet list with search terms
Input: Two strings to search for, which in this program are the candidates last name and full name.
The last string is a TweetId which represents the oldest tweet to accept searches from. All tweets are more recent than this one.
Return: A tweetlist pointer containing all tweet which were searched for
*/
 TweetList* populateList(string name, string fullName, string startId) {
	cout << "Populating: " + name + ", " + fullName << endl;

	const int MAX_RUNS = 1; // We only want to run once here
	int numRuns = 0; // Counter for the number of runs
	TweetList* list = new TweetList(); // Start up the list
	time_t start, tempStart, current; // Used to track how much time has passed

	string lastId = startId; // Creates a variable to track the most recent id, so we can get only new tweets

	time(&start);
	time(&current);
	while (true) {
		if (difftime(current, start) <= THREE_MINUTES && numRuns < MAX_RUNS) {
			time(&tempStart);

			// Populate 400 tweets each list to begin
			list->populateSince(fullName, lastId);
			list->populateSince(name, lastId);
			list->populateSince(fullName, lastId);
			list->populateSince(name, lastId);

			// Update last id
			if (list->getHead() != NULL)
				lastId = list->getLargestTweetID();

			// At 30 seconds populate again
			while (true) {
				if (difftime(current, tempStart) >= ONE_MINUTE / 2) {
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					break;
				}
				time(&current);
			}

			// Update last id
			if (list->getHead() != NULL)
				lastId = list->getLargestTweetID();

			// And at one minute
			while (true) {
				if (difftime(current, tempStart) >= ONE_MINUTE) {
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					break;
				}
				time(&current);
			}

			// Update last id
			if (list->getHead() != NULL)
				lastId = list->getLargestTweetID();

			// And 1 minute 30 seconds
			while (true) {
				if (difftime(current, tempStart) >= (3 * ONE_MINUTE) / 2) {
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					break;
				}
				time(&current);
			}

			// Update last id
			if (list->getHead() != NULL)
				lastId = list->getLargestTweetID();

			// And 2 minutes, currently skipped to not max out program
			while (true) {
				if (difftime(current, tempStart) >= TWO_MINUTES) {
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					break;
				}
				time(&current);
			}

			// Update last id
			if (list->getHead() != NULL)
				lastId = list->getLargestTweetID();
			
			// And 2 minutes and 30 seconds
			while (true) {
				if (difftime(current, tempStart) >= (5 * ONE_MINUTE) / 2) {
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					break;
				}
				time(&current);
			}

			// Update last id
			if (list->getHead() != NULL)
				lastId = list->getLargestTweetID();

			// And finally at 3 minutes, skipped to not max out program
			while (true) {
				if (difftime(current, tempStart) >= THREE_MINUTES) {
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					list->populateSince(fullName, lastId);
					list->populateSince(name, lastId);
					break;
				}
				time(&current);
			}
			numRuns++; // Increment the number of runs
		}
		// Break after a single run
		else
			break;

		time(&current);
	}
	return list; // Return the vector of TweetLists that were populated
}

/*
void csvifiyAll()
Author: Lane Miller
Description: csvifiyAll creates .csv files for all tweet list files which are created using TweetList's saveTweetInfoToFile()
method. It then saves them in savedlists/csv, one for each candidate.
*/
void csvifiyAll() {
	ofstream output("savedlists/csv/sanders.csv", ios::trunc);
	output << csvifiyCandidate(SANDERS_LOWER);
	output.close();
	output.open("savedlists/csv/clinton.csv", ios::trunc);
	output << csvifiyCandidate(CLINTON_LOWER);
	output.close();
	output.open("savedlists/csv/trump.csv", ios::trunc);
	output << csvifiyCandidate(TRUMP_LOWER);
	output.close();
	output.open("savedlists/csv/cruz.csv", ios::trunc);
	output << csvifiyCandidate(CRUZ_LOWER);
	output.close();
}

/*
string csvifiyCandidate(string)
Author: Lane Miller
Description: csvifiyCandidate creates a csv string containing all of the information for a given candidate
Input: The candidate which is being csvifiyed, in the same format as the folder which their tweet list files are found
Return: A csv string with all info in all tweet list files created
*/
string csvifiyCandidate(string candidate) {
	string csvC = "";
	for (int i = 1; i <= NUM_DAYS_RAN; i++)
		csvC += csvifiyCandidateDay(to_string(i), candidate);
	return csvC;
}

/*
string csvifiyCandidateDay(string, string)
Author: Lane Miller
Description: csvifiyCandidate creates a csv string containing all of the information for a given candidate
Input: A string containing which day is current being csvifiyed, and which candidate.
Return: A csv string with all info in a given days tweet list files
*/
string csvifiyCandidateDay(string day, string candidate) {
	string csvDay = "";
	string fileName;
	ifstream input;
	// Open each hours tweet list file and csvifiy it
	for (int i = 1; i <= LAST_HOUR; i++) {
		fileName = "savedlists/day" + day + "/" + candidate + "/" + "hour" + to_string(i) + "info.txt";
		cout << fileName;
		input.open(fileName);
		csvDay += day + "," + to_string(i + ((stoi(day) - 1) * NUM_HOURS_RAN_PER_DAY)) + ",";
		csvDay += csvifiyTweetInfo(input);
		input.close();
		csvDay += "\n";
	}
	return csvDay;
}

/*
string csvifiyTweetInfo(string)
Author: Lane Miller
Description: Creates a csv string for all the information in a given tweet list file
Input: An ifstream object opened with a tweet list file
Return: A csv string with all info in a given tweet list file
*/
string csvifiyTweetInfo(ifstream& input) {
	string csvInfo = ""; // String to hold all info
	string delim = ":"; // Deliminator, will change in file
	string thisLine = ""; // String to hold the current line in file
	// Begin by getting first 11 lines of general info
	for (int i = 0; i < NUM_GENERAL_LINES; i++) {
		getline(input, thisLine);
		csvInfo += thisLine.substr(thisLine.find(delim) + 2, thisLine.length() - 1) + ",";
	}
	getline(input, thisLine); // Get the empty line
	csvInfo += " ,"; // Empty slot to signal the start of good keywords
	// Then 3 lines of good keyword info
	for (int i = 0; i < NUM_KEYWORD_LINES; i++) {
		getline(input, thisLine);
		csvInfo += thisLine.substr(thisLine.find(delim) + 2, thisLine.length() - 1) + ",";
	}
	delim = " "; // Switch to keyword delim
	getline(input, thisLine); // Get the empty line
	// Get all good keywords
	for (int i = 0; i < NUM_GOOD_KEYWORDS; i++) {
		getline(input, thisLine);
		csvInfo += thisLine.substr(thisLine.find(delim) + 1, thisLine.length() - 1) + ",";
	}
	csvInfo += " ,"; // Empty slot to signal the start of bad keywords
	getline(input, thisLine); // Get the empty line
	delim = ":";
	// Get 3 lines of bad keyword info
	for (int i = 0; i < NUM_KEYWORD_LINES; i++) {
		getline(input, thisLine);
		csvInfo += thisLine.substr(thisLine.find(delim) + 2, thisLine.length() - 1) + ",";
	}
	delim = " ";
	getline(input, thisLine); // Get the empty line
	// Finally get all bad keywords
	for (int i = 0; i < NUM_BAD_KEYWORDS - 1; i++) {
		getline(input, thisLine);
		csvInfo += thisLine.substr(thisLine.find(delim) + 1, thisLine.length() - 1) + ",";
	}
	// Do not add a comma onto the last entry
	getline(input, thisLine);
	csvInfo += thisLine.substr(thisLine.find(delim) + 1, thisLine.length() - 1);
	return csvInfo; // Return all info in csv format
}

/*
void searchGeoTest()
Author: Lane Miller
Description: Populates a TweetList with a user given search term and asks them for
a coordinate and radius to find tweets around. It then calls these tweets' printTweetInfo()
*/
void searchGeoTest() {
	TweetList tweets; // Creates a TweetList object and starts it up

	system("pause"); // Pauses before continuing

	// Asks the user for a radius and stores it
	string radiusString;
	cout << "Enter a radius (int): "; 
	getline(cin, radiusString);
	int radius = stoi(radiusString);

	tweets.populateGeo(getUserSearchTerm(), getUserCoordinates(), radius); // Populates the list with the user defined info

	// Sort and print the info
	tweets.sortByTweetID();
	tweets.printTweetInfo();
}

/*
void keywordSearchTest()
Author: Lane Miller
Description: Populates a TweetList with a user given search term, and then checks
for our predefinied keywords within the tweets. It then prints how many times they
appeared and their printTweetInfo()
*/
void keywordSearchTest() {
	TweetList startList; // Begins a list to look for keywords within and starts it up

	system("pause"); // Pauses before continuing

	// Populates with 500 tweets containing a user given search term
	string searchTerm = getUserSearchTerm();
	for (int i = 0; i < 3; i++)
		startList.populateMixed(searchTerm);
	cout << "Populated.\n";

	// ifstream objects to read in the keywords from the files
	ifstream goodIn("good_keywords.txt");
	ifstream badIn("bad_keywords.txt");
	// vector objects to store tweet objects containing our keywords
	vector<Tweet> badTweets = startList.getBadKeywordOccurances();
	startList.printBadKeywordOccurances();
	vector<Tweet> goodTweets = startList.getGoodKeywordOccurances();
	startList.printGoodKeywordOccurances();

	system("pause"); // Pause before printing tweet info
	
	// Creates new tweet lists from these vectors
	TweetList goodList(goodTweets);
	TweetList badList(badTweets);

	// Print tweet info of the tweet containing the found keywords
	goodList.printTweetInfo();
	cout << endl;
	badList.printTweetInfo();
}

/*
void saveTest()
Author: Lane Miller
Description: Populates a TweetList with a 1000 tweets containing a user defined search term and saves it in
savedlists/testoutput1.txt and its info in testoutput2, clearing the last save. This list can later be loaded with loadTest().
*/
void saveTest() {
	// Opens output files to save to
	ofstream output1("savedlists/testoutput1.txt", ios::trunc);
	ofstream output2("savedlists/testoutput2.txt", ios::trunc);
	TweetList tweets; // Begins a new TweetList and starts it up

	system("pause"); // Pauses before continuing

	string searchTerm = getUserSearchTerm(); // Gets the search term

	for (int i = 0; i < 3; i++)
		tweets.populate(searchTerm); // Populates with 300 tweets and gets the head node
	
	cout << "Populated.\n";
	tweets.sortByTweetID();
	cout << "Sorted.\n";
	tweets.saveToFile(output1);
	tweets.saveTweetInfoToFile(output2);
	cout << "Saved.\n";
}

/*
void loadTest()
Author: Lane Miller
Description: Populates a TweetList with a pre saved list from saveTest() and calls its printTweetInfo()
*/
void loadTest() {
	ifstream input1("savedlists/testoutput1.txt");
	ifstream input2("savedlists/testoutput2.txt");
	TweetList tweets;
	string csv = "";

	tweets.loadFromFile(input1);
	csv = csvifiyTweetInfo(input2);
	tweets.sortByTweetID();
	tweets.printTweetInfo();
	cout << csv << endl;
}

/*
string getUserSearchTerm()
Author: Lane Miller
Description: Asks the user for a search term to populate a TweetList with and returns it
Return: A string containing the user's desired search term
NOTE: Assumes user only enters proper one line strings
*/
string getUserSearchTerm() {
	string term;
	cout << "Please enter a search term (string): ";
	getline(cin, term);
	return term;
}

/*
string getUserCoordinates()
Author: Lane Miller
Description: Asks the user for a search term to populate a TweetList as well as coordinates
to supply for the populateGeo() method which gets tweets only around a certain radius.
Return: The Coordinate object containing the user definied coordinate point
NOTE: Assumes user only enters proper doubles
*/
Coordinates getUserCoordinates() {
	string lat;
	string lon;
	cout << "Enter a latitude (double): ";
	getline(cin, lat);
	cout << "Enter a longitude (double): ";
	getline(cin, lon);
	Coordinates coords = *new Coordinates(stod(lat), stod(lon));
	return coords;
}