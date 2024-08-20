#include <string>
#include <numbers>
#include <deque>

using namespace std; //specified requirment based on hardware setup

struct MAISEOptimalStampElement
{
	double availbilityEnd;
	uint8_t offSetData;
	//const unsigned int confidence : 2;
	uint8_t confidence; //assume memory stored as bytes, in addition to extra bitwise manipulation being slower
};

struct MAISEOptimalStamp {
	deque <MAISEOptimalStampElement> optimalStamps = {};
	//assuming insertions are made much more frequent than get
	//structure: ensures x has lower confidence level than x+1
	//           ensures x has higher availbilityEnd than x+1
	//due to self optimizing, it should never be too large to the point where static or mapping is required

	void ClearEntries() {
		optimalStamps.clear();
	}

	void InsertNewEntry(const MAISEOptimalStampElement* newElement) {
		int stampTime = 0;

		//using index instead of iterator due to invalidation after deletion of elements before it
		while (stampTime < optimalStamps.size()) {
			if (optimalStamps[stampTime].availbilityEnd <= newElement->availbilityEnd) {
				break;
			}
			else if (optimalStamps[stampTime].confidence >= newElement->confidence) { //won't be relevant
				return;
			}
			stampTime++;
		}

		int stampConfidence = stampTime;

		while (stampConfidence < optimalStamps.size())
		{
			if (optimalStamps[stampConfidence].confidence > newElement->confidence) {
				break;
			}
			stampConfidence++;
		}

		optimalStamps.erase(optimalStamps.begin() + stampTime, optimalStamps.begin() + stampConfidence);

		if (stampTime != stampConfidence) {
			optimalStamps.insert(optimalStamps.begin() + stampTime, *newElement);
		}
		else if (optimalStamps.empty()) {
			optimalStamps.push_front(*newElement);
		}
		else if (stampTime == optimalStamps.size()) {
			optimalStamps.push_back(*newElement);
		}
	}

	uint8_t GetBestData(const double timeNow) const {
		//cleanup option available if mutable, eg remove all entries before the timeNow every get
		//it should still be very efficient due to the auto resizing of the deque
		deque<MAISEOptimalStampElement>::const_reverse_iterator element = optimalStamps.rbegin();
		for (; element != optimalStamps.rend(); ++element) {
			if (element->availbilityEnd >= timeNow) {
				return element->offSetData;
			}
		}
		return 0;
	}

/*
	void PrintEverything() {
		deque<MAISEOptimalStampElement>::iterator element = optimalStamps.begin();
		for (element; element != optimalStamps.end(); ++element) {
			cout << "AvailbilityEnd: " << element->availbilityEnd << "  OffsetData: " << to_string(element->offSetData) << "  Confidence: " << to_string(element->confidence) << '\n';
		}
		cout << '\n';
	}
*/
};

class Photometer
{
public:
	// This signature cannot change
	void consume(
		double now,            // monotonic seconds
		const uint8_t data[2]  // raw from the sensor
	);

	// This signature cannot change
	double estimate(
		double now  // monotonic seconds
	) const;

	MAISEOptimalStamp MAISEData = {};
	const double referenceIlluminance = 50000;
};

void Photometer::consume(double now, const uint8_t data[2]) {
	uint8_t HRZ = ((data[0] & 0b11110000) >> 4);
	int8_t VAL = ((data[0] & 0b00000111) << 5) | ((data[1] & 0b00011111) >> 3);
	uint8_t CNF = (data[1] & 0b00000011);
	if (data[1] & (1 << 2)) {
		MAISEData.ClearEntries(); 
		//assume "all prior" targets all data entries that has been entered prior to this and not their time, if it is based on time, then do a clear like FindBestTime() instead
	}
	MAISEOptimalStampElement consumedEntry = {
		now + 16.5 * (1 << HRZ),
		VAL,
		CNF
	};
	MAISEData.InsertNewEntry(&consumedEntry);
}

double Photometer::estimate(double now) const
{
	return referenceIlluminance + MAISEData.GetBestData(now);
}
