/*
TwitterUser.h acts as a header-only class to store all the pertinent 
information contained within JSON responses regarding accounts
*/

#ifndef TWITTER_USER_H
#define TWITTER_USER_H

// This is Inner-most include in the project, so we define string and the namespace std. 
// This extends to all other files in project
#include <string>
using namespace std;

// Class to contain information about a given Twitter user
class TwitterUser {
private:
	bool verified; // Holds weather or not the person is a verified (important) person on Twitter
	long numFollowers; // Number of people following the user
	string userId; // User ID. Stored as string but contains a double. Useful for matching users.
	string name; // Real name
	string screenName; // Twitter name
	string location; // Where they say they live (not checked with geo-info of tweets)
	string dateCreated; // Date the account was created
public:
	bool isVerified() { return verified; }
	long getNumFollowers() { return numFollowers; }
	string getUserId() { return userId; }
	string getName() { return name; }
	string getScreenName() { return screenName; }
	string getLocation() { return location; }
	string getDateCreated() { return dateCreated; }
	void setVerified(bool v) { verified = v; }
	void setNumFollowers(long nfs) { numFollowers = nfs; }
	void setUserId(string ui) { userId = ui; }
	void setName(string n) { name = n; }
	void setScreenName(string sn) { screenName = sn; }
	void setLocation(string l) { location = l; }
	void setDateCreated(string dc) { dateCreated = dc; }
};

#endif