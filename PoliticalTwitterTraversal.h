/*
PoliticalTwitterTraversal.h
Author: Lane Miller
Used for the main method of PTT, and tests many of it's functions.

dayRun() - Example code of how each day was ran in our data mining
hourRun() - A full demonstration of the program running for a single hour
hourSearch() - Searches and populates various TweetLists for a full hour. Used in hourRun().
populateVector() - Searches and populates two TweetLists for 3 minutes. This is iterated in hourSearch()

csvifiyAll() - Saves all csv files for all candidates and all days.
csvifiyCandidate(string) - Creates csv's for all days of a candidate
csvifiyCandidateDay(string, string) - Returns a string in csv format containing all info from a day's worth of candidate's tweet info 
csvifiyTweetInfo(ifstream&) - Returns a string in csv format for a given hour's tweet info

searchTest() - Asks user for a search term and gives back 100 recent tweets sorted chronologically
searchGeoTest() - Asks user for coordinates, a radius, and a search term to search for geolocalized tweets
keywordSearchTest() - Searches a list of a 500 tweets for our list of keywords and prints info about the found tweets
saveTest() - Asks the user for a search term, and saves 100 recent tweets to a file to be loaded later
loadTest() - Loads from the test files saveTest() creates. *Only to be called after saveTest()*

getUserSearchTerm() - Asks the user a search term and returns it
getUserCoordinates() - Asks the user for coordinates and returns it
*/

#include "TweetList.h" // Includes the class for the tweet list *AS WELL AS* the API and JSON parser
#include "stdafx.h" // Includes info for the WIN32 application
#include <cstdio> // Included for the TwitCurl library
#include <time.h> // For measuring time

// ***Constant which needs to be updated daily***
const string OUTPUT_FOLDER = "savedlists/day1";

// Constants for building our file names
const string TRUMP_FOLDER = "/trump";
const string CRUZ_FOLDER = "/cruz";
const string SANDERS_FOLDER = "/sanders";
const string CLINTON_FOLDER = "/clinton";
const string HOUR = "/hour";
const string WORLD = "W";
const string TWEETS = "tweets";
const string INFO = "info";
const string FILETYPE = ".txt";

// Time constants, in seconds
const double ONE_MINUTE = 60;
const double TWO_MINUTES = 60 * 2;
const double THREE_MINUTES = 60 * 3;
const double ONE_HOUR = 60 * 60;

// Candidate name constants
const string TRUMP = "Trump";
const string FULL_TRUMP = "Donald Trump";
const string TRUMP_LOWER = "trump";
const string CRUZ = "Cruz";
const string FULL_CRUZ = "Ted Cruz";
const string CRUZ_LOWER = "cruz";
const string CLINTON = "Clinton";
const string FULL_CLINTON = "Hillary Clinton";
const string CLINTON_LOWER = "clinton";
const string SANDERS = "Sanders";
const string FULL_SANDERS = "Bernie Sanders";
const string SANDERS_LOWER = "sanders";

// Constants for parsing tweet info files
const int NUM_GENERAL_LINES = 11;
const int NUM_KEYWORD_LINES = 3;
const int NUM_GOOD_KEYWORDS = 20;
const int NUM_BAD_KEYWORDS = 20;
const int NUM_DAYS_RAN = 7;
const int NUM_HOURS_RAN_PER_DAY = 20;

// Other constants
const int NUM_COUNTRIES = 1;
const int NUM_CANDIDATES = 4;
const int MAX_HOUR_RUNS = 5;
const int LAST_HOUR = 20;

// Methods for populating lists with tweets over a period of time
void dayRun(); // Example code of how we ran each day
void hourRun(string hour); // An hour long example of the project. This is iterated to create data points.
vector<TweetList*> hourSearch(vector<string>); // Used by hourRun
TweetList* populateList(string, string, string); // used by hourSearch

// Methods to convert tweet list files into csv's
void csvifiyAll(); // Converts all tweet list files into a single csv file for each candidate
string csvifiyCandidate(string); // Converts all of a single candidates tweet list files into a single csv string
string csvifiyCandidateDay(string, string); // Converts a single days worth of a candidates tweet list files into csv strings
string csvifiyTweetInfo(ifstream&); // Converts a single tweet list file into a csv string

// Test methods to see basic functionality of the program
void searchTest(); // Basic test which shows the info for 100 tweets
void searchGeoTest(); // Basic test which asks for coordinates and shows the info for 100 tweets within a radius of that coordinate
void keywordSearchTest(); // Basic test for finding keywords within the tweets
void saveTest(); // Basic test for saving lists
void loadTest(); // Basic test for loading lists

// Methods to ask the user for search terms
string getUserSearchTerm(); // Asks the user for a search term and returns it
Coordinates getUserCoordinates(); // Asks a user for a latitude and longitude and returns it