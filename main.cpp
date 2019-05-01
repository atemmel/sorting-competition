#include "stopwatch.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <vector>
#include <string>
#include <chrono>
#include <array>
#include <queue>
#include <thread>

constexpr int n_threads = 4;
constexpr int n_buckets= 128;
constexpr int max_index = 5;

using Str = std::string;
using Bucket = std::queue<Str>;
using Buckets = std::vector<Bucket>;
using ABuckets = std::array<Bucket, n_buckets>;

template<typename It>
void radixSort(It first, It last)
{
	Buckets buckets(n_buckets);
	
	for(int i = max_index; i >= 0; i--)
	{
		for(auto it = first; it != last; it++)
		{
			buckets[(*it)[i] ].push(*it);
		}

		auto it = first;

		for(auto & bucket : buckets)
		{
			while(!bucket.empty() )
			{
				*it = bucket.front();
				bucket.pop();
				++it;
			}
		}
	}
}

template<typename It>
void radixSortA(It first, It last)
{
	ABuckets buckets;
	
	for(int i = max_index; i >= 0; i--)
	{
		for(auto it = first; it != last; it++)
		{
			buckets[(*it)[i] ].push(*it);
		}

		auto it = first;

		for(auto & bucket : buckets)
		{
			while(!bucket.empty() )
			{
				*it = bucket.front();
				bucket.pop();
				++it;
			}
		}
	}
}

template<typename It>
void recursiveRadixSort(int index, It first, It last)
{
	if(index < 0) return;

	Buckets buckets(n_buckets);

	for(auto it = first; it != last; it++)
	{
		buckets[(*it)[index] ].push(*it);
	}

	auto it = first;

	for(auto & bucket : buckets)
	{
		while(!bucket.empty() )
		{
			*it = bucket.front();
			bucket.pop();
			++it;
		}
	}

	recursiveRadixSort(index - 1, first, last);
}

template<typename It>
void recursiveRadixSortNoCopy(int index, It first, It last, Buckets &buckets)
{
	if(index < 0) return;

	for(auto it = first; it != last; it++)
	{
		buckets[(*it)[index] ].push(*it);
	}

	auto it = first;

	for(auto & bucket : buckets)
	{
		while(!bucket.empty() )
		{
			*it = bucket.front();
			bucket.pop();
			++it;
		}
	}

	recursiveRadixSortNoCopy(index - 1, first, last, buckets);
}

template<typename It>
void threadedRadixSort(It first, It last)
{
	using Bucket = std::queue<Str>;
	using Buckets = std::vector<Bucket>;

	std::array<std::thread, n_threads> pool;

	int segment = std::distance(first, last) / n_threads;

	for(auto it = first, i = 0; it != last; it += segment, i++)
	{
		auto lambda = [&]()
		{
			recursiveRadixSort(max_index, it, it + segment);
		};

		pool[i] = std::thread(lambda);
	}

	for(auto & thread : pool) thread.join();

	Buckets buckets(n_buckets);

	for(int i = max_index; i >= 0; i--)
	{
		for(auto it = first; it != last; it++)
		{
			buckets[(*it)[i] ].push(*it);
		}

		auto it = first;

		for(auto & bucket : buckets)
		{
			while(!bucket.empty() )
			{
				*it = bucket.front();
				bucket.pop();
				++it;
			}
		}
	}
}

template<typename Sort, typename It>
void benchmark(Sort sort, unsigned iterations, It begin, It end)
{

	for(unsigned i = 0; i < iterations; i++)
	{
		printf("Step no: %d\t", i);

		Stopwatch watch;
		sort(begin, end);
		double seconds = watch.getSeconds();
		std::cout << seconds << ',';
		printf("Took: %f\n", seconds);
	}
	std::cout << "\n";
	printf("\n");
}


int main()
{
	std::vector<Str> data, copy;

	auto print = [](auto &value)
	{
		std::cout << value <<  ' ';
	};

	std::ifstream file("new_plates.txt");
	std::string buffer = "";

	while(file >> buffer) 
	{
		data.push_back(buffer);
	}
	file.close();

	copy = data;


	std::ofstream out;
	std::ostringstream stream;
	auto oldBuff = std::cout.rdbuf();

	std::cout.rdbuf(stream.rdbuf() );

	auto log = [&](const char* path)
	{
		out.open(path);
		out << stream.str();
		stream.clear();
		out.close();
		std::copy(copy.begin(), copy.end(), data.begin() );
	};

	auto radixSortLambda = [](auto first, auto last)
	{
		radixSort(first, last);
	};

	auto radixSortALambda = [](auto first, auto last)
	{
		radixSortA(first, last);
	};

	auto recursiveRadixSortLambda = [](auto first, auto last)
	{
		recursiveRadixSort(max_index, first, last);
	};

	auto recursiveRadixSortNoCopyLambda = [](auto first, auto last)
	{
		Buckets buckets(n_buckets);
		recursiveRadixSortNoCopy(max_index, first, last, buckets);
	};

	auto threadedRadixSortLambda = [](auto first, auto last)
	{
		threadedRadixSort(first, last);
	};

	unsigned iterations = 5;

	printf("Radix sort:\n");
	benchmark(radixSortLambda, iterations, data.begin(), data.end() );
	log("radix.csv");

	printf("Recursive Radix A sort:\n");
	benchmark(radixSortALambda, iterations, data.begin(), data.end() );
	log("recursiveradixa.csv");

	printf("Recursive Radix sort:\n");
	benchmark(recursiveRadixSortLambda, iterations, data.begin(), data.end() );
	log("recursiveradix.csv");

	printf("Recursive Radix NoCopy sort:\n");
	benchmark(recursiveRadixSortNoCopyLambda, iterations, data.begin(), data.end() );
	log("recursiveradixnocopy.csv");

	printf("Threaded Radix sort:\n");
	benchmark(threadedRadixSortLambda, iterations, data.begin(), data.end() );
	log("threadedradix.csv");

	return 0;
}

