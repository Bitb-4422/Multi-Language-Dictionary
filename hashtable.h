
#ifndef _HASHTABLE
#define _HASHTABLE
#include <vector>
using namespace std;


class Translation
{
	private:
		string language;
		vector<string> meanings;
	public:
		Translation(string meanings,string language);
		void addMeaning(string newMeanings);
		void display();
		friend class Entry;
		friend class HashTable;
};

class Entry
{
	private:
		string word;
		vector<Translation> translations;
		bool deleted;  // is the bucket is available to be reused after being deleted
	public:
		Entry(string word, string meanings,string language);
		void addTranslation(string newMeanings, string language);
		void print();
		friend class HashTable;
};

class HashTable
{
	private:
		Entry **buckets;		        			// Array of Pointers to Entries for Linear and Quadratic Probing
		unsigned int size;					   		//Current Size of HashTable
		unsigned int capacity;				    	// Total Capacity of HashTable
		unsigned int collisions; 					// Total Number of Collisions
	public:
		HashTable(int capacity);
		unsigned long hashCode(string word);
		unsigned int getSize();
		unsigned int getCollisions();
		void import(string path);
		void addWord(string word, string meanings, string language);
		void delWord(string word);
		void insert(string word, string meanings,string language);
		void delTranslation(string word, string language);
		void delMeaning(string word, string meaning, string language);
		void exportData(string language, string filePath);
		void find(string word);
		~HashTable();
};
#endif