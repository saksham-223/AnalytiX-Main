#ifndef ML_CORRECTOR_H
#define ML_CORRECTOR_H

#include "lexer.h"
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct Correction {
    string original;
    string corrected;
    int line;
    double probability;
};

// A genuine Probabilistic Machine Learning Model using N-Grams (Markov Chain)
class MLModel {
private:
    // Bi-gram maps: How frequently does Token B follow Token A?
    // Map of [PrevToken] -> Map[CurrentToken, Count]
    unordered_map<string, unordered_map<string, int> > bigramCounts;
    unordered_map<string, int> unigramCounts;
    int totalTokens = 0;

public:
    void train(const vector<Token>& validCodeTokens);
    double getProbability(const string& prevToken, const string& currentToken);
    
    // Predicts and corrects tokens based on Bayes probability & Levenshtein
    vector<Correction> performMLCorrection(vector<Token>& tokens);
};

#endif
