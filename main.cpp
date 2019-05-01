#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <chrono>
#include <array>
#include <queue>

using Str = std::string;

template<typename It>
void radixSort(It first, It last)
{
	using Bucket = std::queue<Str>;
	std::vector<Bucket> buckets(128);
	
	for(int i = 5; i >= 0; i--)
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

int main()
{
	std::vector<Str> data;

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

	std::cout << "Begin:\n";
	auto start = std::chrono::high_resolution_clock::now();
	radixSort(data.begin(), data.end() );
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << std::chrono::duration<double>(end - start).count() << "s\n";
	std::cout << std::boolalpha << "Vector sorted: " << std::is_sorted(data.begin(), data.end() ) << '\n';

	return 0;
}

