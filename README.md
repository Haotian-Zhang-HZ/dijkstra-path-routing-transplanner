# ECS 34 Project 4 README

Student 1: Haotian Zhang (923663223)

## Project Status


## Known Issues
- No known issues at this stage. 
Function implementations are completed.
All testcases completed and passed.
100% line coverage


## Code References
- No Code References.

## Generative AI Use



### Prompt 1
in C++ how to initialize a unordered set from a vector

### Response 1
In C++, you can initialize an std::unordered_set from an std::vector in a few straightforward ways. Here’s the cleanest method using the range constructor:

#include <iostream>
#include <vector>
#include <unordered_set>

int main() {
    std::vector<int> vec = {1, 2, 3, 2, 4, 1};

    // Initialize unordered_set using vector range constructor
    std::unordered_set<int> uset(vec.begin(), vec.end());

    // Print the elements
    for (int x : uset) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    return 0;
}

Explanation:
	•	vec.begin() and vec.end() provide the start and end iterators of the vector.
	•	std::unordered_set’s constructor takes a range [first, last) and automatically removes duplicates because sets only store unique elements.

✅ After this, uset will contain {1, 2, 3, 4} (order is unspecified because it’s an unordered set).

⸻

If you want, I can also show a modern C++20 way using std::ranges which is even cleaner.

Do you want me to show that?

### Prompt 2
How to implement min heap in C++

### Response 2

Creating a min heap in C++ is straightforward, but there are a few approaches depending on whether you want convenience (priority_queue) or manual control (make_heap). Here’s a concise guide:

⸻

1️⃣ Using priority_queue (recommended)

By default, priority_queue is a max heap, but you can use std::greater to make it a min heap.

#include <iostream>
#include <queue>
#include <vector>

int main() {
    // Min heap of integers
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

    // Add elements
    minHeap.push(10);
    minHeap.push(5);
    minHeap.push(20);

    // Remove elements (smallest first)
    while (!minHeap.empty()) {
        std::cout << minHeap.top() << " ";  // Output: 5 10 20
        minHeap.pop();
    }
}

Key points:
	•	priority_queue<Type, Container, Comparator>
	•	Type → element type (e.g., int)
	•	Container → usually vector<Type>
	•	Comparator → std::greater<Type> for min heap

⸻

2️⃣ Using make_heap with a vector

You can manually create a heap with any container, usually a vector.

#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::vector<int> heap = {10, 5, 20};

    // Convert vector into a min heap
    std::make_heap(heap.begin(), heap.end(), std::greater<int>());

    // Add new element
    heap.push_back(15);
    std::push_heap(heap.begin(), heap.end(), std::greater<int>());

    // Pop the smallest element
    std::pop_heap(heap.begin(), heap.end(), std::greater<int>());
    int smallest = heap.back();
    heap.pop_back();
    std::cout << smallest << std::endl;  // Output: 5
}

Notes:
	•	make_heap → creates the heap
	•	push_heap → reorders after insertion
	•	pop_heap → moves smallest element to the end, then you remove it manually

⸻

✅ Summary:
	•	For most cases, use priority_queue with greater<>.
	•	Use make_heap if you want manual control or need a heap in a vector.

⸻

If you want, I can show a min heap for pairs (distance, node), which is exactly what’s used in Dijkstra’s algorithm. This is extremely useful for competitive programming.

Do you want me to show that?

### Prompt 3
how to store a user-defined struct in priority queue

### Response 3
Absolutely! Let’s go step by step. In C++, std::priority_queue by default creates a max-heap (largest element on top). When you want a priority queue of a user-defined struct, you need to tell C++ how to compare your structs. You have a few options: using operator<, a custom comparator struct, or a lambda.

⸻

1. Using operator< inside the struct

Suppose we have:

#include <iostream>
#include <queue>
#include <vector>

struct PlannerVertex {
    int id;
    double TimeTraveled;

    // define comparison for max-heap
    bool operator<(const PlannerVertex& other) const {
        return TimeTraveled < other.TimeTraveled; // larger TimeTraveled = higher priority
    }
};

int main() {
    std::priority_queue<PlannerVertex> pq;

    pq.push({1, 5.0});
    pq.push({2, 2.0});
    pq.push({3, 8.0});

    while (!pq.empty()) {
        std::cout << pq.top().id << " : " << pq.top().TimeTraveled << "\n";
        pq.pop();
    }
}

✅ Output (largest TimeTraveled first):

3 : 8
1 : 5
2 : 2

Note: The operator< defines the priority for the queue.

⸻

2. Using a custom comparator struct

If you don’t want to put the comparison inside the struct:

struct Compare {
    bool operator()(const PlannerVertex& a, const PlannerVertex& b) {
        return a.TimeTraveled > b.TimeTraveled; // min-heap: smaller first
    }
};

int main() {
    std::priority_queue<PlannerVertex, std::vector<PlannerVertex>, Compare> pq;

    pq.push({1, 5.0});
    pq.push({2, 2.0});
    pq.push({3, 8.0});

    while (!pq.empty()) {
        std::cout << pq.top().id << " : " << pq.top().TimeTraveled << "\n";
        pq.pop();
    }
}

✅ Output (smallest TimeTraveled first):

2 : 2
1 : 5
3 : 8


⸻

3. Using a lambda comparator (C++11 and later)

auto cmp = [](const PlannerVertex& a, const PlannerVertex& b) {
    return a.TimeTraveled > b.TimeTraveled; // min-heap
};

std::priority_queue<PlannerVertex, std::vector<PlannerVertex>, decltype(cmp)> pq(cmp);

pq.push({1, 5.0});
pq.push({2, 2.0});
pq.push({3, 8.0});

Note: You must use decltype(cmp) for the lambda type.

⸻

✅ Key points
	1.	Default std::priority_queue is a max-heap using operator<.
	2.	To reverse the order (min-heap), you need custom comparator.
	3.	You can store structs or pointers. For pointers, remember the comparator compares *ptr.
	4.	Don’t forget to specify std::vector<Struct> as the underlying container if using a custom comparator or lambda.

⸻

If you want, I can give a ready-to-use version for storing pointers to your struct in a min-heap—which avoids copying the struct every time. This is very handy for big structs.

Do you want me to show that version?.how to create a priority queue with user defined struct


### Prompt 4
How to represent infinity of double in C++

### Response 4
In C++, the standard way to represent infinity for a double is using the <limits> header:

#include <iostream>
#include <limits>

int main() {
    double inf = std::numeric_limits<double>::infinity();

    std::cout << "Infinity: " << inf << std::endl;

    // You can check for infinity
    if (inf > 1e308) {
        std::cout << "inf is really big!" << std::endl;
    }

    return 0;
}

Notes:
	1.	std::numeric_limits<double>::infinity() returns a positive infinity.
	2.	Negative infinity can be obtained by -std::numeric_limits<double>::infinity().
	3.	Comparisons with infinity behave as expected:
	•	any_finite_number < inf → true
	•	inf + 1000 == inf → true
	4.	This works reliably across platforms, unlike using large constants like 1e308.

⸻

If you want, I can also show a shortcut using HUGE_VAL, which is another legacy C-style way.

Do you want me to show that too?