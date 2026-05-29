#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cctype>
#include <vector>

using namespace std;

int Dop2()
{
    ifstream inputFile("input.json");
    if (!inputFile.is_open())
    {
        cout << "File not found" << endl;
        return 0;
    }
    ofstream outputFile("output.json");
    if (!outputFile.is_open()) 
    {
        cout << "Cannot create output file" << endl;
        return 0;
    }

    string line;
    int i = 0;
    map<int, int> wallet;
    int amount = 0;
    string strategy = "";
    int regim = 0;
    int k = 0, v = 0;

    while (getline(inputFile, line)) 
    {
        if (i == 0 || i == 1) 
        {
            i++;
            continue;
        }
        if (line.find("\"amount\"") != string::npos) 
        {
            regim = 1;
        }
        else if (line.find("\"strategy\"") != string::npos) 
        {
            regim = 2;
        }
        if (regim == 0 && i >= 2) 
        {
            bool hasDigits = false;
            for (char c : line) 
            {
                if (isdigit(c)) 
                { 
                    hasDigits = true; break; 
                }
            }
            if (!hasDigits) 
            {
                i++;
                continue;
            }

            int numbers[100];
            int numCount = 0;
            int currentNum = 0;
            bool inNum = false;

            for(char c : line) 
            {
                if (isdigit(c)) 
                {
                    currentNum = currentNum * 10 + (c - '0');
                    inNum = true;
                }
                else 
                {
                    if (inNum) 
                    {
                        numbers[numCount++] = currentNum;
                        currentNum = 0;
                        inNum = false;
                    }
                }
            }
            if (inNum) 
            {
                numbers[numCount++] = currentNum;
            }

            for (int j = 0; j + 1 < numCount; j += 2)
            {
                wallet[numbers[j]] = numbers[j + 1];
            }

            i++;
        }
        else if (regim == 1) 
        {
            for (char c : line) 
            {
                if (isdigit(c)) 
                {
                    amount = amount * 10 + (c - '0');
                }
            }
            regim = 0;
        }
        else if (regim == 2) 
        {
            if (line.find("MAX") != string::npos) 
            {
                strategy = "MAX";
            }
            else if (line.find("UNIFORM") != string::npos) 
            {
                strategy = "UNIFORM";
            }
            else if (line.find("MIN") != string::npos) 
            {
                strategy = "MIN";
            }
            regim = 0;
        }
        else {
            i++;
        }
    }

    vector<pair<int, int>> dispense;
    int remain = amount;

    if (strategy == "MAX") {
        for (auto j = wallet.rbegin(); j != wallet.rend(); ++j) 
{
            int nominal = j->first;
            int avail = j->second;

            if (remain >= nominal && avail > 0) {
                int count = remain / nominal;
                if (count > avail) {
                    count = avail;
                }
                if (count > 0) {
                    dispense.push_back({nominal, count});
                    remain = remain - (nominal * count);
                }
            }
        }
    }
    else if (strategy == "MIN")
    {
        bool exactFound = false;

        for (const auto& p : wallet)
        {
            int nominal = p.first;
            int available = p.second;

            if (nominal == remain && available > 0)
            {
                dispense.push_back({ nominal, 1 });
                remain = 0;
                exactFound = true;
                break;
            }
        }

        if (!exactFound && remain > 0)
        {
            map<int, int> tempWallet = wallet;
            vector<pair<int, int>> tempDispense;
            int tempRemain = remain;

            for (auto it = tempWallet.rbegin(); it != tempWallet.rend(); ++it)
            {
                int nominal = it->first;
                int avail = it->second;
                if (tempRemain >= nominal && avail > 0)
                {
                    int count = tempRemain / nominal;
                    if (count > avail)
                    {
                        count = avail;
                    }
                    if (count > 0)
                    {
                        tempDispense.push_back({ nominal, count });
                        tempRemain = tempRemain - (nominal * count);
                    }
                }
            }
            if (tempRemain == 0)
            {
                dispense = tempDispense;
                remain = 0;
            }
        }
    }
    else if (strategy == "UNIFORM")
    {
        int totalNotes = 0;
        for (const auto& p : wallet) 
        {
            totalNotes += p.second;
        }

        if (totalNotes > 0) 
        {
            int remainingAmount = amount;

            for (const auto& p : wallet) 
            {
                int nominal = p.first;
                int available = p.second;
                if (available > 0 && remainingAmount >= nominal) 
                {
                    int count = amount / totalNotes;
                    if (count > available) count = available;
                    if (count > 0) {
                        dispense.push_back({ nominal, count });
                        remainingAmount -= nominal * count;
                    }
                }
            }

            if (remainingAmount > 0) 
            {
                for (auto it = wallet.rbegin(); it != wallet.rend(); ++it) 
                {
                    int nominal = it->first;
                    int available = it->second;
                    int alreadyGiven = 0;

                    for (auto& d : dispense) 
                    {
                        if (d.first == nominal) 
                        {
                            alreadyGiven = d.second;
                            break;
                        }
                    }

                    int remainingAvailable = available - alreadyGiven;
                    if (remainingAvailable > 0 && remainingAmount >= nominal) 
                    {
                        int count = remainingAmount / nominal;
                        if (count > remainingAvailable) 
                        {
                            count = remainingAvailable;
                        }
                        if (count > 0) {
                            bool found = false;
                            for (auto& d : dispense) 
                            {
                                if (d.first == nominal) 
                                {
                                    d.second += count;
                                    found = true;
                                    break;
                                }
                            }
                            if (!found) 
                            {
                                dispense.push_back({ nominal, count });
                            }
                            remainingAmount -= nominal * count;
                        }
                    }
                }
            }
        }
    }

    outputFile << "[\n";
    outputFile << "{\n";
    outputFile << "\"dispense\": [";

    for (size_t idx = 0; idx < dispense.size(); idx++) {
        outputFile << "[" << dispense[idx].first << ", " << dispense[idx].second << "]";
        if (idx < dispense.size() - 1)
        {
            outputFile << ", ";
        }
    }

    outputFile << "]\n";
    outputFile << "}\n";
    outputFile << "]\n";

    inputFile.close();
    outputFile.close();
    return 0;
}

int main() {
    Dop2();
    return 0;
}