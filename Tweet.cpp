/*
Tweet.cpp
Author: Lane Miller

int getInfluence() - Gives an integer value estimating how influential the tweet was
int seachForKeyword(string) - Searches the tweet for a given keyword and returns if it was found
bool isPostedByUsername(string) - Checks if the tweet was posted by a given username
bool isPostedByName(string) - Checks if the tweet was posted by a given person's name
*/

#include "Tweet.h"

Coordinates::Coordinates() {
	lat = 0;
	lon = 0;
}

Coordinates::Coordinates(double la, double lo) {
	lat = la;
	lon = lo;
}

void Tweet::printInfo() {
	cout << "User Name: " << getPostingUser().getName() << endl;
	cout << "Screen Name: " << getPostingUser().getScreenName() << endl;
	cout << "Tweet ID: " << getIdNum() << endl;
	cout << "Time of Posting: " << getTimePosted() << endl;
	cout << "Tweet Text: " << getText() << endl;
	cout << "Number of Retweets: " << to_string(getNumRetweets()) << endl;
	cout << "Number of Favorites: " << to_string(getNumFavs()) << endl;
	cout << "Number of User Followers: " << to_string(getPostingUser().getNumFollowers()) << endl;

	// Checks if the tweet has geoInfo, and prints it if it does
	if (hasGeoInfo()) {
		cout << "Place: " + getGeoInfo().getPlace() << endl;
		cout << "Longitude: " + to_string(getGeoInfo().getCoordinates().getLon()) << endl;
		cout << "Latitude: " + to_string(getGeoInfo().getCoordinates().getLat()) << endl;
	}
}

/*
int getInfluence()
Author: Lane Miller
Description: Uses an algorithm to give an estimate for how far reaching or powerful the tweet was
Return: An integer value, currently: (3 * retweets) + favs + (user followers / 5),
All multiplied by 1.5 if the user was verified.
*/
int Tweet::getInfluence() {
	int influenceLevel = (numRetweets * 3) + numFavs + (postingUser.getNumFollowers() / 5);
	if (postingUser.isVerified())
		influenceLevel = (int)(influenceLevel * 1.5);
	return influenceLevel;
}

/*
int searchForKeyword(string keyword)
Author: Lane Miller
Description: Searchs the tweet for a given keyword
Input: A string to search the tweet for
Return: An integer value. 0 = not found, 1 = found in tweet text, 2 = found in user's name
*/
int Tweet::searchForKeyword(string keyword) {
	size_t foundText = text.find(keyword);
	if (foundText != string::npos)
		return 1;
	else
		return 0;
}

/*
bool isPostedByUsername(string username)
Author: Lane Miller
Description: Checks if a tweet was posted by a given username
Input: A string containing a username to check if this user posted the tweet
Return: True if they posted the tweet, false if not
*/
bool Tweet::isPostedByUsername(string username) {
	if (postingUser.getScreenName().compare(username))
		return true;
	else
		return false;
}

/*
bool isPostedByName(string name)
Author: Lane Miller
Description: Checks if a tweet was posted by a a given person's name.
This is not quite as exact as username, as multiple users may have the same name,
or a user may have inputted a name that isn't thiers.
Input: A string containing a name to check if this person posted the tweet
Return: True if they posted the tweet, false if not
*/
bool Tweet::isPostedByName(string name) {
	if (postingUser.getName().compare(name))
		return true;
	else
		return false;
}