#include <iostream>
#include <stdlib.h> // for atoi() and exit()
#include <time.h>   // for clock() and clock_t

#include <vector>
using namespace std;




int main(int argc, char** argv) {
  // Get desired size for the test vector
  if(argc != 2) {
    cout << "Usage: " << argv[0] << " <test vector size (a natural number)>\n";
    exit(EXIT_FAILURE);
  }
  int test_size = atoi(argv[1]);

  // Build a test vector of integers
  vector<int> test_array;
  int i;
  for(i = 1; i <= test_size; i++)
    test_array.push_back(i);

  // Iterate through vector using a C++ iterator
  vector<int>::iterator it = test_array.begin();
  time_t linear_search_time = clock();
  while(test_size != (*it))
    it++;
  linear_search_time = clock() - linear_search_time;
  cout << "Linear search time scanning through a " << test_size
    << " integer vector using vector<int>::iterator: " 
    << linear_search_time << endl;

  return 0;
}
