#include <iostream>
#include <vector>
#include <cmath>
#include <stdlib.h> 
#include <chrono>

using namespace std;

struct BitVector {

	vector<bool> vecBool;
	
	BitVector (int const size) {
		vecBool.resize(size, true);

		// populate the empty bit vector with 1s randomly inserted
		/*		
		srand(time(NULL));		
		int random;

		do {
			random = rand() % (size-1);
			vecBool [random].flip();		
		} while (count(vecBool.begin(), vecBool.end(), true) != 3);
		*/
	}
};

class rank_support {

public:
	vector<uint64_t> superblocks {0};
	vector<uint16_t> blocks {0};
	int superBlockSize;
	int miniBlockSize;
	int blocksize;

	void build (vector <bool> const &vec) {
		blocksize = static_cast<int> (vec.size());
		miniBlockSize = ceil(log2(blocksize));
		superBlockSize = pow(miniBlockSize,2);
		int superBlockSum = 0, miniBlockSum = 0;

		for (int i = 0; i < blocksize; i++) {
			
			if (vec.at(i) == 1) {
				superBlockSum++;
				miniBlockSum++;
			}
			
			if((i+1)%miniBlockSize == 0 || i == blocksize) {
				blocks.push_back(miniBlockSum);
			}

			if ((i+1)%superBlockSize == 0 || i == blocksize) {
 				superblocks.push_back(superBlockSum);
				miniBlockSum = 0;
				blocks.pop_back();
				blocks.push_back(0);
 			}
		}
	}
	
	uint64_t rank1(vector <bool> const &vec, uint64_t i) {
		int superBlockNum = i/superBlockSize;
		int miniBlockNum = i/miniBlockSize;
		int remainderBlockSize = blocksize%miniBlockSize;
		int lookUpBits;
		int maxBlockSize;

		/* getting the within-block queries is implemented here by finding the index at the end of the previous
		block and then subtracting that from the position we want to query rank at. Then within-block is just the
		sum of the 1s up to i from the last block. */
		lookUpBits = (i+1) - (miniBlockNum * miniBlockSize);		
		int bitpatternSum = count(vec.begin()+(i-lookUpBits+1), vec.begin()+(i+1), true);
		return bitpatternSum + blocks [miniBlockNum] + superblocks [superBlockNum];
	}

	// size of data structure in bytes	
	uint64_t overhead() {
		return ((sizeof(superblocks[0]) * superblocks.size()) + (sizeof(blocks[0]) * blocks.size()));
	}

}; 

class select_support {

public:	
	uint64_t select1 (vector <bool> const &vec, rank_support &rankInstance, uint64_t i, int fr, int bk) {

		if (fr <= bk) {
			
			int midpoint = fr + (bk-fr) / 2;
			int midpointRank = rankInstance.rank1(vec, midpoint);
		
			if  ((midpointRank == i) && (vec [midpoint] != 0)) {
				return midpoint+1;
			} else if ((midpointRank == i) && (vec [midpoint] == 0)) {   
				return select1 (vec, rankInstance, i, fr, midpoint-1);
			}

			if  (midpointRank > i) {
				return select1 (vec, rankInstance, i, fr, midpoint-1);
			} else {
				return select1 (vec, rankInstance, i, midpoint+1, bk);
			}
		}
		return -1;
	}

};

class SparseArray {
public:
	vector<bool> sparseArray;
	vector<string> valueSet;

	void create(uint64_t size) {
		sparseArray.resize(size,false);
	}

	void append(string elem, uint64_t pos) {
		valueSet.push_back(elem);
		sparseArray [pos].flip(); 
	}

	bool get_at_rank(uint64_t r, std::string& elem) {

		if (r > valueSet.size()) { 
			return false;
		} else {
			elem = valueSet [r]; 
			return true;
		}
	}

	bool get_at_index(rank_support &rankInstance, uint64_t r, std::string& elem) {

		if (sparseArray [r] != 1) {
			return false;
		} else {
			int positionRank = rankInstance.rank1(sparseArray, r);
			elem = valueSet [positionRank-1];
			return true; 
		}
	}

	uint64_t num_elem_at(rank_support &rankInstance, uint64_t r) {
		return rankInstance.rank1(sparseArray, r);
	}

	uint64_t size() {
		return sparseArray.size();
	}

	uint64_t num_elem(rank_support &rankInstance) {
		return rankInstance.rank1(sparseArray, sparseArray.size());
	}

}; 

void print(vector <uint64_t> const &a) {
   	cout << "The vector elements are : ";

	for(int i=0; i < a.size(); i++) {
		cout << a.at(i) << ' ';
	}
	cout << "\n";
}

int main () 
{
	clock_t start, end;

	// user determine size of bit vector
	int userIdx = 1000;
	BitVector B(userIdx);  
	
	// print content of bit vector	
	/*
	for (int i = 0; i < B.vecBool.size(); i++) {
		cout << B.vecBool.at(i) << ' ';
	}
	cout << "\n";
	*/
	
	// instantiate rank support data structure r
	rank_support r;
	r.build(B.vecBool);

	//print(r.superblocks);
	//print(r.blocks);

	// calling rank(1000) on bit vector B; 0-based index
	cout << "Rank(1000) is " << r.rank1(B.vecBool,9999) << endl;

	// instantiate select support instance s
	select_support s;

	// calling select(1) on bit vector B; 1-based index
	cout << "Select(1) is " << s.select1(B.vecBool,r,1,0,r.blocksize-1) << endl; 

	// create sparse array
	SparseArray T;
	T.create(100000);

	// insert elements to sparse array
	for (int i; i<89999; i++){
		T.append("foo", i);
	}

	// create rank support data structure for sparse array
	rank_support rSA;
	rSA.build(T.sparseArray);
	string e{""};
	
	// get_at_index method
	cout << T.get_at_index(rSA, 5, e) << "\n";
	// get_at_rank method
	cout << T.get_at_rank(1, e) << "\n";

	/* evaluate runtime 
	start = clock();
	end = clock();
	printf ("Sparse Array get at Index: %0.8f sec \n",((float) end - start)/CLOCKS_PER_SEC);
	*/
} 
	
