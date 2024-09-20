#include<vector>
#include<sstream>
#include<iostream>
#include<fstream>
#include<cctype> // For tolower

#include "hashtable.h"

using namespace std;

// define translation class
Translation::Translation(string meanings,string language)
{
    // Constructor for Translation class
    this->language = language;
    addMeaning(meanings);
}

void Translation::addMeaning(string newMeanings)
{
    // Add new meanings to the translation
    string newMeaning;

    // Split the new meanings by semicolon
    stringstream sstr(newMeanings);
    while(getline(sstr, newMeaning, ';'))
    {
        // Check if the new meaning is empty
        if (newMeaning.empty()) {
            continue; // Skip this meaning and move to the next
        }

        // Check if the new meaning already exists
        bool exists = false;
        for(int i = 0; i < meanings.size(); i++){
            if(newMeaning == meanings[i]){
                exists = true;
                break;
            }
        }
        if (!exists) {
            meanings.push_back(newMeaning);
        } 
    }
}


void Translation::display()
{     
    // Display the meanings of the translation
    for (int i = 0 ; i< meanings.size() ; i++) 
    {
        cout << meanings[i] ;
        if(i != meanings.size()-1)
        cout << "; ";
    }
    cout << endl;

}

// define Entry class
Entry::Entry(string word, string meanings,string language)
{
    // Constructor for Entry class
    this->word = word;
    this->deleted = false;
    
    addTranslation(meanings, language);
}

void Entry::addTranslation(string newMeanings, string language)
{
    // Add translation to the entry
    // Iterate through existing translations to find the one with the same language
    for (int i = 0; i<translations.size(); i++) {
        if (translations[i].language == language) {
            // Add new meanings to the existing translation
            translations[i].addMeaning(newMeanings);
            return;
        }
    }
    // If translation with the same language doesn't exist, create a new one
    Translation translation(newMeanings, language);
    this->translations.push_back(translation);
}

void Entry::print() 
{
    // Print the entry
    for (int i = 0; i < translations.size(); i++) {
        cout << left << setw(10) << translations[i].language << ": ";
        translations[i].display();
    }
}

// define Hashtable class
HashTable::HashTable(int capacity)
{
    // Constructor for HashTable class
    // Initialize the buckets
    buckets = new Entry*[capacity];
    for (int i = 0; i<capacity; i++){
        buckets[i]= nullptr;
    }

    this->size = 0;					   		//Current Size of HashTable
    this->capacity = capacity;				// Total Capacity of HashTable
    this->collisions = 0;
}

unsigned long HashTable::hashCode(string word)
{
    // Calculate the hash code for a word
    unsigned long hashValue = 0;

    // Convert the word to lowercase before hashing
    transform(word.begin(), word.end(), word.begin(), ::tolower);

    //Cycle shift 
    for (int i = 0; i < word.length(); i++){
    	hashValue += static_cast<unsigned long>(word[i]);
    	hashValue = (hashValue << 5) | (hashValue >> (64-5));
    }

    //Personal hashing(inspiration from polynomial and polynomial rolling)
    // unsigned int poly_base = 53;
    // for (int i = 0; i < word.length(); i++){
    //     hashValue = (hashValue + word[i])*poly_base % this->capacity;
    // }

    // Polynomial hashing
    // unsigned int poly_base = 59;
    // for (int i = 0; i < word.length(); i++)
    // {
    //     hashValue = (hashValue * poly_base + word[i])% this->capacity;
    // }
    
    // Polynomial rolling hashing
    // unsigned int poly_base = 53;
    // unsigned long power = 1;
    // for (int i = 0; i < word.length(); i++)
    // {
    //     hashValue = (hashValue + word[i]*power)% this->capacity;
    //     power = (power*poly_base)%this->capacity;
    // }

    return hashValue;
}

unsigned int HashTable::getSize(){
    // Get the current size of the hashtable
    return this->size;
}

unsigned int HashTable::getCollisions(){
    // Get the total number of collisions
    return this->collisions;
}

void HashTable::import(string path) 
{
    // Import data from a file
    ifstream Dictionary(path);
    if (!Dictionary.is_open()) {
        cerr << "Unable to open file for reading" << endl;
        exit(-1);
    }

    string language;
    getline(Dictionary, language);

    int wordsImported = 0;

    string line;
    while (getline(Dictionary, line)) 
    {
        // Split the line into word and meanings
        size_t delimiterPos = line.find(':');
        if (delimiterPos == string::npos) 
        {
            cerr << "Invalid format: " << line << endl;
            continue; // Skip this line and move to the next
        }

        string word = line.substr(0, delimiterPos);
        string meanings = line.substr(delimiterPos + 1);
        string meaning;

        // Check for empty word or meanings
        if (word.empty() || meanings.empty()) 
        {
            cerr << "Invalid format: " << line << endl;
            continue; // Skip this line and move to the next
        }
        
        stringstream sstr(meanings);
        while(getline(sstr, meaning, ';'))
        {
            // Check if the meaning is empty
            if (meaning.empty()) 
            {
                cerr << "Invalid format: " << line << endl;
                continue; // Skip this meaning and move to the next
            }
           
        } 
        addWord(word, meanings, language);
        wordsImported++; 
        
    }
    cout << wordsImported << " " << language << " words have been imported successfully" << endl;

    Dictionary.close();
}

void HashTable::addWord(string word, string meanings, string language){
    // Add a word to the hashtable
    insert(word,meanings,language);
}

void HashTable::delWord(string word)
{
    // Delete a word from the hashtable
    if (word.empty()) 
    {
        // Handle invalid input
        cout << "Invalid input: empty word" << endl;
        return; 
    }

    string init_word = word;
    // Convert the word to lowercase before searching
    transform(word.begin(), word.end(), word.begin(), ::tolower);

    // Calculate the index using the hash code
    unsigned long index = hashCode(word) % this->capacity;
    
    int offset = 1;

    // Linear probing to find the word and mark it as deleted
    while(buckets[index] != nullptr)
    {
        // Convert the word in the bucket to lowercase before comparing
        string currentWord = buckets[index]->word;
        transform(currentWord.begin(), currentWord.end(), currentWord.begin(), ::tolower);

        if (currentWord == word && !buckets[index]->deleted)
        {
            buckets[index]->deleted = true;
            cout << init_word << " has been successfully deleted from the Dictionary" << endl;
            size--;
            return;
        }
        index = (index + offset * offset) % this->capacity; // Quadratic probing
        offset++; // Increase the offset for the next probe
    }
    cout << init_word << " not found in the Dictionary " << endl;
}

void HashTable::insert(string word, string meanings,string language)
{
    // Insert a word into the hashtable
    // Convert the word to lowercase before inserting
    transform(word.begin(), word.end(), word.begin(), ::tolower);

    // Calculate the index using the hash code
    unsigned long index = hashCode(word) % this->capacity;

    // Check for collisions and handle them using quadratic probing
    int offset = 1;
    // Quadratic probing to find an empty slot or the key
    while(buckets[index] != nullptr)
    {
        if (buckets[index]->word == word)
        {
            buckets[index]->addTranslation(meanings, language);
            return;
        }
        collisions++; // Increment collision count only when a collision occurs
        index = (index + offset * offset) % this->capacity; // Quadratic probing
        offset++; // Increase the offset for the next probe
    }

    // Insert the key-value pair into the hash table
    buckets[index] = new Entry(word, meanings, language);
    size++;
}

void HashTable::delTranslation(string word, string language) 
{
    // Delete a translation from the hashtable
    if (word.empty() || language.empty()) {
        // Handle invalid input
        cout << "Invalid input " << endl;
        return;
    }
    // Convert the word to lowercase before searching
    transform(word.begin(), word.end(), word.begin(), ::tolower);

    unsigned long index = hashCode(word) % this->capacity;

    int offset = 1;

    while (buckets[index] != nullptr) 
    {
        // Convert the word in the bucket to lowercase before comparing
        string currentWord = buckets[index]->word;
        transform(currentWord.begin(), currentWord.end(), currentWord.begin(), ::tolower);

        if (currentWord == word)
        {
            for (int i = 0; i < buckets[index]->translations.size(); ++i) 
            {
                if (buckets[index]->translations[i].language == language) 
                {
                    buckets[index]->translations.erase(buckets[index]->translations.begin() + i);
                    if(buckets[index]->translations.empty()){
                        delWord(word);
                    }
                    cout << "Translation has been successfully deleted from the Dictionary" << endl;
                    return;
                }
            }
            cout << "Translation not found for word in language " << endl;
            return;
        }
        index = (index + offset * offset) % this->capacity; // Quadratic probing
        offset++; // Increase the offset for the next probe
    }

    cout << "Word not found in the Dictionary" << endl;
}

void HashTable::delMeaning(string word, string meaning, string language) 
{
    // Delete a meaning from the hashtable
    if (word.empty() || meaning.empty() || language.empty()) {
        // Handle invalid input
        cout << "Invalid input" << endl;
        return;
    }
    string init_word = word;
    // Convert the word to lowercase before searching
    transform(word.begin(), word.end(), word.begin(), ::tolower);

    unsigned long index = hashCode(word) % this->capacity;

    int offset = 1;

    while (buckets[index] != nullptr) 
    {
        // Convert the word in the bucket to lowercase before comparing
        string currentWord = buckets[index]->word;
        transform(currentWord.begin(), currentWord.end(), currentWord.begin(), ::tolower);
        
        if (currentWord == word) 
        {
            for (int i = 0; i < buckets[index]->translations.size(); ++i) {
                Translation& translation = buckets[index]->translations[i];
                
                if (translation.language == language) {
                    for (int j = 0; j < translation.meanings.size(); ++j)
                    {
                        if (translation.meanings[j] == meaning) {
                            translation.meanings.erase(translation.meanings.begin() + j);
                            if(translation.meanings.empty())
                            {
                                delTranslation(word,language);
                                // buckets[index]->translations.erase(buckets[index]->translations.begin() + i);
                            }
                            cout << "Meaning has been successfully deleted from the Translation " << endl;
                            return;
                        }
                    }
                    cout << "Meaning not found "<< endl;
                    return;
                }
            }
            cout << "Translation not found in any language" << endl;
            return;
        }
        index = (index + offset * offset) % this->capacity; // Quadratic probing
        offset++; // Increase the offset for the next probe
    }

    cout << init_word << " not found in the Dictionary" << endl;
}

void HashTable::exportData(string language, string filePath) 
{
    // Export data to a file
    if (language.empty()) {
        // Handle invalid input
        cout << "Invalid input: empty language" << endl;
        return; 
    }
    ofstream Dictionary(filePath);
    if (!Dictionary) {
        cout << "Unable to open file for writing" << endl;
        exit(-1);
    }

    Dictionary << language << endl;

    int cnt = 0;
    for (int i = 0; i < capacity; ++i) 
    {
        if (buckets[i] != nullptr) 
        {
            for (int j = 0; j < buckets[i]->translations.size(); ++j) 
            {
                Translation& translation = buckets[i]->translations[j];
                if (translation.language == language) 
                {
                    Dictionary << buckets[i]->word << ":";
                    for (int k = 0; k < translation.meanings.size(); ++k) 
                    {
                        Dictionary << translation.meanings[k];
                        if (k != translation.meanings.size() - 1)
                            Dictionary << ";";
                    }
                    Dictionary << endl;
                    cnt++;
                    break;  
                }
            }
        }
    }
    if(cnt == 0){
        cout << " Language not found " << endl;
    }
    else
        cout << cnt << " records have been exported to " << filePath << endl;
    Dictionary.close();
}

void HashTable:: find(string word)
{
    // Find a word in the hashtable
    if (word.empty()) {
        // Handle invalid input
        cout << "Invalid input: empty word" << endl;
        return;
    }

    string init_word = word;
    // Convert the word to lowercase before searching
    transform(word.begin(), word.end(), word.begin(), ::tolower);

    unsigned long index = hashCode(word)%this->capacity;
  
    int comparisons = 0;
    int probes = 0; // Initialize a variable to keep track of the number of probes
    int offset = 1;

    // Linear probing to find the word
    while(probes < this->capacity && buckets[index] != nullptr)
    {
        comparisons++;
        
        // Convert the word in the bucket to lowercase before comparing
        string currentWord = buckets[index]->word;
        transform(currentWord.begin(), currentWord.end(), currentWord.begin(), ::tolower);
        if (currentWord == word && !buckets[index]->deleted)
        {
            cout << init_word << " found in the Dictionary after " << comparisons << " comparisons" << endl; 
            buckets[index]->print();
            return;
        }
        
        else 
        {
            // Move to the next slot using quadratic probing
            index = (index + offset * offset) % this->capacity;
            offset++; // Increase the offset for the next probe
            probes++;
        }
    }
    // If the loop completes without finding the word
    cout << init_word << " not found in the Dictionary" << endl;
}

// Destructor for the HashTable class
HashTable::~HashTable()
{
    for (int i = 0; i < capacity; i++)
    {
       delete buckets[i]; // Delete each Entry object 
    }

    delete[] buckets; // Delete the array of Entry pointers
}
