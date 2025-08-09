#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <random>
#include <iomanip>
#include <chrono>

using namespace std;

// Global debug file stream
ofstream debugFile;

// Data structures
struct DataPoint
{
    vector<string> features;
    string label;
};

struct Node
{
    bool isLeaf;
    string label;        // For leaf nodes
    int featureIndex;    // For internal nodes
    string featureValue; // For categorical splits
    double threshold;    // For numerical splits
    bool isNumerical;
    map<string, Node *> children; // For categorical features
    Node *leftChild;              // For numerical features (<=)
    Node *rightChild;             // For numerical features (>)

    Node() : isLeaf(false), featureIndex(-1), threshold(0.0), isNumerical(false),
             leftChild(nullptr), rightChild(nullptr) {}
};

class DecisionTree
{
private:
    Node *root;
    vector<string> featureNames;
    vector<bool> isFeatureNumerical;
    string criterion;
    int maxDepth;

    // Utility functions
    double calculateEntropy(const vector<DataPoint> &data);
    double calculateInformationGain(const vector<DataPoint> &data, int featureIndex);
    double calculateGainRatio(const vector<DataPoint> &data, int featureIndex);
    double calculateNWIG(const vector<DataPoint> &data, int featureIndex);
    double calculateIntrinsicValue(const vector<DataPoint> &data, int featureIndex);

    // For numerical features
    pair<double, double> findBestNumericalSplit(const vector<DataPoint> &data, int featureIndex);
    vector<DataPoint> filterNumerical(const vector<DataPoint> &data, int featureIndex, double threshold, bool lessEqual);

    // For categorical features
    map<string, vector<DataPoint>> splitCategorical(const vector<DataPoint> &data, int featureIndex);

    // Tree building
    Node *buildTree(const vector<DataPoint> &data, int depth);
    int selectBestFeature(const vector<DataPoint> &data);
    string getMajorityClass(const vector<DataPoint> &data);
    bool isPure(const vector<DataPoint> &data);

    // Helper functions
    bool isNumeric(const string &str);
    void preprocessData(vector<DataPoint> &data);
    void handleMissingValues(vector<DataPoint> &data);

public:
    DecisionTree(const string &crit, int maxD) : root(nullptr), criterion(crit), maxDepth(maxD) {}
    ~DecisionTree() { deleteTree(root); }

    void deleteTree(Node *node);
    void train(vector<DataPoint> &data, const vector<string> &featNames);
    string predict(const DataPoint &point);
    double calculateAccuracy(const vector<DataPoint> &testData);
    int countNodes(Node *node);
    int calculateDepth(Node *node);
    void printTree(Node *node, int depth = 0);

    // Public methods to access tree statistics
    int getNodeCount() { return countNodes(root); }
    int getTreeDepth() { return calculateDepth(root); }
};

// Utility function to check if string is numeric
bool DecisionTree::isNumeric(const string &str)
{
    if (str.empty() || str == "?")
        return false; // Handle missing values
    char *end;
    strtod(str.c_str(), &end);
    return end != str.c_str() && *end == '\0';
}

// Calculate entropy
double DecisionTree::calculateEntropy(const vector<DataPoint> &data)
{
    if (data.empty())
        return 0.0;

    map<string, int> classCounts;
    for (const auto &point : data)
    {
        classCounts[point.label]++;
    }

    double entropy = 0.0;
    int total = data.size();
    for (const auto &pair : classCounts)
    {
        double probability = (double)pair.second / total;
        if (probability > 0)
        {
            debugFile << "[DEBUG] probability " << probability << endl;
            entropy -= probability * log2(probability);
        }
    }
    return entropy;
}

// Find best numerical split using midpoint strategy
pair<double, double> DecisionTree::findBestNumericalSplit(const vector<DataPoint> &data, int featureIndex)
{
    vector<double> values;
    for (const auto &point : data)
    {
        values.push_back(stod(point.features[featureIndex]));
    }

    debugFile << "[DEBUG] nawhhhh " << endl; 

    sort(values.begin(), values.end());
    values.erase(unique(values.begin(), values.end()), values.end());

    double bestThreshold = 0.0;
    double bestGain = -1.0;
    double parentEntropy = calculateEntropy(data);

     debugFile << "[DEBUG] nawhhhh 2 " << endl;

    debugFile << "[DEBUG] bruh 1 " << endl; 

    for (int i = 0; i < values.size() - 1; i++)
    {
        double threshold = (values[i] + values[i + 1]) / 2.0;

        vector<DataPoint> leftData = filterNumerical(data, featureIndex, threshold, true);

        debugFile << "[DEBUG] bruh 2 " << endl; 

        vector<DataPoint> rightData = filterNumerical(data, featureIndex, threshold, false);

        debugFile << "[DEBUG] bruh 3 " << endl; 

        if (leftData.empty() || rightData.empty())
            continue;

        double leftEntropy = calculateEntropy(leftData);
        double rightEntropy = calculateEntropy(rightData);

        double weightedEntropy = ((double)leftData.size() / data.size()) * leftEntropy +
                                 ((double)rightData.size() / data.size()) * rightEntropy;

        double gain = parentEntropy - weightedEntropy;

        if (gain > bestGain)
        {
            bestGain = gain;
            bestThreshold = threshold;
        }
    }
    debugFile << "[DEBUG] bruh " << endl; 

    return make_pair(bestThreshold, bestGain);
}

// Filter data for numerical features
vector<DataPoint> DecisionTree::filterNumerical(const vector<DataPoint> &data, int featureIndex, double threshold, bool lessEqual)
{
    vector<DataPoint> result;
    for (const auto &point : data)
    {
        double value = stod(point.features[featureIndex]);
        if ((lessEqual && value <= threshold) || (!lessEqual && value > threshold))
        {
            result.push_back(point);
        }
    }
    return result;
}

// Split data for categorical features
map<string, vector<DataPoint>> DecisionTree::splitCategorical(const vector<DataPoint> &data, int featureIndex)
{
    map<string, vector<DataPoint>> splits;
    for (const auto &point : data)
    {
        splits[point.features[featureIndex]].push_back(point);
    }
    return splits;
}

// Calculate Information Gain for a feature
double DecisionTree::calculateInformationGain(const vector<DataPoint> &data, int featureIndex)
{
    double parentEntropy = calculateEntropy(data);
    debugFile << "parentEntropy " << parentEntropy << endl; 

    double weightedEntropy = 0.0;

    if (isFeatureNumerical[featureIndex])
    {
        debugFile << "[DEBUG] Not that cooked tho 1 " << endl; 
        auto splitResult = findBestNumericalSplit(data, featureIndex);

        debugFile << "[DEBUG] Not that cooked tho 2 " << endl; 
        double threshold = splitResult.first;

        vector<DataPoint> leftData = filterNumerical(data, featureIndex, threshold, true);

        debugFile << "[DEBUG] Not that cooked tho 3 " << endl; 
        vector<DataPoint> rightData = filterNumerical(data, featureIndex, threshold, false);

        debugFile << "[DEBUG] Not that cooked tho 4 " << endl; 

        if (!leftData.empty() && !rightData.empty())
        {
            weightedEntropy = ((double)leftData.size() / data.size()) * calculateEntropy(leftData) +
                              ((double)rightData.size() / data.size()) * calculateEntropy(rightData);
        }
    }
    else
    {
        auto splits = splitCategorical(data, featureIndex);
        for (const auto &split : splits)
        {
            double weight = (double)split.second.size() / data.size();
            weightedEntropy += weight * calculateEntropy(split.second);
        }
    }
    debugFile << "[DEBUG] Not that cooked tho " << endl; 

    debugFile << "[DEBUG] parentEntropy " << parentEntropy << endl; 
    debugFile << "[DEBUG] weightedEntropy " << weightedEntropy << endl; 

    return parentEntropy - weightedEntropy;
}

// Calculate Intrinsic Value
double DecisionTree::calculateIntrinsicValue(const vector<DataPoint> &data, int featureIndex)
{
    double intrinsicValue = 0.0;

    if (isFeatureNumerical[featureIndex])
    {
        auto splitResult = findBestNumericalSplit(data, featureIndex);
        double threshold = splitResult.first;

        vector<DataPoint> leftData = filterNumerical(data, featureIndex, threshold, true);
        vector<DataPoint> rightData = filterNumerical(data, featureIndex, threshold, false);

        if (!leftData.empty() && !rightData.empty())
        {
            double leftRatio = (double)leftData.size() / data.size();
            double rightRatio = (double)rightData.size() / data.size();

            if (leftRatio > 0)
                intrinsicValue -= leftRatio * log2(leftRatio);
            if (rightRatio > 0)
                intrinsicValue -= rightRatio * log2(rightRatio);
        }
    }
    else
    {
        auto splits = splitCategorical(data, featureIndex);
        for (const auto &split : splits)
        {
            double ratio = (double)split.second.size() / data.size();
            if (ratio > 0)
            {
                intrinsicValue -= ratio * log2(ratio);
            }
        }
    }

    return intrinsicValue;
}

// Calculate Gain Ratio
double DecisionTree::calculateGainRatio(const vector<DataPoint> &data, int featureIndex)
{
    double ig = calculateInformationGain(data, featureIndex);
    double iv = calculateIntrinsicValue(data, featureIndex);

    return (iv == 0.0) ? 0.0 : ig / iv;
}

// Calculate NWIG
double DecisionTree::calculateNWIG(const vector<DataPoint> &data, int featureIndex)
{
    double ig = calculateInformationGain(data, featureIndex);

    int k;
    if (isFeatureNumerical[featureIndex])
    {
        set<string> uniqueValues;
        for (const auto &point : data)
        {
            uniqueValues.insert(point.features[featureIndex]);
        }
        k = uniqueValues.size();
    }
    else
    {
        auto splits = splitCategorical(data, featureIndex);
        k = splits.size();
    }

    double penalty1 = log2(k + 1);
    double penalty2 = 1.0 - (double)(k - 1) / data.size();

    return (ig / penalty1) * penalty2;
}

// Select best feature based on criterion
int DecisionTree::selectBestFeature(const vector<DataPoint> &data)
{
    debugFile << "[DEBUG] Selecting best feature from " << featureNames.size() << " features..." << endl;
    int bestFeature = -1;
    double bestScore = -1.0;

    for (int i = 0; i < featureNames.size(); i++)
    {
        auto start = chrono::high_resolution_clock::now();

        debugFile << "[DEBUG] " << i << endl;

        double score = 0.0;

        if (criterion == "IG")
        {
            debugFile << "[DEBUG] Cooked" << endl;
            score = calculateInformationGain(data, i);
        }
        else if (criterion == "IGR")
        {
            score = calculateGainRatio(data, i);
        }
        else if (criterion == "NWIG")
        {
            score = calculateNWIG(data, i);
        }
        debugFile << "[DEBUG] Not Cooked" << endl;

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

        debugFile << "[DEBUG] Not Cooked 2" << endl;

        debugFile << "[DEBUG] Feature names size " << featureNames.size() << endl;
        debugFile << "[DEBUG] Trying to access " << featureNames[i] << endl;

        debugFile << "[DEBUG] " << criterion << endl;
        debugFile << "[DEBUG] " << score << endl;

        debugFile << "[DEBUG] Feature " << i << " (" << featureNames[i] << "): "
                  << criterion << " = " << score << " (took " << duration.count() << "ms)" << endl;

        debugFile << "[DEBUG] Not Cooked 3" << endl;

        if (score > bestScore)
        {
            bestScore = score;
            bestFeature = i;
        }
    }

    debugFile << "[DEBUG] Best feature: " << bestFeature
              << " (" << (bestFeature >= 0 ? featureNames[bestFeature] : "NONE")
              << ") with score: " << bestScore << endl;

    return bestFeature;
}

// Get majority class
string DecisionTree::getMajorityClass(const vector<DataPoint> &data)
{
    map<string, int> classCounts;
    for (const auto &point : data)
    {
        classCounts[point.label]++;
    }

    string majorityClass;
    int maxCount = 0;
    for (const auto &pair : classCounts)
    {
        if (pair.second > maxCount)
        {
            maxCount = pair.second;
            majorityClass = pair.first;
        }
    }
    return majorityClass;
}

// Check if data is pure
bool DecisionTree::isPure(const vector<DataPoint> &data)
{
    if (data.empty())
        return true;
    string firstLabel = data[0].label;
    for (const auto &point : data)
    {
        if (point.label != firstLabel)
            return false;
    }
    return true;
}

// Preprocess data to determine feature types
void DecisionTree::preprocessData(vector<DataPoint> &data)
{
    debugFile << "[DEBUG] Preprocessing data to determine feature types..." << endl;
    isFeatureNumerical.resize(featureNames.size(), false);

    for (int i = 0; i < featureNames.size(); i++)
    {
        bool allNumeric = true;
        int numericCount = 0;
        int totalCount = 0;

        for (const auto &point : data)
        {
            totalCount++;
            if (!isNumeric(point.features[i]))
            {
                allNumeric = false;
            }
            else
            {
                numericCount++;
            }
        }
        isFeatureNumerical[i] = allNumeric;

        debugFile << "[DEBUG] Feature " << i << " (" << featureNames[i] << "): "
                  << numericCount << "/" << totalCount << " numeric values -> "
                  << (allNumeric ? "NUMERICAL" : "CATEGORICAL") << endl;

        // Show sample values
        debugFile << "  Sample values: ";
        for (int j = 0; j < min(5, (int)data.size()); j++)
        {
            debugFile << "'" << data[j].features[i] << "' ";
        }
        debugFile << endl;
    }
}

// Handle missing values (represented as "?")
void DecisionTree::handleMissingValues(vector<DataPoint> &data)
{
    debugFile << "[DEBUG] Handling missing values..." << endl;
    int replacements = 0;

    for (auto &point : data)
    {
        for (int i = 0; i < point.features.size(); i++)
        {
            if (point.features[i] == "?")
            {
                replacements++;
                if (isFeatureNumerical[i])
                {
                    point.features[i] = "0"; // Replace with 0 for numerical features
                }
                else
                {
                    point.features[i] = ""; // Replace with empty string for categorical features
                }
            }
        }
    }

    debugFile << "[DEBUG] Replaced " << replacements << " missing values" << endl;
}

// Build tree recursively
Node *DecisionTree::buildTree(const vector<DataPoint> &data, int depth)
{
    debugFile << "[DEBUG] Building tree at depth " << depth << " with " << data.size() << " samples" << endl;

    Node *node = new Node();

    // Base cases
    if (data.empty())
    {
        debugFile << "[DEBUG] Empty data -> creating empty leaf" << endl;
        node->isLeaf = true;
        node->label = "";
        return node;
    }

    if (isPure(data) || (maxDepth > 0 && depth >= maxDepth))
    {
        string majorityClass = getMajorityClass(data);
        debugFile << "[DEBUG] Pure data or max depth reached -> creating leaf with label: " << majorityClass << endl;
        node->isLeaf = true;
        node->label = majorityClass;
        return node;
    }

    // Find best feature to split on
    int bestFeature = selectBestFeature(data);
    if (bestFeature == -1)
    {
        string majorityClass = getMajorityClass(data);
        debugFile << "[DEBUG] No good feature found -> creating leaf with label: " << majorityClass << endl;
        node->isLeaf = true;
        node->label = majorityClass;
        return node;
    }

    node->featureIndex = bestFeature;
    node->isNumerical = isFeatureNumerical[bestFeature];

    debugFile << "[DEBUG] Splitting on feature " << bestFeature << " (" << featureNames[bestFeature]
              << ") - " << (node->isNumerical ? "NUMERICAL" : "CATEGORICAL") << endl;

    if (isFeatureNumerical[bestFeature])
    {
        // Numerical feature
        auto splitResult = findBestNumericalSplit(data, bestFeature);
        node->threshold = splitResult.first;

        debugFile << "[DEBUG] Numerical split threshold: " << node->threshold << endl;

        vector<DataPoint> leftData = filterNumerical(data, bestFeature, node->threshold, true);
        vector<DataPoint> rightData = filterNumerical(data, bestFeature, node->threshold, false);

        debugFile << "[DEBUG] Left branch: " << leftData.size() << " samples, Right branch: " << rightData.size() << " samples" << endl;

        if (!leftData.empty())
        {
            node->leftChild = buildTree(leftData, depth + 1);
        }
        if (!rightData.empty())
        {
            node->rightChild = buildTree(rightData, depth + 1);
        }
    }
    else
    {
        // Categorical feature
        auto splits = splitCategorical(data, bestFeature);
        debugFile << "[DEBUG] Categorical split into " << splits.size() << " branches:" << endl;

        for (const auto &split : splits)
        {
            debugFile << "[DEBUG]   '" << split.first << "': " << split.second.size() << " samples" << endl;
            if (!split.second.empty())
            {
                node->children[split.first] = buildTree(split.second, depth + 1);
            }
        }
    }

    return node;
}

// Train the decision tree
void DecisionTree::train(vector<DataPoint> &data, const vector<string> &featNames)
{
    debugFile << "[DEBUG] Starting training with " << data.size() << " samples..." << endl;

    featureNames = featNames;

    auto start = chrono::high_resolution_clock::now();
    preprocessData(data);
    auto end = chrono::high_resolution_clock::now();
    debugFile << "[DEBUG] Preprocessing took: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    start = chrono::high_resolution_clock::now();
    handleMissingValues(data);
    end = chrono::high_resolution_clock::now();
    debugFile << "[DEBUG] Missing value handling took: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    start = chrono::high_resolution_clock::now();
    root = buildTree(data, 0);
    end = chrono::high_resolution_clock::now();
    debugFile << "[DEBUG] Tree building took: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;

    debugFile << "[DEBUG] Training completed. Tree depth: " << getTreeDepth() << ", Node count: " << getNodeCount() << endl;
}

// Predict a single data point
string DecisionTree::predict(const DataPoint &point)
{
    Node *current = root;

    while (current && !current->isLeaf)
    {
        if (current->isNumerical)
        {
            string featureValue = point.features[current->featureIndex];
            // Handle missing values during prediction
            if (featureValue == "?")
            {
                featureValue = "0"; // Replace with 0 for numerical
            }
            double value = stod(featureValue);
            if (value <= current->threshold)
            {
                current = current->leftChild;
            }
            else
            {
                current = current->rightChild;
            }
        }
        else
        {
            string featureValue = point.features[current->featureIndex];
            // Handle missing values during prediction
            if (featureValue == "?")
            {
                featureValue = ""; // Replace with empty string for categorical
            }
            if (current->children.find(featureValue) != current->children.end())
            {
                current = current->children[featureValue];
            }
            else
            {
                break; // Unknown value, return majority class
            }
        }
    }

    return current ? current->label : "";
}

// Calculate accuracy on test data
double DecisionTree::calculateAccuracy(const vector<DataPoint> &testData)
{
    int correct = 0;
    for (const auto &point : testData)
    {
        if (predict(point) == point.label)
        {
            correct++;
        }
    }
    return (double)correct / testData.size();
}

// Count nodes in tree
int DecisionTree::countNodes(Node *node)
{
    if (!node)
        return 0;

    int count = 1;
    if (node->isNumerical)
    {
        count += countNodes(node->leftChild);
        count += countNodes(node->rightChild);
    }
    else
    {
        for (const auto &child : node->children)
        {
            count += countNodes(child.second);
        }
    }
    return count;
}

// Calculate tree depth
int DecisionTree::calculateDepth(Node *node)
{
    if (!node || node->isLeaf)
        return 0;

    int maxChildDepth = 0;
    if (node->isNumerical)
    {
        maxChildDepth = max(calculateDepth(node->leftChild), calculateDepth(node->rightChild));
    }
    else
    {
        for (const auto &child : node->children)
        {
            maxChildDepth = max(maxChildDepth, calculateDepth(child.second));
        }
    }
    return 1 + maxChildDepth;
}

// Delete tree
void DecisionTree::deleteTree(Node *node)
{
    if (!node)
        return;

    if (node->isNumerical)
    {
        deleteTree(node->leftChild);
        deleteTree(node->rightChild);
    }
    else
    {
        for (auto &child : node->children)
        {
            deleteTree(child.second);
        }
    }
    delete node;
}

// Data loading functions
vector<DataPoint> loadIrisData(const string &filename)
{
    debugFile << "[DEBUG] Loading Iris data from: " << filename << endl;
    vector<DataPoint> data;
    ifstream file(filename);
    string line;

    if (!file.is_open())
    {
        debugFile << "[ERROR] Could not open file: " << filename << endl;
        return data;
    }

    // Skip header
    getline(file, line);
    debugFile << "[DEBUG] Iris header: " << line << endl;

    int lineCount = 0;
    while (getline(file, line))
    {
        stringstream ss(line);
        string item;
        DataPoint point;

        // Skip ID
        getline(ss, item, ',');

        // Read features
        for (int i = 0; i < 4; i++)
        {
            getline(ss, item, ',');
            point.features.push_back(item);
        }

        // Read label
        getline(ss, item, ',');
        point.label = item;

        data.push_back(point);
        lineCount++;
    }

    debugFile << "[DEBUG] Loaded " << lineCount << " Iris samples" << endl;
    if (!data.empty())
    {
        debugFile << "[DEBUG] First Iris sample: ";
        for (const auto &feature : data[0].features)
        {
            debugFile << feature << " ";
        }
        debugFile << "-> " << data[0].label << endl;
    }

    return data;
}

vector<DataPoint> loadAdultData(const string &filename)
{
    debugFile << "[DEBUG] Loading Adult data from: " << filename << endl;
    vector<DataPoint> data;
    ifstream file(filename);
    string line;

    if (!file.is_open())
    {
        debugFile << "[ERROR] Could not open file: " << filename << endl;
        return data;
    }

    int lineCount = 0;
    int missingValueCount = 0;

    while (getline(file, line))
    {
        stringstream ss(line);
        string item;
        DataPoint point;

        // Read all features (14 features)
        for (int i = 0; i < 14; i++)
        {
            getline(ss, item, ',');
            // Trim whitespace
            item.erase(0, item.find_first_not_of(" \t"));
            item.erase(item.find_last_not_of(" \t") + 1);
            if (item == "?")
                missingValueCount++;
            point.features.push_back(item);
        }

        // Read label
        getline(ss, item, ',');
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        point.label = item;

        data.push_back(point);
        lineCount++;

        if (lineCount % 5000 == 0)
        {
            debugFile << "[DEBUG] Loaded " << lineCount << " Adult samples so far..." << endl;
        }
    }

    debugFile << "[DEBUG] Loaded " << lineCount << " Adult samples total" << endl;
    debugFile << "[DEBUG] Found " << missingValueCount << " missing values (?)" << endl;

    if (!data.empty())
    {
        debugFile << "[DEBUG] First Adult sample: ";
        for (const auto &feature : data[0].features)
        {
            debugFile << feature << " ";
        }
        debugFile << "-> " << data[0].label << endl;
    }

    // Show label distribution
    map<string, int> labelCounts;
    for (const auto &point : data)
    {
        labelCounts[point.label]++;
    }
    debugFile << "[DEBUG] Adult label distribution:" << endl;
    for (const auto &pair : labelCounts)
    {
        debugFile << "  " << pair.first << ": " << pair.second << " samples" << endl;
    }

    return data;
}

// Filter features and split data into train and test
pair<pair<vector<DataPoint>, vector<DataPoint>>, vector<string>>
trainTestSplitWithFilter(vector<DataPoint> &data, const vector<string> &allFeatureNames,
                         const set<string> &skipFeatures = {}, double trainRatio = 0.8)
{

    // Create filtered feature list and indices
    vector<string> filteredFeatureNames;
    vector<int> keepIndices;

    for (int i = 0; i < allFeatureNames.size(); i++)
    {
        if (skipFeatures.count(allFeatureNames[i]) == 0)
        {
            filteredFeatureNames.push_back(allFeatureNames[i]);
            keepIndices.push_back(i);
        }
    }

    // Filter data to remove skipped features
    vector<DataPoint> filteredData;
    for (const auto &point : data)
    {
        DataPoint newPoint;
        for (int idx : keepIndices)
        {
            newPoint.features.push_back(point.features[idx]);
        }
        newPoint.label = point.label;
        filteredData.push_back(newPoint);
    }

    // Shuffle and split
    random_device rd;
    mt19937 g(rd());
    shuffle(filteredData.begin(), filteredData.end(), g);

    int trainSize = (int)(filteredData.size() * trainRatio);
    vector<DataPoint> trainData(filteredData.begin(), filteredData.begin() + trainSize);
    vector<DataPoint> testData(filteredData.begin() + trainSize, filteredData.end());

    return make_pair(make_pair(trainData, testData), filteredFeatureNames);
}

// Original split function for backward compatibility
pair<vector<DataPoint>, vector<DataPoint>> trainTestSplit(vector<DataPoint> &data, double trainRatio = 0.8)
{
    random_device rd;
    mt19937 g(rd());
    shuffle(data.begin(), data.end(), g);

    int trainSize = (int)(data.size() * trainRatio);
    vector<DataPoint> trainData(data.begin(), data.begin() + trainSize);
    vector<DataPoint> testData(data.begin() + trainSize, data.end());

    return make_pair(trainData, testData);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "Usage: " << argv[0] << " <criterion> <maxDepth>" << endl;
        cout << "Criterion: IG, IGR, or NWIG" << endl;
        cout << "MaxDepth: 0 for no pruning" << endl;
        return 1;
    }

    string criterion = argv[1];
    int maxDepth = stoi(argv[2]);

    // Feature names
    vector<string> irisFeatures = {"SepalLengthCm", "SepalWidthCm", "PetalLengthCm", "PetalWidthCm"};
    vector<string> adultFeatures = {"age", "workclass", "fnlwgt", "education", "education-num",
                                    "marital-status", "occupation", "relationship", "race",
                                    "sex", "capital-gain", "capital-loss", "hours-per-week", "native-country"};

    // Test both datasets
    cout << "Testing Decision Tree with " << criterion << " criterion and max depth " << maxDepth << endl;
    cout << "=================================================================" << endl;

    // Iris dataset
    cout << "\nIris Dataset Results:" << endl;

    // Open debug file for Iris
    debugFile.open("iris_debug.txt");
    debugFile << "=== IRIS DATASET DEBUG LOG ===" << endl;
    debugFile << "Criterion: " << criterion << ", Max Depth: " << maxDepth << endl;
    debugFile << "================================" << endl;

    vector<DataPoint> irisData = loadIrisData("./Datasets/Iris.csv");

    double irisAccuracySum = 0.0;
    int irisNodesSum = 0;
    int irisDepthSum = 0;

    for (int run = 0; run < 20; run++)
    {
        debugFile << "\n--- RUN " << run << " ---" << endl;
        auto result = trainTestSplitWithFilter(irisData, irisFeatures);
        auto split = result.first;
        auto filteredFeatures = result.second;

        DecisionTree tree(criterion, maxDepth);
        tree.train(split.first, filteredFeatures);

        double accuracy = tree.calculateAccuracy(split.second);
        cout << "Accuracy in run " << run << ": " << accuracy << endl;
        debugFile << "[DEBUG] Accuracy in run " << run << ": " << accuracy << endl;
        irisAccuracySum += accuracy;

        if (run == 0)
        { // Only count for first run to avoid confusion
            irisNodesSum = tree.getNodeCount();
            irisDepthSum = tree.getTreeDepth();
            debugFile << "[DEBUG] Tree statistics - Nodes: " << irisNodesSum << ", Depth: " << irisDepthSum << endl;
        }
    }

    debugFile << "\n=== IRIS FINAL RESULTS ===" << endl;
    debugFile << "Average Accuracy: " << fixed << setprecision(4) << irisAccuracySum / 20.0 << endl;
    debugFile << "Number of Nodes: " << irisNodesSum << endl;
    debugFile << "Tree Depth: " << irisDepthSum << endl;
    debugFile.close();

    cout << "Average Accuracy: " << fixed << setprecision(4) << irisAccuracySum / 20.0 << endl;
    cout << "Number of Nodes (without pruning): " << irisNodesSum << endl;
    cout << "Tree Depth (without pruning): " << irisDepthSum << endl;

    // Adult dataset
    cout << "\nAdult Dataset Results:" << endl;

    // Open debug file for Adult
    debugFile.open("adult_debug.txt");
    debugFile << "=== ADULT DATASET DEBUG LOG ===" << endl;
    debugFile << "Criterion: " << criterion << ", Max Depth: " << maxDepth << endl;
    debugFile << "================================" << endl;

    vector<DataPoint> adultData = loadAdultData("./Datasets/adult.data");

    double adultAccuracySum = 0.0;
    int adultNodesSum = 0;
    int adultDepthSum = 0;

    for (int run = 0; run < 20; run++)
    {
        debugFile << "\n--- RUN " << run << " ---" << endl;

        // Skip fnlwgt and education-num for Adult dataset
        set<string> skipFeatures = {"fnlwgt"};
        auto result = trainTestSplitWithFilter(adultData, adultFeatures, skipFeatures);
        auto split = result.first;
        auto filteredFeatures = result.second;

        DecisionTree tree(criterion, maxDepth);
        tree.train(split.first, filteredFeatures);

        double accuracy = tree.calculateAccuracy(split.second);
        cout << "Accuracy in run " << run << ": " << accuracy << endl;
        debugFile << "[DEBUG] Accuracy in run " << run << ": " << accuracy << endl;
        adultAccuracySum += accuracy;

        if (run == 0)
        {
            adultNodesSum = tree.getNodeCount();
            adultDepthSum = tree.getTreeDepth();
            debugFile << "[DEBUG] Tree statistics - Nodes: " << adultNodesSum << ", Depth: " << adultDepthSum << endl;
        }
    }

    debugFile << "\n=== ADULT FINAL RESULTS ===" << endl;
    debugFile << "Average Accuracy: " << fixed << setprecision(4) << adultAccuracySum / 20.0 << endl;
    debugFile << "Number of Nodes: " << adultNodesSum << endl;
    debugFile << "Tree Depth: " << adultDepthSum << endl;
    debugFile.close();

    cout << "Average Accuracy: " << fixed << setprecision(4) << adultAccuracySum / 20.0 << endl;
    cout << "Number of Nodes (without pruning): " << adultNodesSum << endl;
    cout << "Tree Depth (without pruning): " << adultDepthSum << endl;

    return 0;
}