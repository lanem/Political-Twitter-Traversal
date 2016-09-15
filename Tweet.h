/*
Tweet.h acts as a header-only class to store all the pertinent
information contained within JSON responses regarding tweets and the user that posted them
*/

#ifndef TWEET_H
#define TWEET_H

#include "TwitterUser.h"

#include <iostream>

// Class to contain coordinate information. Used by GeoInfo. lon = x, lat = y
class Coordinates {
private:
	double lon;
	double lat;
public:
	Coordinates();
	Coordinates(double, double); // Overridden constructor which assigns the lat and lon
	// Getters and setters for lon and lat
	void setLon(double lo) { lon = lo; }
	void setLat(double la) { lat = la; }
	double getLon() { return lon; }
	double getLat() { return lat; }
};

// Class to contain geographical information for the tweet
class GeoInfo {
private:
	Coordinates coordinates; // Actual coordinates...will need to triangulate to the center of a box coordinates.
	string place; // Holds the place as a string
	string country; // Holds the country code
public:
	Coordinates getCoordinates() { return coordinates; }
	string getPlace() { return place; }
	string getCountry() { return country; }
	void setCoordinates(Coordinates c) { coordinates = c; }
	void setPlace(string p) { place = p; }
	void setCountry(string c) { country = c; }
};

class Tweet {
private:
	bool geoInfoExists = false; // Used to check if there is geoInfo avaliable
	int numRetweets; // Number retweets it got
	int numFavs; // Number favorites it got
	string idNum; // The id number for the tweet, useful for matching tweets. Stored as string but contains a double.
	string timePosted; // Time the tweet was posted
	string text; // Contains the actual tweet text
	TwitterUser postingUser; // Contains all the info of the posting user

	// Contains all the info for the geographical info. DOES NOT EXIST BY DEFAULT.
	// ALWAYS check hasGeoInfo() BEFORE accessing, as most tweets do not contain geoInfo.
	GeoInfo geoInfo;
public:
	void printInfo(); // Prints most of the tweet info to the console

	// Getters and setters for each variable above
	bool hasGeoInfo() { return geoInfoExists; } // Used to check if geo info exists before accessing
	int getNumRetweets() { return numRetweets; }
	int getNumFavs() { return numFavs; }
	string getIdNum() { return idNum; }
	string getTimePosted() { return timePosted; }
	string getText() { return text; }
	TwitterUser getPostingUser() { return postingUser; }
	GeoInfo getGeoInfo() { return geoInfo; }
	void setGeoInfo(bool gie) { geoInfoExists = gie; }
	void setNumRetweets(int nr) { numRetweets = nr; }
	void setNumFavs(int nf) { numFavs = nf; }
	void setIdNum(string idn) { idNum = idn; }
	void setTimePosted(string tp) { timePosted = tp; }
	void setText(string t) { text = t; }
	void setPostingUser(TwitterUser tu) { postingUser = tu; }
	void setGeoInfo(GeoInfo gi) { geoInfo = gi; }
	int getInfluence();
	int searchForKeyword(string);
	bool isPostedByUsername(string);
	bool isPostedByName(string);
};

#endif