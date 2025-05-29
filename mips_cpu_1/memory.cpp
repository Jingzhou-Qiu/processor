#include <vector>
#include <cstdint>
#include <iostream>
#include <cmath>
#include "memory.h"

// Disable debug by ensuring ENABLE_DEBUG is not defined
#undef ENABLE_DEBUG

#ifdef ENABLE_DEBUG
#define DEBUG(x) x
#else
#define DEBUG(x) 
#endif

using namespace std;

// Check if hit in the cache
bool Cache::isHit(uint32_t address, uint32_t &loc) {
    int idx = getIndex(address);
    int tag = getTag(address);
    DEBUG(cout << name << " In isHIT: " << " tag: " << tag << endl;)

    for (int w=0; w<assoc; w++) {
        DEBUG(cout << "way: " << w << " line tag: " << line[idx*assoc+w].tag << " replBits: " << (int)line[idx*assoc+w].replBits << endl;)
        if (line[idx*assoc+w].valid && line[idx*assoc+w].tag == tag) {
            DEBUG(cout << name << " isHit is true" << endl;)
            loc = idx*assoc+w;
            updateReplacementBits(idx, w);
            return true;
        }
    }
    return false;
}

// Update replacement bits after access
void Cache::updateReplacementBits(int idx, int way) {
    uint8_t curRepl = line[idx*assoc+way].replBits;
    DEBUG(cout << name << " curRepl: " << (int)curRepl << endl;)
    for (int w=0; w<assoc; w++) {
        if (line[idx*assoc+w].valid && line[idx*assoc+w].replBits > curRepl) {
            DEBUG(cout << "way " << w << " replBits is: " << (int)line[idx*assoc+w].replBits << " will now decrease by 1" << endl;)
            line[idx*assoc+w].replBits--;
        }
    }
    line[idx*assoc+way].replBits = assoc-1;
}

// Read a word from this cache
bool Cache::read(uint32_t address, uint32_t &read_data, MSHREntry &entry) {
    uint32_t loc = 0;
    int missCountdown = name == "L1"? entry.L1_penality : entry.L2_penality;
    if (missCountdown) {
        DEBUG(cout << name + " Cache (read miss) at address " << std::hex << address << std::dec << ": " << missCountdown << " cycles remaining to be serviced\n");
        missCountdown--;
        if (name == "L1"){
            entry.L1_penality = missCountdown;
        }else{
            entry.L2_penality = missCountdown;
        }
        return false;
    }
    // Once miss penalty is completely paid, isHit should return true
    if (!isHit(address, loc)) {
        missCountdown = missPenalty-1;
        if (name == "L1"){
            entry.L1_penality = missCountdown;
        }else{
            entry.L2_penality = missCountdown;
        }
        return false;
    }
    read_data = line[loc].data[getOffset(address)/4]; 
    DEBUG(cout << name + " Cache (read hit): " << read_data << "<-[" << std::hex << address << std::dec << "]\n");
    entry.success = true;
    entry.write_value = read_data;
    return true;
}

// Write a word to this cache
bool Cache::write(uint32_t address, uint32_t write_data, MSHREntry &entry) {
    uint32_t loc = 0;
    int missCountdown = name == "L1"? entry.L1_penality : entry.L2_penality;
    if (missCountdown) {
        DEBUG(cout << name + " Cache (write miss) at address " << std::hex << address << std::dec << ": " << missCountdown << " cycles remaining to be serviced\n");
        missCountdown--;
        if (name == "L1"){
            entry.L1_penality = missCountdown;
        }else{
            entry.L2_penality = missCountdown;
        }
        return false;
    }
    // Once miss penalty is completely paid, isHit should return true
    if (!isHit(address, loc)) {
        missCountdown = missPenalty-1;
        if (name == "L1"){
            entry.L1_penality = missCountdown;
        }else{
            entry.L2_penality = missCountdown;
        }
        return false;
    }
    line[loc].data[getOffset(address)/4] = write_data;
    line[loc].dirty = true; 
    DEBUG(cout << name + " Cache (write hit): [" << std::hex << address << std::dec << "]<-" << write_data << "\n");
    entry.success = true;
    return true;
}

// Call this only if you know that a valid line with matching tag exists at that address 
CacheLine Cache::readLine(uint32_t address) {
    int idx = getIndex(address);
    int tag = getTag(address);

    for (int w=0; w<assoc; w++) {
        if (line[idx*assoc+w].valid && line[idx*assoc+w].tag == tag) {
            return line[idx*assoc+w];
        }
    }
    CacheLine c;
    c.valid = false;
    return c;
}

// Call this only if you know that a valid line with matching tag exists at that address 
void Cache::writeBackLine(CacheLine evictedLine) {
    int idx = getIndex(evictedLine.address);
    int tag = getTag(evictedLine.address);

    for (int w=0; w<assoc; w++) {
        if (line[idx*assoc+w].valid && line[idx*assoc+w].tag == tag) {
            for (int i = 0; i < CACHE_LINE_SIZE/4; i++) {
                line[idx*assoc+w].data[i] = evictedLine.data[i];
            }
            line[idx*assoc+w].dirty = true;
        }
    }
}

// Replace a line at the set corresponding this address
void Cache::replace(uint32_t address, CacheLine newLine, CacheLine &evictedLine) {
    
    int idx = getIndex(address);
    newLine.address = address;
    newLine.tag = getTag(address);
    newLine.valid = true;
    newLine.replBits = assoc - 1;

    /* Return if replacement already completed. */ 
    for (int w=0; w<assoc; w++) {
        if (line[idx*assoc+w].valid && line[idx*assoc+w].tag == newLine.tag) {
            updateReplacementBits(idx, w);
            return;
        }
    }
    /* Replace. */ 
    for (int w=0; w<assoc; w++) {
        if (!line[idx*assoc+w].valid || line[idx*assoc+w].replBits == 0) {
            DEBUG(cout << name + " Cache: replacing line at idx:" << idx << " way:" << w << " due to conflicting address:" << std::hex << address << std::dec << "\n");
            evictedLine = line[idx*assoc+w];
            line[idx*assoc+w] = newLine;
            return;
        }else{
            line[idx*assoc+w].replBits -= 1;
        }
    }
}

// Invalidate a line
void Cache::invalidateLine(uint32_t address) {
    int idx = getIndex(address);
    int tag = getTag(address);

    for (int w=0; w<assoc; w++) {
        if (line[idx*assoc+w].valid && line[idx*assoc+w].tag == tag) {
            line[idx*assoc+w].valid = false;
        }
    }
}

void Memory::tick(){

    for (auto &entry : mshr.entries) { 
        if ((!entry.is_write && L1.read(entry.address, entry.write_value, entry)) || (entry.is_write && L1.write(entry.address, entry.write_value, entry))) {
        } else if ((!entry.is_write && L2.read(entry.address, entry.write_value, entry)) || (entry.is_write && L2.write(entry.address, entry.write_value, entry))) {
            // Read from L2 but don't return a success status until miss penalty is paid off completely
            CacheLine evictedLine;
            L1.replace(entry.address, L2.readLine(entry.address), evictedLine);
    
            // writeback dirty line
            if (evictedLine.valid && evictedLine.dirty) {
                L2.writeBackLine(evictedLine);
            }
        } else {
            // Read from memory but don't return a success status until miss penalty is paid off completely
            int lineAddr = entry.address & ~(CACHE_LINE_SIZE-1);
            CacheLine c;
            CacheLine evictedLine;
            evictedLine.valid = false;
            DEBUG(print(lineAddr, 8));
            for (int i = 0; i < CACHE_LINE_SIZE/4; i++) {
               c.data[i] = mem[lineAddr/4+i];
            }
            L2.replace(entry.address, c, evictedLine); 
    
            // model an inclusive hierarchy
            if (evictedLine.valid) {
                L1.invalidateLine(evictedLine.address);
            }
    
            // writeback dirty line
            if (evictedLine.valid && evictedLine.dirty) {
                lineAddr = evictedLine.address & ~(CACHE_LINE_SIZE-1);
                for (int i = 0; i < CACHE_LINE_SIZE/4; i++) {
                   mem[lineAddr/4+i] = evictedLine.data[i];
                }
            }
        }
        
    }

}




bool Memory::access(uint32_t address, uint32_t &read_data, uint32_t write_data, bool mem_read, bool mem_write) {
    if (opt_level == 0) {
        if (mem_read) {
            read_data = mem[address/4];
        }
        if (mem_write) {
            mem[address/4] = write_data;
        }
        return true;
    }

    if (!mem_read && !mem_write) {
        return true;
    }

    if (mem_write) {
        MSHREntry entry;
        entry.address = address;
        entry.write_value = write_data;
        entry.is_write = true;
        entry.L1_penality = 0;
        entry.L2_penality = 0;
        entry.success = false;
        mshr.entries.push_back(entry);
        return false;
    }

    for (auto &entry : mshr.entries) {
        if (entry.is_write && entry.address == address) {
            read_data = entry.write_value;
            return true;
        }
        if (!entry.is_write && entry.address == address) {
            return false;
        }
    }

    MSHREntry entry;
    entry.address = address;
    entry.is_write = false;
    entry.L1_penality = 0;
    entry.L2_penality = 0;
    entry.success = false;
    mshr.entries.push_back(entry);
    return false;
}
