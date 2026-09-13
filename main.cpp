#include <iostream>
#include <iomanip>
#include <cstdint>
#include <string>

#include "coututils.hpp"

const int HEAP_SIZE = 1*1028;

typedef uint8_t byte;
typedef uint16_t address;

byte reg1;
byte reg2;
byte reg3[4];
byte reg4[8];
byte heap[HEAP_SIZE];

// [size][occupied][data]

address useBlock(address blockAddress, int bytes){
	int blockSize = heap[blockAddress];
	
	if(blockSize >= bytes + 3){ // bytes + metadata + at least 1 usable byte ? then split into 2 blocks
		heap[blockAddress+2+bytes] = blockSize - (bytes + 2); // second block with leftover length
		heap[blockAddress+2+bytes+1] = false; // mark as free
		
		heap[blockAddress] = bytes;  
		heap[blockAddress+1] = true; // first block
		return blockAddress+2;	
	}
	else { // if not enough space to make another usable block, just take up this whole block
		heap[blockAddress+1] = true;
		return blockAddress+2;
	}	
}

address allocate(int bytes){
	int i = 0;
	int mergeCandidate = -1;
	while(i < HEAP_SIZE){
		if(heap[i] == 0){ // this must mean free, size of a block is never zero, always jumping to size field
			heap[i] = bytes;
			heap[i+1] = true;
			return i+2;
		}
		else if(heap[i] >= bytes){ // always jumping to size field
			if(heap[i+1] == false){ // this must mean its free
				return useBlock(i, bytes);
			} else{ // this must mean its occupied
				i += heap[i] + 2; // jump to next size field or free
			}
		}
		else if(heap[i] < bytes){ // always jumping to size field
			if(heap[i+1] == false) { // this block is too small but free so look to merge it
				if(mergeCandidate > -1){ // if previous block is merge candidate, merge
					heap[mergeCandidate] = heap[mergeCandidate] + heap[i] + 2;
					if(heap[mergeCandidate] >= bytes){ // if enough space stop looking
						return useBlock(mergeCandidate, bytes);
					}
				}
				else { // otherwise make this the merge candidate
					mergeCandidate = i; 
				}
			} else{
				mergeCandidate = -1;
			}
			i += heap[i] + 2; // jump to next size field or free
		}
	}
	std::cerr << "couldn't allocate\n";
	return -1;
}

void free(address tofree){
	heap[tofree-1] = false;
}

void write(const void* memory, address location, int size){
	const byte* src = (const byte*)memory;
	for(int i = 0; i < size; i++){
		heap[location + i] = src[i];
	}
}

void printHeap(){
	int emptyCounter = 0;
	for(int i = 0; i < HEAP_SIZE; i++){
		char c = heap[i];

		if (c == '\0') {
			emptyCounter++;
			if(emptyCounter < 4){
				std::cout << i << ": " << "\\0\n";	
			}
			else if(emptyCounter == 4) {
				std::cout << ". . .\n";
				continue;
			}
		}
		else {
			emptyCounter = 0;
			
			std::cout << i << ": ";

			if (std::isprint(c)) std::cout << "'" << c << "'";
			else if (c == '\n') std::cout << "\\n";
			else std::cout << (int)c;
			
			std::cout << "\n";
		}
		
	}
}

int main(){

	address string = allocate(sizeof("Hello World!\n"));
	write("Hello World!\n", string, sizeof("Hello World!\n"));
	printHeap();
		   
	free(string);
	printHeap();

	string = allocate(sizeof("smol"));
	write("smol", string, sizeof("smol"));
	printHeap();

	address string2 = allocate(sizeof("tiny"));
	write("tiny", string2, sizeof("tiny"));
	printHeap();

	free(string);
	free(string2);
	
	string = allocate(sizeof("Hello World!\n"));
	write("Hello World!\n", string, sizeof("Hello World!\n"));
	printHeap();
	
}
