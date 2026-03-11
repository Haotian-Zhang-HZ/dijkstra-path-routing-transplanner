# ECS 34 Project 4 README

Student 1: Haotian Zhang (923663223)

About Partner Issue:
I am working alone on this project because my ex-partner has been completely unreachable and never attends class. I have tried to find another classmate to work with, but everyone who regularly attends lectures already has a partner. As a result, I have been unable to find a reliable partner for this project.

## Project Status
- Project Completed.  
- All `CTransportationPlannerCommandLine`, `CDijkstraTransportationPlanner`, `CDijkstraPathRouter`, and `CCSVBusSystemIndexer` functions have been implemented and tested.  
- Google Tests pass successfully and provide decent coverage for all implemented modules.  
- All required behaviors and command-line interactions specified in the project description have been implemented.

## Known Issues
- No known issues at this stage. 
Function implementations are completed.
All testcases completed and passed.


## Code References
- No Code References.

## Generative AI Use
## AI Use
- I used ChatGPT for informational questions only.  
- I asked questions such as:  
  - How to initialize a `std::unordered_set` from a `std::vector` in C++  
  - How to implement a min heap in C++  
  - How to store a user-defined struct in a `std::priority_queue`  
  - Whether storing a struct object or a `shared_ptr` to that struct in a priority queue is faster when constantly pushing and popping  
  - How to split a string by consecutive blocks of spaces whose number may vary in C++  
  - How to represent infinity for a `double` in C++  
- No AI-generated code, functions, or implementations were used in my project.  

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



### Prompt 5
storing a struct obj or storing a shared_ptr to that struct in priority queue, which one is faster if we need to constantly push and pop from PQ

### Response 5

Ah, this is a classic performance tradeoff in C++ STL containers like std::priority_queue. Let’s break it down carefully.

You’re asking about two options:
	1.	Storing struct objects directly:

struct Node { int x; double y; /* etc */ };
std::priority_queue<Node> pq;


	2.	Storing std::shared_ptr to struct objects:

std::priority_queue<std::shared_ptr<Node>> pq;



⸻

1️⃣ Storing structs directly
	•	Memory layout: Each element is fully contained in the container’s underlying storage (usually a std::vector).
	•	Push/Pop cost:
	•	When pushing/popping, std::priority_queue may move or copy the struct to maintain the heap invariant.
	•	If your struct is small and trivially copyable (like a few ints/doubles), copying is very cheap.
	•	Moving is also cheap if you define a move constructor (or let the compiler generate one).
	•	Cache friendliness: Very good, because data is contiguous in memory.
	•	Overhead: Minimal — just the cost of copying/moving structs.

⸻

2️⃣ Storing shared_ptr<Node>
	•	Memory layout: The priority queue stores pointers, not the actual objects. The objects themselves live on the heap.
	•	Push/Pop cost:
	•	Each push/pop involves copying/moving a shared_ptr (cheap, just pointer + atomic ref count change).
	•	But allocating the struct on the heap (std::make_shared<Node>()) is relatively expensive.
	•	Every shared_ptr increment/decrement is an atomic operation, which is slower than a simple move of a struct.
	•	Cache friendliness: Worse, because the objects are scattered in the heap.
	•	Benefits: Useful if the objects are large, or need shared ownership across multiple containers.

⸻

⚡ Performance Rule of Thumb
	•	Small structs (≤ a few dozen bytes) → store directly in PQ. Moving/copying is faster than heap allocation + shared_ptr overhead.
	•	Large structs (> hundreds of bytes, or contain vectors/strings) → shared_ptr might make sense to avoid expensive copies.

In your case:

“we need to constantly push and pop from PQ”

	•	You definitely want minimal overhead, so storing the struct directly is almost always faster for constant push/pop.
	•	Only use shared_ptr if you need multiple owners of the same object or the struct is very large.

⸻

Example comparison (rough estimate)

struct Small { int x, y; double z; }; // ~16 bytes
std::priority_queue<Small> pq1;

struct Large { std::vector<int> v; }; // ~24 bytes + heap
std::priority_queue<std::shared_ptr<Large>> pq2;

	•	pq1.push() / pop() is basically just memmove of 16 bytes → very fast.
	•	pq2.push() / pop() requires:
	1.	Allocating Large on heap (~100–1000 bytes depending on vector)
	2.	Creating shared_ptr (atomic ref count)
	3.	Copying pointer (~8 bytes) in the heap array → slower, more scattered memory → worse cache.

⸻

✅ Conclusion:
	•	If your struct is small and you only need it inside the PQ → store the struct directly.
	•	If your struct is large or needs shared ownership → shared_ptr.
	•	For high-frequency push/pop (like in Dijkstra’s PQ), direct storage is almost always faster.

⸻

If you want, I can give a tiny benchmark showing push/pop speed difference between struct vs shared_ptr<struct> for a PQ — it’s surprisingly dramatic for small structs.

Do you want me to do that?

### Prompt 6
how to split s a string by consecutive block of spaces whose number may vary in C++

### Response 6
In C++, if you want to split a string by any consecutive block of spaces (where the number of spaces may vary), the most robust way is to use std::regex or std::istringstream with some manual handling. I’ll show a few methods.

⸻

Method 1: Using std::regex

This works well when you want to treat any sequence of spaces as a delimiter.

#include <iostream>
#include <string>
#include <vector>
#include <regex>

int main() {
    std::string s = "This   is  a   test   string";
    
    std::regex re("\\s+"); // one or more whitespace characters
    std::sregex_token_iterator first{s.begin(), s.end(), re, -1}, last;
    std::vector<std::string> tokens{first, last};
    
    for (const auto& token : tokens) {
        std::cout << "[" << token << "]\n";
    }
}

Output:

[This]
[is]
[a]
[test]
[string]

✅ Explanation: \\s+ matches one or more whitespace characters, and the -1 flag tells the iterator to return the parts between matches.

⸻

Method 2: Using std::istringstream (simpler for spaces)

std::istringstream treats any whitespace as a delimiter automatically, but it only splits on any single or multiple whitespace together:

#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string s = "This   is  a   test   string";
    std::istringstream iss(s);
    std::string word;
    
    while (iss >> word) {
        std::cout << "[" << word << "]\n";
    }
}

Output:

[This]
[is]
[a]
[test]
[string]

✅ Explanation: The extraction operator (>>) automatically skips any number of consecutive whitespaces, so you don’t need regex.

⸻

Method 3: Manual split with find_first_not_of

If you want full control without regex or streams:

#include <iostream>
#include <string>
#include <vector>

int main() {
    std::string s = "This   is  a   test   string";
    std::vector<std::string> tokens;
    size_t start = 0, end;
    
    while ((start = s.find_first_not_of(' ', start)) != std::string::npos) {
        end = s.find(' ', start);
        tokens.push_back(s.substr(start, end - start));
        start = end;
    }
    
    for (const auto& t : tokens)
        std::cout << "[" << t << "]\n";
}

✅ This gives the same output and works even if the number of spaces varies.

⸻

💡 Recommendation:
	•	Use std::istringstream if you only need to split by spaces (simplest).
	•	Use std::regex if you might have other whitespace characters (tabs, etc.) or more complex patterns.

⸻

If you want, I can also show a one-liner modern C++17/20 version using ranges that’s very elegant for this.

Do you want me to show that version too?