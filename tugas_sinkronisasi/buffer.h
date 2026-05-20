#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <string>
#include <iostream>

class Buffer {
private:
    std::vector<int> q;
    int evenCount = 0;
    int oddCount = 0;

public:
    void push(int val) {
        q.push_back(val);
        if (val % 2 == 0) evenCount++;
        else oddCount++;
    }

    int popEven() {
        for (auto it = q.begin(); it != q.end(); ++it) {
            if (*it % 2 == 0) {
                int val = *it;
                q.erase(it);
                evenCount--;
                return val;
            }
        }
        return -1; // Should not be reached if conditions are checked properly
    }

    int popOdd() {
        for (auto it = q.begin(); it != q.end(); ++it) {
            if (*it % 2 != 0) {
                int val = *it;
                q.erase(it);
                oddCount--;
                return val;
            }
        }
        return -1; // Should not be reached if conditions are checked properly
    }

    int size() const { return q.size(); }
    int getEvenCount() const { return evenCount; }
    int getOddCount() const { return oddCount; }

    std::string print() const {
        std::string s = "Buffer: [";
        for (size_t i = 0; i < q.size(); ++i) {
            s += std::to_string(q[i]) + (i == q.size() - 1 ? "" : ", ");
        }
        s += "]";
        return s;
    }
};

bool canProdEven(const Buffer& b) {
    return b.getEvenCount() < 10;
}

bool canProdOdd(const Buffer& b) {
    return b.getEvenCount() > b.getOddCount();
}

bool canConsEven(const Buffer& b) {
    return b.size() >= 3 && b.getEvenCount() > 0;
}

bool canConsOdd(const Buffer& b) {
    return b.size() >= 7 && b.getOddCount() > 0;
}

#endif // BUFFER_H
