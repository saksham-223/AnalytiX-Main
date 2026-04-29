#include "ml_corrector.h"
#include <algorithm>
#include <iostream>

// Helper logic: Calculates raw typo severity
int levenshtein(const string& s1, const string& s2) {
    int m = s1.length(), n = s2.length();
    vector<vector<int> > dp(m + 1, vector<int>(n + 1));
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int j = 0; j <= n; j++) dp[0][j] = j;
    
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1]) dp[i][j] = dp[i - 1][j - 1];
            else dp[i][j] = 1 + min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
        }
    }
    return dp[m][n];
}

void MLModel::train(const vector<Token>& validCodeTokens) {
    if (validCodeTokens.empty()) return;
    
    string prev = "START";
    for (const auto& t : validCodeTokens) {
        // Treat identifiers as generic placeholders for learning syntax sequences 
        string curr = (t.type == "IDENTIFIER" || t.type == "NUMBER") ? t.type : t.value; 
        bigramCounts[prev][curr]++;
        unigramCounts[prev]++;
        totalTokens++;
        prev = curr;
    }
}

// Retrieves P(Current | Previous)
double MLModel::getProbability(const string& prevToken, const string& currentToken) {
    if (unigramCounts[prevToken] == 0) return 0.05; // Smoothing for unknown paths
    return (double)bigramCounts[prevToken][currentToken] / unigramCounts[prevToken];
}

vector<Correction> MLModel::performMLCorrection(vector<Token>& tokens) {
    vector<Correction> corrections;
    vector<string> knownKeywords = {"int", "float", "if", "else", "while", "return", "printf"};
    
    string prev = "START";
    
    for (auto& t : tokens) {
        if (t.type == "IDENTIFIER" && t.value.length() >= 3) { 
            string bestCandidate = "";
            double bestProb = -1.0;
            
            for (auto kw : knownKeywords) {
                // Determine if it looks structurally similar
                if (levenshtein(t.value, kw) <= 2) { 
                    
                    // Core ML Logic: Evaluate Bayesian Probability
                    double prob = getProbability(prev, kw);
                    
                    if (prob > bestProb) {
                        bestProb = prob;
                        bestCandidate = kw;
                    }
                }
            }
            
            // If the model confidently predicts it was a keyword error based on syntax history
            if (bestCandidate != "" && bestProb > 0.01) { 
                corrections.push_back({t.value, bestCandidate, t.line, bestProb});
                t.type = "KEYWORD";
                t.value = bestCandidate;
                
                // Update stream for accuracy of subsequent tokens
                prev = bestCandidate; 
                continue;
            }
        }
        prev = (t.type == "IDENTIFIER" || t.type == "NUMBER") ? t.type : t.value;
    }
    return corrections;
}
