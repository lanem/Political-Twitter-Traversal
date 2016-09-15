/* TweetList.cpp contains the bulk of our project. It incorperates the API and the JSON parser.
*/
#include "TweetList.h"

// Includes our static variables
int TweetList::numTweetsInAllObjects;
int TweetList::numTweetsReadInAllObjects;
double TweetList::timeTwitterWorkedForUs;

// Overridden deafult constructor that starts the twitCurl object
TweetList::TweetList() {
	numTweets = 0;
	numTweetsReadIn = 0;
	startUp();
}

// Constructor which loads a saved list
TweetList::TweetList(ifstream &input) {
	numTweets = 0;
	numTweetsReadIn = 0;
	loadFromFile(input);
	startUp();
}

// Constructor which loads a list from a vector of tweets
TweetList::TweetList(vector<Tweet> tweets) {
	numTweets = 0;
	numTweetsReadIn = 0;
	for (unsigned int i = 0; i < tweets.size(); i++)
		insert(tweets[i]);
	startUp();
}

// startUp() runs through all oAuth nessecary for the twitCurl object to work.
// This is done on declaration and shouldn't need to be called by the user
void TweetList::startUp() {
	// These constants are defined in the header class and are for the group twitter account
	// Sets the username and password for the twitter object
	twitObj.setTwitterUsername(USERNAME);
	twitObj.setTwitterPassword(PASSWORD);

	// Sets the oAuth keys. These are directly related to our twitter account.
	twitObj.getOAuth().setConsumerKey(CONSUMER_KEY);
	twitObj.getOAuth().setConsumerSecret(CONSUMER_SECRET);

	// Begin looking for saved oAuth tokens. These change whenever you want to log in to a new account.
	// They will only need to be set once for us as we use one account, so they are saved in the following file.
	// This file should always open and should therefore skip the process of getting new oAuth tokens.
	// It's still included however incase the files get removed or if the program breaks for whatever reason.
	ifstream tokenKeyIn("token_key.txt");
	ifstream tokenSecretIn("token_secret.txt");
	string tokenKey;
	string tokenSecret;

	// If the files opened, read in the keys
	if (tokenKeyIn && tokenSecretIn) {
		cout << "Token key and secret files found.\n";
		getline(tokenKeyIn, tokenKey);
		getline(tokenSecretIn, tokenSecret);
		// If keys read in correctly, set them in the oAuth stream
		if (tokenSecret.size() > 0 && tokenKey.size() > 0) {
			cout << "Token key and secret found.\n";
			twitObj.getOAuth().setOAuthTokenKey(tokenKey);
			twitObj.getOAuth().setOAuthTokenSecret(tokenSecret);
			cout << "Set oAuth Tokens: \n" + tokenKey + "\n" + tokenSecret + "\n";
		}
		// Otherwise something went wrong and tokens need to be recreated
		else {
			cout << "Token key and secret not found. Creating new ones...\n";
			getAndSetNewOAuth();
		}
	}
	else {
		cout << "No oAuth files found. Creating new ones...\n";
		getAndSetNewOAuth();
	}

	// End of the oAuth stream to authenticate Twitter information
	// Verify all credientials are good and twitObj is ready to use
	// ReplyMsg will contain info about the Twit_Traversal account if successful and and error message if not
	string replyMsg;

	if (twitObj.accountVerifyCredGet()) {
		twitObj.getLastWebResponse(replyMsg);
		cout << replyMsg << endl;
		cout << "Ready to use!\n";
	}
}

// getAndSetNewOAuth() walks the user through creating new oAuth tokens, and saves them in the correct files
void TweetList::getAndSetNewOAuth() {
	string authUrl; // Creates string to hold the URL to visit in order to authenticate the account
	string tokenKey; // Variable to hold the key once found and before they are put into files
	string tokenSecret; // Variable to hold the secret once found and before they are put into files

	twitObj.oAuthRequestToken(authUrl); // Sets the authentication URL to visit and stores it in authUrl

	// Asks user to authenticate program by visiting the given URL
	string tempStr;
	cout << "Go to this link for PIN:\n" + authUrl + "\nEnter Pin Here:";
	cin >> tempStr;

	twitObj.getOAuth().setOAuthPin(tempStr); // Sets the PIN with the user-returned value

	twitObj.oAuthAccessToken(); // Creates new tokens for the object

	// Gets the newly created tokens
	twitObj.getOAuth().getOAuthTokenKey(tokenKey);
	twitObj.getOAuth().getOAuthTokenSecret(tokenSecret);

	// Then stores these tokens in files for future use (*course objective*)
	ofstream oAuthTokenKeyOut("token_key.txt", ios::trunc);
	ofstream oAuthTokenSecretOut("token_secret.txt", ios::trunc);

	oAuthTokenKeyOut << tokenKey;
	oAuthTokenSecretOut << tokenSecret;

	oAuthTokenKeyOut.close();
	oAuthTokenSecretOut.close();

	startUp(); // Re-try start-up now with the newly saved keys.
}

// insert() inserts a tweet at the beginning of the list
void TweetList::insert(Tweet t) {
	Node<Tweet>* newTweet = new Node<Tweet>(t);

	// Do not add duplicates or empty tweets, but update their counters
	if (contains(t.getIdNum()) != NULL || !t.getIdNum().size()) {
		numTweetsReadInAllObjects++;
		numTweetsReadIn++;
		delete (newTweet);
		return;
	}

	// If list is empty, add head to the top
	if (head == NULL) {
		head = newTweet;
	}

	// Otherwise add node to the start of the list
	else {
		newTweet->next = head;
		head = newTweet;
	}

	// Update all counters
	numTweets++;
	numTweetsReadIn++;
	numTweetsInAllObjects++;
	numTweetsReadInAllObjects++;
}

Node<Tweet>* TweetList::append(TweetList *list) {
	Node<Tweet>* node = list->getHead();
	while (node != NULL) {
		insert(node->tweet);
		node = node->next;
	}
	numTweetsReadIn += (list->getNumTweetsReadIn() - list->getNumTweets());
	numTweetsReadInAllObjects += (list->getNumTweetsReadIn() - list->getNumTweets());
	return head;
}

Node<Tweet>* TweetList::contains(string tweetId) {
	Node<Tweet>* currentNode = head;

	// If the list is empty,
	if (currentNode == NULL)
		return NULL;

	while (currentNode != NULL) {
		if (tweetId.compare(currentNode->tweet.getIdNum()) == 0)
			return currentNode;
		currentNode = currentNode->next;
	}

	return NULL;
}

// populate() begins to add tweets to the list by getting JSON responses from a TwitCurl object and parsing them
// Returns the head node on success and NULL on failure
Node<Tweet>* TweetList::populate(string searchTerm) {
	// Reads in a JSON response to begin parsing containing 100 tweets with the given search term
	string preToParse;
	twitObj.searchDefault(searchTerm);
	twitObj.getLastWebResponse(preToParse);

	return beginParse(preToParse);
}

// populate() begins to add tweets to the list by getting JSON responses from a TwitCurl object and parsing them
// Returns the head node on success and NULL on failure
Node<Tweet>* TweetList::populateSince(string searchTerm, string tweetId) {
	// Reads in a JSON response to begin parsing containing 100 tweets with the given search term
	string preToParse;
	twitObj.searchSince(searchTerm, tweetId);
	twitObj.getLastWebResponse(preToParse);

	return beginParse(preToParse);
}

// populateMixed() begins to add mixed tweets to the list by getting JSON responses from a TwitCurl object and parsing them
// Returns the head node on success and NULL on failure
Node<Tweet>* TweetList::populateMixed(string searchTerm) {
	// Reads in a JSON response to begin parsing containing 100 tweets with the given search term
	string preToParse;
	twitObj.searchMixed(searchTerm);
	twitObj.getLastWebResponse(preToParse);

	return beginParse(preToParse);
}

// populatePopular() begins to add popular tweets to the list by getting JSON responses from a TwitCurl object and parsing them
// Returns the head node on success and NULL on failure
Node<Tweet>* TweetList::populatePopular(string searchTerm) {
	// Reads in a JSON response to begin parsing containing 100 tweets with the given search term
	string preToParse;
	twitObj.searchPopular(searchTerm);
	twitObj.getLastWebResponse(preToParse);

	return beginParse(preToParse);
}

Node<Tweet>* TweetList::populateGeo(string searchTerm, Coordinates coords, int radius) {
	string preToParse;
	twitObj.searchGeo(searchTerm, coords.getLat(), coords.getLon(), radius);
	twitObj.getLastWebResponse(preToParse);

	return beginParse(preToParse);
}

Node<Tweet>* TweetList::populateGeoSince(string searchTerm, Coordinates coords, int radius, string tweetId) {
	string preToParse;
	twitObj.searchGeoSince(searchTerm, coords.getLat(), coords.getLon(), radius, tweetId);
	twitObj.getLastWebResponse(preToParse);

	return beginParse(preToParse);
}

Node<Tweet>* TweetList::beginParse(string preToParse) {
	// Our JSON parser wants a char* to parse, so this converts our JSON string to one
	wstring theParse = s2ws(preToParse);
	const wchar_t* toParse = theParse.c_str();

	// Read in the data and check if something went wrong
	JSONValue *parse = JSON::Parse(toParse);

	// Check to see if reading in the parse failed, and abort if it did.
	if (parse == NULL) {
		printf("Populate failed: Parsing failed\n");
		return NULL;
	}
	// Checks to see if the parse is in valid format and is an actual object
	if (parse->IsObject() == false) {
		printf("Populate failed: The root is not an object\n");
		return NULL;
	}

	JSONObject root = parse->AsObject(); // Finally reads in all of the data and tweets if all is successful.

	// Looks for the word "statuses" which marks the beginning of tweet information and checks to make sure it is correctly formatted
	if (root.find(L"statuses") != root.end() && root[L"statuses"]->IsArray()) {
		JSONArray arrayOfTweets = root[L"statuses"]->AsArray(); // Stores our tweets in JSON form as a vector.

		// We can now start extracting the information from each tweet and putting it in the list
		for (unsigned int i = 0; i < arrayOfTweets.size(); i++) {
			// Check to make sure each tweet is formatted correctly
			if (arrayOfTweets[i]->IsObject())
				insert(buildTweet(arrayOfTweets[i]->AsObject()));
			else
				cout << "Tweet not formatted correctly. Tweet not read in.";
		}
		// Also finds the metadata for the search to see how long it took
		if (root.find(L"search_metadata") != root.end() && root[L"search_metadata"]->IsObject()) {
			JSONObject metadata = root[L"search_metadata"]->AsObject();
			// Then finds the time the search was completed in as adds it to timeTwitterWorkedForUs
			if (metadata.find(L"completed_in") != metadata.end() && metadata[L"completed_in"]->IsNumber())
				timeTwitterWorkedForUs += metadata[L"completed_in"]->AsNumber();
			else
				cout << "Unable to find the time the search was completed in.";

			if (metadata.find(L"tweets_read_in") != metadata.end() && metadata[L"tweets_read_in"]->IsNumber()) {
				numTweetsReadIn = (int) metadata[L"tweets_read_in"]->AsNumber();
				numTweetsReadInAllObjects += (numTweetsReadIn - numTweets);
			}
		}
		else
			cout << "Unable to find seach metadata.";
	}
	// Otherwise the poulate failed if we couldn't find statuses, print out an error message
	else {
		cout << "Populate failed! Did not find statuses within response\n";
		return NULL;
	}
	
	delete parse;
	return getHead(); // Return the head node to the list
}

// buildTweet converts JSON responses containing tweet information into a Tweet object
// Goes in order of the information for effeciency of the parser
Tweet TweetList::buildTweet(JSONObject info) {
	Tweet tweet; // Creates a new tweet object to start building

	// Finds what language the tweet was written in and saves it in lang
	string lang;
	if (info.find(L"lang") != info.end() && info[L"lang"]->IsString()) {
		lang = ws2s(info[L"lang"]->AsString());
	}

	// If the language isn't english, skip the tweet. Will still parse if language not found- needed for save method.
	// This will return an empty tweet object, which will get marked in insert() and not be inserted.
	if (lang != "en" && lang.length() != 0)
		cout << "Non-English tweet skipped.\n"; // May comment this out to speed up parsing.
	else {
		// First checks to see if the given tweet is a retweet. This is true if retweeted_status exists as an object
		// If so, we want to read in the original tweet, so read in this tweet and retry
		if (info.find(L"retweeted_status") != info.end() && info[L"retweeted_status"]->IsObject())
			return buildTweet(info[L"retweeted_status"]->AsObject()); // Retry with the found retweeted tweet *(course objective)*

		// Finds and set when the tweet was posted if found and is in correct format
		if (info.find(L"created_at") != info.end() && info[L"created_at"]->IsString()) {
			wstring dateCreated = info[L"created_at"]->AsString();
			tweet.setTimePosted(ws2s(dateCreated)); // Converts the wstring to string and stores it in the tweet object
		}
		else
			cout << "Unable to find time tweet was posted. Unknown tweet id.\n";

		// Finds and sets the id of the tweet if found and is in correct format
		if (info.find(L"id_str") != info.end() && info[L"id_str"]->IsString()) {
			wstring idNum = info[L"id_str"]->AsString();
			tweet.setIdNum(ws2s(idNum)); // Converts the wstring to a double and stores it in the tweet object
		}
		else
			cout << "Unable to find tweet id. Unknown tweet id.\n";

		// Finds and sets the text of the tweet if found and is in correct format
		if (info.find(L"text") != info.end() && info[L"text"]->IsString()) {
			wstring text = info[L"text"]->AsString();
			tweet.setText(ws2s(text));
		}
		else
			cout << "Unable to find tweet text. Tweet id: " + tweet.getIdNum() << endl;

		// Checks to find the user field and if it is valid. Then passes this object off to buildUser() to continue finding info for the user
		if (info.find(L"user") != info.end() && info[L"user"]->IsObject()) {
			JSONObject userInfo = info[L"user"]->AsObject();
			tweet.setPostingUser(buildUser(userInfo)); // Sets the tweet's positingUser to the TwitterUser object buildUser() returns
		}
		else {
			cout << "Couldn't find user information. Tweet id: " + tweet.getIdNum() << endl;
		}

		// Checks to find if the is geoInfo associated with the tweet. This code will be skipped if no geoInfo is included (normal- no error msg).
		if (info.find(L"place") != info.end() && info[L"place"]->IsObject()) {
			JSONObject place = info[L"place"]->AsObject();
			tweet.setGeoInfo(buildGeoInfo(place));
			tweet.setGeoInfo(true); // Marks the tweet as having geoInfo
		}

		// Finds and sets the retweet count of the tweet if found and is in correct format
		if (info.find(L"retweet_count") != info.end() && info[L"retweet_count"]->IsNumber()) {
			int rtnum = (int)info[L"retweet_count"]->AsNumber(); // Casted to an int as it starts as a double
			tweet.setNumRetweets(rtnum);
		}
		else
			cout << "Unable to find number of retweets. Tweet id: " + tweet.getIdNum() << endl;

		// Finds and sets the favorite count of the tweet if found and is in correct format
		if (info.find(L"favorite_count") != info.end() && info[L"favorite_count"]->IsNumber()) {
			int favnum = (int)info[L"favorite_count"]->AsNumber(); // Casted to an int as it starts as a double
			tweet.setNumFavs(favnum);
		}
		else
			cout << "Unable to find number of favorites. Tweet id: " + tweet.getIdNum() << endl;
	}

	return tweet;
}

TwitterUser TweetList::buildUser(JSONObject userInfo) {
	TwitterUser user;

	// Finds and sets the user id of the TwitterUser
	if (userInfo.find(L"id_str") != userInfo.end() && userInfo[L"id_str"]->IsString()) {
		string userId = ws2s(userInfo[L"id_str"]->AsString());
		user.setUserId(userId);
	}
	else
		cout << "Unable to find user id for Twitter user. Unknown id." << endl;

	// Finds and sets the self-declared name of the TwitterUser
	if (userInfo.find(L"name") != userInfo.end() && userInfo[L"name"]->IsString()) {
		string name = ws2s(userInfo[L"name"]->AsString());
		user.setName(name);
	}
	else
		cout << "Unable to find name for Twitter user. User id: " + user.getUserId() << endl;

	// Finds and sets the self-declared location of the TwitterUser
	if (userInfo.find(L"location") != userInfo.end() && userInfo[L"location"]->IsString()) {
		string loc = ws2s(userInfo[L"location"]->AsString());
		user.setLocation(loc);
	}
	else
		cout << "Unable to find location for Twitter user. User id: " + user.getUserId() << endl;

	// Finds and sets the screen name of the TwitterUser
	if (userInfo.find(L"screen_name") != userInfo.end() && userInfo[L"screen_name"]->IsString()) {
		string sname = ws2s(userInfo[L"screen_name"]->AsString());
		user.setScreenName(sname);
	}
	else
		cout << "Unable to find screen name for Twitter user. User id: " + user.getUserId() << endl;

	// Description of user was left out to save memory
	/*
	// Finds and sets the description (biography) of the TwitterUser
	if (userInfo.find(L"description") != userInfo.end() && userInfo[L"description"]->IsString()) {
		string bio = ws2s(userInfo[L"description"]->AsString());
		user.setDescrip(bio);
	}
	else
		cout << "Unable to find screen name for Twitter user. User id: " + user.getUserId() << endl;
	*/

	// Finds and sets the number of followers of the TwitterUser
	if (userInfo.find(L"followers_count") != userInfo.end() && userInfo[L"followers_count"]->IsNumber()) {
		long numFllwrs = (long) userInfo[L"followers_count"]->AsNumber();
		user.setNumFollowers(numFllwrs);
	}
	else
		cout << "Unable to find number of followers for Twitter user. User id: " + user.getUserId() << endl;

	// Finds and sets the time of creation of the TwitterUser
	if (userInfo.find(L"created_at") != userInfo.end() && userInfo[L"created_at"]->IsString()) {
		string timeCreated = ws2s(userInfo[L"created_at"]->AsString());
		user.setDateCreated(timeCreated);
	}
	else
		cout << "Unable to find date created for Twitter user. User id: " + user.getUserId() << endl;

	// Finds and sets if the TwitterUser is verified
	if (userInfo.find(L"verified") != userInfo.end() && userInfo[L"verified"]->IsBool()) {
		bool ver = userInfo[L"verified"]->AsBool();
		user.setVerified(ver);
	}
	else
		cout << "Unable to find verified status for Twitter user. User id: " + user.getUserId() << endl;

	return user;
}

GeoInfo TweetList::buildGeoInfo(JSONObject placeInfo) {
	GeoInfo geo;
	Coordinates coords;
	bool coordsFound = false;

	// Finds and sets the full place name of the tweet if found and is in correct format
	if (placeInfo.find(L"country_code") != placeInfo.end() && placeInfo[L"country_code"]->IsString()) {
		string place = ws2s(placeInfo[L"country_code"]->AsString());
		geo.setCountry(place);
	}
	else
		cout << "Unable to find country name for tweet. Unknown id." << endl;

	// Finds and sets the full place name of the tweet if found and is in correct format
	if (placeInfo.find(L"full_name") != placeInfo.end() && placeInfo[L"full_name"]->IsString()) {
		string place = ws2s(placeInfo[L"full_name"]->AsString());
		geo.setPlace(place);
	}
	else
		cout << "Unable to find place name for tweet. Unknown id." << endl;

	// Checks to see if list was previously saved
	// If so, coordinates are readily avilable, get lat first
	if (placeInfo.find(L"lat") != placeInfo.end() && placeInfo[L"lat"]->IsNumber()) {
		coords.setLat(placeInfo[L"lat"]->AsNumber());
		coordsFound = true;
	}
	else
		coordsFound = false;
	// And then lon
	if (placeInfo.find(L"lon") != placeInfo.end() && placeInfo[L"lon"]->IsNumber()) {
		coords.setLon(placeInfo[L"lon"]->AsNumber());
		coordsFound = true;
	}
	else
		coordsFound = false;

	// If the coordinates did not get read in, the tweet was not previously saved
	if (!coordsFound) {
		Coordinates coords[4]; // Will store each of the 4 coordinates Twitter gives
		// First find the bounding bow that contains coordinate metadata
		if (placeInfo.find(L"bounding_box") != placeInfo.end() && placeInfo[L"bounding_box"]->IsObject()) {
			JSONObject coordinatesContainer = placeInfo[L"bounding_box"]->AsObject();
			// For whatever reason the coordinates are stored in an array within an array...run the checks
			if (coordinatesContainer.find(L"coordinates") != coordinatesContainer.end() && coordinatesContainer[L"coordinates"]->IsArray()) {
				JSONArray coordinateBoxOuter = coordinatesContainer[L"coordinates"]->AsArray();
				// Actually finds the coordinate array here
				if (coordinateBoxOuter[0]->IsArray()) {
					JSONArray coordinateBox = coordinateBoxOuter[0]->AsArray();
					// Everything went well. Start reading in each coordinate and storing it in coords
					for (unsigned int curCoord = 0; curCoord < coordinateBox.size(); curCoord++) {
						if (coordinateBox[curCoord]->IsArray()) {
							JSONArray coordinate = coordinateBox[curCoord]->AsArray(); // Gets the array containing the actual lat and lon
							// First number is always lon
							if (coordinate[0]->IsNumber()) {
								coords[curCoord].setLon(coordinate[0]->AsNumber());
							}
							// Second is always lat
							if (coordinate[1]->IsNumber()) {
								coords[curCoord].setLat(coordinate[1]->AsNumber());
							}
						}
						// Print out error messages if something broke
						else
							cout << "Unable to find tweet coordinate. Unknown id." << endl;
					}
				}
				else
					cout << "Unable to find array of tweet coordinates. Unknown id." << endl;
			}
			else
				cout << "Unable to find outer array for tweet coordinates. Unknown id." << endl;
		}
		else
			cout << "Unable to find bounding box for tweet coordinates. Unknown id." << endl;

		geo.setCoordinates(findCoordinate(coords[0], coords[1], coords[2], coords[3])); // Find the middle of the coordinates
	}
	else
		geo.setCoordinates(coords);

	return geo;
}

Coordinates TweetList::findCoordinate(Coordinates c1, Coordinates c2, Coordinates c3, Coordinates c4) {
	Coordinates coord;

	// Finds the middle longitude value
	// Does this by finding a mis-matching pair of longitudes and using midpoint formula
	if (c1.getLon() != c2.getLon())
		coord.setLon((c1.getLon() + c2.getLon()) / 2);
	else if (c1.getLon() != c3.getLon())
		coord.setLon((c1.getLon() + c3.getLon()) / 2);
	else if (c1.getLon() != c4.getLon())
		coord.setLon((c1.getLon() + c4.getLon()) / 2);

	// Finds the middle latitude value by same logic as above
	if (c1.getLat() != c2.getLat())
		coord.setLat((c1.getLat() + c2.getLat()) / 2);
	else if (c1.getLat() != c3.getLat())
		coord.setLat((c1.getLat() + c3.getLat()) / 2);
	else if (c1.getLat() != c4.getLat())
		coord.setLat((c1.getLat() + c4.getLat()) / 2);

	return coord;
}

void TweetList::saveToFile(ofstream &file) {
	JSONObject root;
	JSONArray tweetArray;

	Node<Tweet> *thisNode = head;
	while (thisNode != NULL) {
		tweetArray.push_back(new JSONValue(writeTweet(thisNode->tweet)));
		thisNode = thisNode->next;
	}

	root[L"search_metadata"] = new JSONValue(writeMetadata());
	root[L"statuses"] = new JSONValue(tweetArray);

	JSONValue *rootVal = new JSONValue(root);
	file << ws2s(rootVal->Stringify(true)).c_str();

	delete rootVal;
}

JSONObject TweetList::writeTweet(Tweet tweet) {
	JSONObject JSONTweet;

	wstring createdAt = s2ws(tweet.getTimePosted());
	wstring idstr = s2ws(tweet.getIdNum());
	wstring text = s2ws(tweet.getText());
	JSONTweet[L"created_at"] = new JSONValue(createdAt);
	JSONTweet[L"id_str"] = new JSONValue(idstr);
	JSONTweet[L"text"] = new JSONValue(text);
	JSONTweet[L"user"] = new JSONValue(writeUser(tweet.getPostingUser()));

	if (tweet.hasGeoInfo())
		JSONTweet[L"place"] = new JSONValue(writeGeoInfo(tweet.getGeoInfo()));

	JSONTweet[L"favorite_count"] = new JSONValue((double)tweet.getNumFavs());
	JSONTweet[L"retweet_count"] = new JSONValue((double)tweet.getNumRetweets());

	return JSONTweet;
}

JSONObject TweetList::writeUser(TwitterUser user) {
	JSONObject JSONUser;

	wstring idstr = s2ws(user.getUserId());
	wstring name = s2ws(user.getName());
	wstring location = s2ws(user.getLocation());
	wstring screenName = s2ws(user.getScreenName());
	//wstring description = s2ws(user.getDescrip());
	long followers = user.getNumFollowers();
	wstring createdAt = s2ws(user.getDateCreated());
	bool verified = user.isVerified();

	JSONUser[L"id_str"] = new JSONValue(idstr);
	JSONUser[L"name"] = new JSONValue(name);
	JSONUser[L"location"] = new JSONValue(location);
	JSONUser[L"screen_name"] = new JSONValue(screenName);
	//JSONUser[L"description"] = new JSONValue(description);
	JSONUser[L"followers_count"] = new JSONValue((double)followers);
	JSONUser[L"created_at"] = new JSONValue(createdAt);
	JSONUser[L"verified"] = new JSONValue(verified);

	return JSONUser;
}

JSONObject TweetList::writeGeoInfo(GeoInfo gi) {
	JSONObject geoInfo;

	wstring country = s2ws(gi.getCountry());
	wstring name = s2ws(gi.getPlace());
	double lon = gi.getCoordinates().getLon();
	double lat = gi.getCoordinates().getLat();

	geoInfo[L"full_name"] = new JSONValue(name);
	geoInfo[L"country_code"] = new JSONValue(country);
	geoInfo[L"lat"] = new JSONValue(lat);
	geoInfo[L"lon"] = new JSONValue(lon);

	return geoInfo;
}

JSONObject TweetList::writeMetadata() {
	JSONObject metadata;

	metadata[L"completed_in"] = new JSONValue(timeTwitterWorkedForUs);
	metadata[L"tweets_read_in"] = new JSONValue((double)numTweetsReadIn);

	return metadata;
}

Node<Tweet>* TweetList::loadFromFile(ifstream &file) {
	string toParse((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

	return beginParse(toParse);
}

// Converts a wide string to a string
// Credit to dk123 on Stack Overflow for this efficient method
string TweetList::ws2s(const wstring &wstr) {
	typedef codecvt_utf8<wchar_t> convert_typeX;
	wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

// Converts a string to a wide string
// Credit to dk123 on Stack Overflow for this efficient method
wstring TweetList::s2ws(const string& str) {
	typedef codecvt_utf8<wchar_t> convert_typeX;
	wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

vector<Tweet> TweetList::listToVector() {
	vector<Tweet> tweets(numTweets);
	Node<Tweet> *node = head;
	while (node != head) {
		tweets.push_back(node->tweet);
		node = node->next;
	}
	return tweets;
}

vector<Tweet> TweetList::getKeywordOccurances(string keyword) {
	vector<Tweet> keywordTweets;
	Node<Tweet> *node = head;
	int numOccurances = 0;
	while (node != NULL) {
		if (node->tweet.searchForKeyword(keyword) != 0)
			keywordTweets.push_back(node->tweet);
		node = node->next;
	}
	return keywordTweets;
}

vector<Tweet> TweetList::getGoodKeywordOccurances() {
	// ifstream objects to read in the keywords from the files
	ifstream goodIn("good_keywords.txt");
	// vector objects to store the keywords and the tweet objects containing those keywords
	vector<string> goodWords;
	vector<Tweet> goodTweets;

	// Reads all of the words/phrases into the respective keyword vectors
	while (goodIn) {
		string substr;
		getline(goodIn, substr, ',');
		goodWords.push_back(substr);
	}

	// Checks the start list for our keywords, saves the tweet object in a vector, and prints how many times the keywords appeared
	for (unsigned int i = 0; i < goodWords.size() - 1; i++) {
		vector<Tweet> theseTweets = getKeywordOccurances(goodWords[i]); // Gets a vector for tweets for the current keyword
		goodTweets.insert(goodTweets.begin(), theseTweets.begin(), theseTweets.end()); // Inserts the tweets into the main vector
		cout << goodWords[i] << ": " << to_string(theseTweets.size()) << endl; // Prints
	}

	return goodTweets;
}

vector<Tweet> TweetList::getBadKeywordOccurances() {
	// ifstream objects to read in the keywords from the files
	ifstream badIn("bad_keywords.txt");
	// vector objects to store the keywords and the tweet objects containing those keywords
	vector<string> badWords;
	vector<Tweet> badTweets;

	// Reads all of the words/phrases into the respective keyword vectors
	while (badIn) {
		string substr;
		getline(badIn, substr, ',');
		badWords.push_back(substr);
	}

	// Checks the start list for our keywords, saves the tweet object in a vector, and prints how many times the keywords appeared
	for (unsigned int i = 0; i < badWords.size() - 1; i++) {
		vector<Tweet> theseTweets = getKeywordOccurances(badWords[i]); // Gets a vector for tweets for the current keyword
		badTweets.insert(badTweets.begin(), theseTweets.begin(), theseTweets.end()); // Inserts the tweets into the main vector
		cout << badWords[i] << ": " << to_string(theseTweets.size()) << endl; // Prints
	}

	return badTweets;
}

// Sort by Tweet ID sorts each of the tweets in the list according to tweet ID
Node<Tweet>* TweetList::sortByTweetID() {
	mergeSort(&head);
	return head;
}

// getLargestTweetID() returns the most recent tweet in the list
string TweetList::getLargestTweetID() {
	Node<Tweet>* node = head;
	string largest = "0";
	while (node != NULL) {
		if (node->tweet.getIdNum().compare(largest))
			largest = node->tweet.getIdNum();
		node = node->next;
	}
	return largest;
}

// Get tweets in country gets all tweets with the correct country code
vector<Tweet> TweetList::getTweetsInCountry(string country) {
	Node<Tweet>* node = head;
	vector<Tweet> tweets;
	while (node != NULL) {
		if (node->tweet.getGeoInfo().getCountry() == country)
			tweets.push_back(node->tweet);
		node = node->next;
	}
	return tweets;
}

// MergeSort() contains code to sort a list by merge
// Credit to GeeksForGeeks.com for this merge sort method (next three functions)
void TweetList::mergeSort(Node<Tweet>** headRef) {
	Node<Tweet>* h = *headRef;
	Node<Tweet>* a;
	Node<Tweet>* b;

	/* Base case -- length 0 or 1 */
	if ((h == NULL) || (h->next == NULL)) {
		return;
	}

	/* Split head into 'a' and 'b' sublists */
	frontBackSplit(h, &a, &b);

	/* Recursively sort the sublists */
	mergeSort(&a);
	mergeSort(&b);

	/* answer = merge the two sorted lists together */
	*headRef = sortedMerge(a, b);
}

// Sorted merge is a helper method for the merge sort
// Credit to GeeksForGeeks.com for this merge sort method
Node<Tweet>* TweetList::sortedMerge(Node<Tweet>* a, Node<Tweet>* b) {
	Node<Tweet>* result = NULL;

	/* Base cases */
	if (a == NULL)
		return(b);
	else if (b == NULL)
		return(a);

	/* Pick either a or b, and recur */
	if (a->tweet.getIdNum().compare(b->tweet.getIdNum()) < 0) {
		result = a;
		result->next = sortedMerge(a->next, b);
	}
	else {
		result = b;
		result->next = sortedMerge(a, b->next);
	}
	return(result);
}

// Helper method for the merge sort
// Credit to GeeksForGeeks.com for this merge sort method
void TweetList::frontBackSplit(Node<Tweet>* source,
Node<Tweet>** frontRef, Node<Tweet>** backRef) {
	Node<Tweet>* fast;
	Node<Tweet>* slow;
	if (source == NULL || source->next == NULL) {
		/* length < 2 cases */
		*frontRef = source;
		*backRef = NULL;
	}
	else {
		slow = source;
		fast = source->next;

		/* Advance 'fast' two nodes, and advance 'slow' one node */
		while (fast != NULL) {
			fast = fast->next;
			if (fast != NULL) {
				slow = slow->next;
				fast = fast->next;
			}
		}

		/* 'slow' is before the midpoint in the list, so split it in two
		at that point. */
		*frontRef = source;
		*backRef = slow->next;
		slow->next = NULL;
	}
}

// get total influence adds the influence of all tweets in the list
int TweetList::getTotalInfluence() {
	int inf = 0;
	Node<Tweet>* node = head;
	while (node != NULL) {
		inf += node->tweet.getInfluence();
		node = node->next;
	}
	return inf;
}

int TweetList::getTotalNumRetweets() {
	int rts = 0;
	Node<Tweet>* node = head;
	while (node != NULL) {
		rts += node->tweet.getNumRetweets();
		node = node->next;
	}
	return rts;
}

int TweetList::getTotalNumFavs() {
	int favs = 0;
	Node<Tweet>* node = head;
	while (node != NULL) {
		favs += node->tweet.getNumFavs();
		node = node->next;
	}
	return favs;
}

// Save tweet info to file saves the tweetlist information that we will be tracking to a given file
void TweetList::saveTweetInfoToFile(ofstream& output) {
	vector<Tweet> goodTweets = getGoodKeywordOccurances();
	vector<string> goodWords;
	int goodInfluence = 0;
	vector<Tweet> badTweets = getBadKeywordOccurances();
	vector<string> badWords;
	int badInfluence = 0;

	// ifstream objects to read in the keywords from the files
	ifstream goodIn("good_keywords.txt");
	ifstream badIn("bad_keywords.txt");

	// Reads all of the words/phrases into the respective keyword vectors
	while (goodIn) {
		string substr;
		getline(goodIn, substr, ',');
		goodWords.push_back(substr);
	}
	while (badIn) {
		string substr;
		getline(badIn, substr, ',');
		badWords.push_back(substr);
	}

	string goodKeywordString = "Good Keyword Occurances:\n";
	for (unsigned int i = 0; i < goodWords.size() - 1; i++) {
		int numOccurances = 0;
		for (unsigned int j = 0; j < goodTweets.size(); j++) {
			if (goodTweets[j].searchForKeyword(goodWords[i]))
				numOccurances++;
		}
		goodKeywordString += goodWords[i] + " " + to_string(numOccurances) + "\n";
	}
	string badKeywordString = "Bad Keyword Occurances:\n";
	for (unsigned int i = 0; i < badWords.size() - 1; i++) {
		int numOccurances = 0;
		for (unsigned int j = 0; j < badTweets.size(); j++) {
			if (badTweets[j].searchForKeyword(badWords[i]))
				numOccurances++;
		}
		badKeywordString += badWords[i] + " " + to_string(numOccurances) + "\n";
	}

	for (unsigned int i = 0; i < goodTweets.size(); i++)
		goodInfluence += goodTweets[i].getInfluence();
	for (unsigned int i = 0; i < badTweets.size(); i++)
		badInfluence += badTweets[i].getInfluence();

	output << "Total Num Tweets: " + to_string(numTweets) << endl;
	output << "(Estimate) Number of tweets/minute: " + to_string(numTweets / 60.0) << endl;
	output << "Total Num Tweets Read In (includes duplicates): " + to_string(numTweetsReadIn) << endl;
	output << "(Estimate) Number of tweets/minute: " + to_string(numTweetsReadIn / 60.0) << endl;
	output << "Time Twitter Worked for Us: " + to_string(timeTwitterWorkedForUs) << endl;
	output << "Total Number of Retweets: " + to_string(getTotalNumRetweets()) << endl;
	output << "Average Number of Retweets: " + to_string(getTotalNumRetweets() / numTweetsReadIn) << endl;
	output << "Total Number of Favorites: " + to_string(getTotalNumFavs()) << endl;
	output << "Average Number of Favorites: " + to_string(getTotalNumFavs() / numTweetsReadIn) << endl;
	output << "Total Influence: " + to_string(getTotalInfluence()) << endl;
	output << "Average Tweet Influence: " + to_string(getTotalInfluence() / numTweets) << endl << endl;
	output << "Total Good Influence: " + to_string(goodInfluence) << endl;
	output << "Average Good Tweet Influence: " + to_string(goodInfluence / goodTweets.size()) << endl;
	output << "Total Num Good Keywords Found: " + to_string(goodTweets.size()) << endl;
	output << goodKeywordString << endl;
	output << "Total Bad Influence: " + to_string(badInfluence) << endl;
	output << "Total Num Bad Keywords Found: " + to_string(badTweets.size()) << endl;
	output << "Average Bad Tweet Influence: " + to_string(badInfluence / badTweets.size()) << endl;
	output << badKeywordString << endl;
}

// Print tweet info prints a bunch of information about each tweet in the list to the console
void TweetList::printTweetInfo() {
	Node<Tweet> *node = head;

	// Prints various information about each given tweet
	// Note there is more info than shown here
	while (node != NULL) {
		node->tweet.printInfo();
		cout << endl;
		node = node->next; // Traverse to next node
	}

	// Print list data
	cout << "Number of tweets: " << to_string(getNumTweets()) << endl;
	cout << "Number of tweets read in (includes duplicates): " << to_string(getNumTweetsReadIn()) << endl;
	cout << "Number of tweets in all current objects: " << to_string(getNumTweetsInAllObjects()) << endl;
	cout << "Number of tweets read in all current objects: " << to_string(getNumTweetsReadInAllObjects()) << endl;
	cout << "Total influence: " << to_string(getTotalInfluence()) << endl;
	cout << "Time twitter worked for us: " + to_string(getTimeTwitterWorkedForUs()) << endl;

	return;
}

// Prints all information about the tweets with good keywords in them
void TweetList::printGoodKeywordOccurances() {
	vector<Tweet> tweets = getGoodKeywordOccurances();
	for (unsigned int i = 0; i < tweets.size(); i++) {
		tweets[i].printInfo();
		cout << endl;
	}
}

// Prints all information about the tweets with bad keywords in them
void TweetList::printBadKeywordOccurances() {
	vector<Tweet> tweets = getBadKeywordOccurances();
	for (unsigned int i = 0; i < tweets.size(); i++) {
		tweets[i].printInfo();
		cout << endl;
	}
}