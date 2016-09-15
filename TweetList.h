// TweetList.h outlines the TweetList class
#ifndef TWEET_LIST_H
#define TWEET_LIST_H

#include <fstream> // To read/write to files.

// Included to convert between wstring and string
#include <codecvt>
#include <locale>

#include "LinkedList.h" // Includes the parent linked list class
#include "libtwitcurl\twitcurl.h" // Includes the Twitter library
#include "simplejson\JSON.h" // Includes the JSON parser

// TweetList class extends the LinkedList class using Tweets as a template
class TweetList: public LinkedList<Tweet> {
private:
	// Constants for the Twitter account. These can be found by visiting Twitter
	const string USERNAME = "Twit_Traversal";
	const string PASSWORD = "Goodard";
	const string CONSUMER_KEY = "FrDnVVHlp3KY3fOgYiWk8Q3QA";
	const string CONSUMER_SECRET = "jD5zZFwyNQ20b5N27oR7RxpAL84FLJH6ti6DULB1ivnMCPKB0p";

	// Loads the TwitCurl object to be used with the username, password, consumer key, and consumer secret defined above.
	// Called by constructors so shouldn't need to be called by the user.
	void startUp(); 
	void getAndSetNewOAuth(); // Used by the startUp if new OAuth needs to be retrieved

	static int numTweetsInAllObjects; // Total number tweets in all TweetList objects
	static int numTweetsReadInAllObjects; // Total number of tweets read in, including all retweets and duplicates
	static double timeTwitterWorkedForUs; // Time taken for the Twitter servers to gather our responses
	int numTweets; //  Total number tweets in the given TweetList object
	int numTweetsReadIn; // Total number of tweets read in, including all retweets, duplicates, and non-english tweets

	// Methods used to sort the list
	void mergeSort(Node<Tweet>**); // Recursive method for the merge sort
	Node<Tweet>* sortedMerge(Node<Tweet>*, Node<Tweet>*); // Helper method for the merge sort
	void frontBackSplit(Node<Tweet>*, Node<Tweet>**, Node<Tweet>**); // Helper method for the merge sort

	// Methods used by the public populate() methods:
	Node<Tweet>* beginParse(string); // Takes a search from the Twit Curl api to begin parsing
	void insert(Tweet); // Inserts a tweet at the start of the list
	Tweet buildTweet(JSONObject); // Takes a JSON version of a tweet and converts it into a Tweet object
	TwitterUser buildUser(JSONObject); // Takes a JSON version of user information and converts it into a TwitterUser object
	GeoInfo buildGeoInfo(JSONObject); // Takes a JSON version of geography information and converts it to a GeoInfo object
	Coordinates findCoordinate(Coordinates, Coordinates, Coordinates, Coordinates); // Finds the middle of 4 coordinates making a box
	string ws2s(const wstring&); // Converts a wide string to a string. Useful with our JSON parser

	// Methods used by the public saveToFile() method
	wstring s2ws(const string&); // Converts a string to a wide string.
	JSONObject writeTweet(Tweet); // writeTweet converts Tweet information into JSON format
	JSONObject writeUser(TwitterUser); // writeUser converts TwitterUser information into JSON format
	JSONObject writeGeoInfo(GeoInfo); // writeGeoInfo converts GeoInfo information into JSON format
	JSONObject writeMetadata(); // writeMetaData converts the metadata for the list into JSON format

public:
	twitCurl twitObj; // Object for twitter searches

	TweetList(); // Overridden default constructor that also starts the twitCurl object.
	TweetList(ifstream&); // Overridden constructor which takes a file to load a tweet list from
	TweetList(vector<Tweet>); // Overridden constructor which takes a vector of tweets to start the list from

	Node<Tweet>* append(TweetList*); // Inserts another TweetList into the list, checking for duplicates and updating counters
	
	void saveToFile(ofstream&); // Saves the current list's information in a text file to be loaded later.
	void saveTweetInfoToFile(ofstream&); // Saves information we will be tracking to a file
	Node<Tweet>* loadFromFile(ifstream&); // Re-loads a list from a file that is pre-formatted with information from saveToFile()

	Node<Tweet>* populate(string); // Populate begins to add recent tweets (100 at a time minus duplicates) with your given search term.
	Node<Tweet>* populateSince(string, string); // PopulateSince adds tweets made only after the given tweet id
	Node<Tweet>* populateMixed(string); // PopulateMixed adds mixed recent and popular Engligh tweets
	Node<Tweet>* populatePopular(string); // PopulatePopular adds 15 popular English tweets
	Node<Tweet>* populateGeo(string, Coordinates, int); // PopulateGeo adds 100 tweets from a given coordinate and radius
	Node<Tweet>* populateGeoSince(string, Coordinates, int, string); // PopulateGeoSince adds 100 tweets after the given tweet Id

	Node<Tweet>* contains(string); // Takes a tweet id (a number, but method takes it as a string) a returns the node if found, null if not

	vector<Tweet> listToVector(); // Converts the list to a vector and returns it
	vector<Tweet> getKeywordOccurances(string); // Returns a list of the tweets with the keywords found
	vector<Tweet> getGoodKeywordOccurances(); // Returns a list of all tweets found with our own keywords
	vector<Tweet> getBadKeywordOccurances(); // Returns a list of all tweets found with our own keywords
	vector<Tweet> getTweetsInCountry(string); // Returns a list of tweets from a given state code

	Node<Tweet>* sortByTweetID(); // Sorts the list by tweetID and returns the new head to the node
	string getLargestTweetID(); // Returns a string containing the largest tweet id in the list

	void printTweetInfo(); // Prints out a variety of information for each tweet in the list
	void printGoodKeywordOccurances(); // Prints out information of tweets with good keywords in them
	void printBadKeywordOccurances(); // Prints out information of tweets with bad keywords int hem

	// Getters for each class variable. Setters not needed.
	double getTimeTwitterWorkedForUs() { return timeTwitterWorkedForUs; }
	int getNumTweets() { return numTweets; }
	int getNumTweetsReadIn() { return numTweetsReadIn; }
	int getNumTweetsInAllObjects() { return numTweetsInAllObjects; }
	int getNumTweetsReadInAllObjects() { return numTweetsReadInAllObjects; }

	// Other getters
	int getTotalInfluence();
	int getTotalNumRetweets();
	int getTotalNumFavs();
};

#endif