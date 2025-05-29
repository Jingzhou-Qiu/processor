#include "processor.h"
#include <cstring>
#include <iostream>
#include <queue>
#include <vector>
#include <array>
#include <cstdint>
#include <tuple>

const size_t instructionQueue_size = 30;
const int reorder_buffer_size = 50;
const int load_store_buffer_size = 20;
const int sheduleing_queue_size = 50;
const int scalar_size = 5;


class InstructionQueue {
    private:
        struct InstructionEntry {
            uint32_t instruction;
            uint32_t pc;
            bool     pending; // Valid bit
            uint32_t predicted_next_pc;
            bool taken;
        };
    
        std::vector<InstructionEntry> instruction_queue; // storage
        size_t head;   // index of oldest entry
        size_t tail;   // index one‑past newest entry
        const size_t max_size = instructionQueue_size;
    
    public:
        InstructionQueue() : head(0), tail(0) {
            instruction_queue.resize(max_size);
        }
    

        bool put(uint32_t instruction, uint32_t pc, bool pending, uint32_t predicted_next_pc, bool taken) {
            if ((tail + 1) % max_size != head) {
                instruction_queue[tail] = {instruction, pc, pending, predicted_next_pc, taken};
                tail = (tail + 1) % max_size;
                return true;
            }
            return false; 
        }
    

        void resolvePendingAddress(uint32_t address, uint32_t value) {
            for (size_t i = head; i != tail; i = (i + 1) % max_size) {
                auto &entry = instruction_queue[i];
                if (entry.pending && entry.pc == address) {
                    entry.instruction = value;
                    entry.pending     = false;

                }
            }
        }
    
        bool is_full()  const { return (tail + 1) % max_size == head; }
        bool is_empty() const { return tail == head || instruction_queue[head].pending;}
    
        // Retrieve & remove the front instruction if it's no longer pending
        std::tuple<uint32_t, uint32_t, uint32_t, bool> get() {
            if (tail != head && !instruction_queue[head].pending) {
            auto front = instruction_queue[head];
            head = (head + 1) % max_size;
            return {front.instruction, front.pc, front.predicted_next_pc, front.taken};
            }
            return {0, 0, 0, false};
        }
        void flush() {
            head = tail = 0;
        }
    };
    
    struct PredicativeReg {
        bool valid;      // Valid bit
        int tag;         // Tag (integer)
        int32_t value;   // Value (int32_t)
    };
    
    class PredicativeRegisterFile {
    private:
        std::vector<PredicativeReg> registers; // 32 entries
    
    public:
        PredicativeRegisterFile() {
            registers.resize(32);
            for (int i = 0; i < 32; ++i) {
                registers[i].valid = true; // Initialize valid bit to false
                registers[i].tag = 0;       // Initialize tag to 0
                registers[i].value = 0;     // Initialize value to 0
            }
        }
    
        // Read a register by index
        PredicativeReg read(int index) const {
            return registers[index];
        }
    
        // Write to a register by index
        void write(int index, bool valid, int tag, int32_t value) {
            registers[index].valid = valid;
            registers[index].tag = tag;
            registers[index].value = value;
        }
    
        // Check if a register is valid
        bool isValid(int index) const {
            return registers[index].valid;
        }
    
        // Update a register based on tag and value
        void update(int tag, int32_t value) {
            for (auto& reg : registers) {
                if (!reg.valid && reg.tag == tag) {
                    reg.value = value;
                    reg.valid = true;
                    return; 
                }
            }
        }
        void updateTag(int index, int tag) {
            registers[index].tag = tag;   // Update the tag
            registers[index].valid = false; // Set valid bit to false
        }

        void syncWithRealRegisters(Registers& realRegFile) {
            uint32_t read_data_1 = 0, read_data_2 = 0;
            for (size_t i = 0; i < registers.size(); ++i) {
                realRegFile.access(i, 0, read_data_1, read_data_2, 0, false, 0);
                registers[i].value = read_data_1; // Assign the value to the predicative register
                registers[i].valid = true;       // Set the valid bit to true
            }
        }

        
    };
    

class BranchPredictor {
    public:
        struct BTBEntry {
            uint32_t tag;
            uint32_t target;
            bool valid;
        };
    
        static constexpr size_t BHT_ENTRIES = 1024;
        static constexpr size_t BTB_ENTRIES = 1024;
    
        BranchPredictor()
            : BHT(BHT_ENTRIES, 1),
            BTB(BTB_ENTRIES)
        {}

        void printEntriesWithTarget() const {
            for (size_t i = 0; i < BTB.size(); ++i) {
            if (BTB[i].valid) {
                std::cout << "Entry " << i << ": "
                      << "Tag: " << BTB[i].tag
                      << ", Target: " <<std::hex << BTB[i].target
                      << std::endl;
            }
            }
        }
    
        // Predict: return <taken or not, predicted target>
        std::pair<bool, uint32_t> predict(uint32_t pc) {
            size_t bht_index = get_bht_index(pc);
            uint8_t counter = BHT[bht_index];
    
            size_t btb_index = get_btb_index(pc);
            const BTBEntry& entry = BTB[btb_index];
    
            uint32_t predicted_target;
            if (entry.valid && entry.tag == get_pc_tag(pc)) {
                predicted_target = entry.target;
                // std::cout << "Predicted target: " << predicted_target << std::endl;
            } else {
                predicted_target = pc + 4; // Default next instruction
            }
    
            bool predict_taken = (counter >= 2);
            

            return {predict_taken, predicted_target};
        }
    
        // Update: after execution, update prediction structures
        void update(uint32_t pc, bool actual_taken, uint32_t actual_target) {
            size_t bht_index = get_bht_index(pc);
            if (actual_taken) {
                if (BHT[bht_index] < 3) BHT[bht_index]++;
                size_t bht_index = get_bht_index(pc);
                size_t btb_index = get_btb_index(pc);
                BTB[btb_index].tag = get_pc_tag(pc);
                BTB[btb_index].target = actual_target;
                BTB[btb_index].valid = true;
                BTB[btb_index].valid = true;
            } else {
                if (BHT[bht_index] > 0) BHT[bht_index]--;
                
            }
        }

    
    private:
        std::vector<uint8_t> BHT;
        std::vector<BTBEntry> BTB;
    
        size_t get_bht_index(uint32_t pc) const {
            return (pc >> 2) & (BHT_ENTRIES - 1);
        }
    
        size_t get_btb_index(uint32_t pc) const {
            return (pc >> 2) % BTB_ENTRIES;
        }
    
        uint32_t get_pc_tag(uint32_t pc) const {
            return (pc >> 2);
        }
    };





class ReorderBuffer {
private:
    static const int MAX_SIZE = reorder_buffer_size; // Maximum size of the ROB
    struct ROBEntry {
        bool execute;          // Execution complete bit
        int dest_reg;          // Destination register
        uint32_t address;   // address
        uint32_t value;        // Value to be written
        uint32_t pc;           // Program counter
        bool mem_write;        // Memory write flag
        bool reg_write;        // Register write flag
        bool halfword;         // Halfword flag
        bool byte;             // Byte flag
        bool jump;             // Jump flag
        bool flush;
        bool pending;
    };

    std::array<ROBEntry, MAX_SIZE> buffer; // Circular queue
    int head; // Pointer to the next entry to be committed
    int tail; // Pointer to the next available slot for adding instructions
    int count; // Number of entries currently in the ROB

public:
    ReorderBuffer() : head(0), tail(0), count(0) {}

    // Check if there is space in the ROB
    bool hasSpace() const {
        return count < MAX_SIZE;
    }
    int commit(BranchPredictor& branch_predictor) {        
        // Move head pointer to the next entry
        int commitIdx = head;
        uint32_t pc = buffer[head].pc;
        branch_predictor.update(pc, buffer[head].jump, buffer[head].address);
        // std::cout << "PC: 0x" << std::hex << pc << std::dec << std::endl;
        head = (head + 1) % MAX_SIZE;
        count--;
        return commitIdx; // Successfully committed an entry
    }

    // Add a new entry to the ROB
    int put(int dest_reg, bool halfword, bool byte, uint32_t pc, 
        bool mem_write , bool reg_write, bool jump, 
        bool execute, uint32_t value, uint32_t address) {

        buffer[tail] = {
            .execute = execute,  
            .dest_reg = dest_reg,
            .address = address,   // Initialize store address to 0
            .value = value,        // Initialize value to 0
            .pc = pc,
            .mem_write = mem_write, // Initialize memory write flag to false
            .reg_write = reg_write, // Initialize register write flag to false
            .halfword = halfword,
            .byte = byte,
            .jump = jump,
            .flush = false,
            .pending = false,
        };

        int index = tail; // Store the current tail index
        tail = (tail + 1) % MAX_SIZE; // Move tail pointer to the next slot
        count++;
        return index; // Return the index where the entry was added
    }

    // Update an entry in the ROB
    void update(int index, uint32_t value, bool jump, uint32_t address, bool update_address) {
        buffer[index].value = value; 
        buffer[index].execute = true; 
        // std::cout << "Jump: " << jump << ", Buffer Jump: " << buffer[index].jump << ", Address: " << std::hex << buffer[index].address << std::endl;
        if (buffer[index].jump != jump){
            buffer[index].jump = jump;
            buffer[index].flush = true;
        }

        if (update_address){
            if(buffer[index].address != address && jump){
                buffer[index].flush = true;
            }
            buffer[index].address = address;
        }
        buffer[index].execute = true;
    }


    void updatePendingBit(int index) {
        if (index >= 0 && index < MAX_SIZE) {
            buffer[index].pending = true;
        }
    }

    std::tuple<int, ROBEntry> getFrontEntryWithIndex() {
        // Advance the head pointer if the current entry has already been executed or is not ready for execution
        if (count > 0 && buffer[head].execute == true && !buffer[head].pending) {
            return std::make_tuple(head, buffer[head]);
        }

        return std::make_tuple(-1, ROBEntry{false, 0, 0, 0, 0, false, false, false, false, false, false});
    }

    void flush() {
        head = 0;  // Reset the head pointer
        tail = 0;  // Reset the tail pointer
        count = 0; // Reset the count
        // Optionally, clear the buffer entries
        for (auto& entry : buffer) {
            entry = {false, 0, 0, 0, 0, false, false, false, false, false, false};
        }
    }

    
};


class LoadStoreBuffer {
private:
    static const int MAX_SIZE = load_store_buffer_size; // Maximum size of the Load/Store Buffer
    struct LSBEntry {
        bool valid_address;    // Valid bit for the address
        bool valid_value;      // Valid bit for the value
        int tag_address;       // Tag for the address
        int tag_value;         // Tag for the value
        uint32_t value;        // Value to be written
        uint32_t address;   // Store address
        bool byte;             // Byte flag
        bool halfword;         // Halfword flag
        bool is_store;         // Binary flag indicating if this is a store (true) or load (false)
        int ROBID;        // Reorder Buffer ID
        bool execute;
        bool complete;
        bool pending;       // cache miss, load in MSHR
    };

    std::array<LSBEntry, MAX_SIZE> buffer; // Circular array
    int head; // Pointer to the next entry to commit
    int tail; // Pointer to the next available slot for adding instructions
    int count; // Number of entries currently in the buffer

public:
    LoadStoreBuffer() : head(0), tail(0), count(0) {}

    // Check if there is space in the buffer
    bool hasSpace() const {
        return count < MAX_SIZE;
    }

    int put(bool valid_value, int tag_address, int tag_value, uint32_t value, bool byte, bool halfword, bool is_store, int ROBID) {
        buffer[tail] = {
            .valid_address = false,  
            .valid_value = valid_value,    
            .tag_address = tag_address,
            .tag_value = tag_value,
            .value = value,
            .address = 0,
            .byte = byte,
            .halfword = halfword,
            .is_store = is_store,
            .ROBID = ROBID,
            .execute = false,
            .complete = false,
            .pending = false,
        };

        int index = tail; // Store the current tail index
        tail = (tail + 1) % MAX_SIZE; // Move tail pointer to the next slot
        count++;
        return index; // Return the index where the entry was added
    }


    void commitByROBID(int ROBID) {
        for (int i = head, count = 0; count < this->count; i = (i + 1) % MAX_SIZE, ++count) {
            if (buffer[i].ROBID == ROBID) {
                buffer[i].complete = true; 
            }
        }
    }
    
    void resolvePendingState(uint32_t address, uint32_t value) {
        for (int i = head, count = 0; count < this->count; i = (i + 1) % MAX_SIZE, ++count) {
            if (buffer[i].pending && buffer[i].address == address) {
                buffer[i].value = value;
                buffer[i].pending = false;
                buffer[i].valid_value = true;
            }
        }
    }

    void update(int tag, uint32_t value) {
        for (int i = 0; i < MAX_SIZE; ++i) {
            if (buffer[i].tag_address == tag && !buffer[i].valid_address) {
                buffer[i].address = value;
                buffer[i].valid_address = true;
            }
            
            if (buffer[i].tag_value == tag && !buffer[i].valid_value) {
                buffer[i].value = value;
                buffer[i].valid_value = true;
            }
        }
    }

    void processValidMemoryInstructions(ReorderBuffer& reorder_buffer) {
        for (int i = head, count = 0; count < this->count; i = (i + 1) % MAX_SIZE, ++count) {
            if (buffer[i].execute & buffer[i].is_store){
                reorder_buffer.update(buffer[i].ROBID, buffer[i].value, false, buffer[i].address, true);
            }
        }
    }

    void updateExecutionBit() {
        for (int i = head, count = 0; count < this->count; i = (i + 1) % MAX_SIZE, ++count) {
                if (buffer[i].is_store && buffer[i].valid_address && buffer[i].valid_value){
                buffer[i].execute = true;
            }
            if (!buffer[i].is_store && buffer[i].valid_address && !buffer[i].execute) { 
                bool can_execute = true;
                uint32_t load_start = buffer[i].address;
                uint32_t load_end = load_start + (buffer[i].byte ? 1 : (buffer[i].halfword ? 2 : 4));
                
                for (int j = head; j != i; j = (j + 1) % MAX_SIZE) { 
                    if (buffer[j].is_store) {
                        if (!buffer[j].valid_address) {
                            can_execute = false;
                            break; 
                        }
                        
                        uint32_t store_start = buffer[j].address;
                        uint32_t store_end = store_start + (buffer[j].byte ? 1 : (buffer[j].halfword ? 2 : 4));
                                               
                        if (!(store_end <= load_start || store_start >= load_end)) { 
                            if (!buffer[j].valid_value) {
                                can_execute = false;
                                break; 
                            } 
                            can_execute = false;
                            break;
                        }
                    }
                }
                
                if (can_execute) {
                    buffer[i].execute = true;
                }
            }
        }
    }

    std::tuple<bool, uint32_t, bool, bool, int, int, bool, uint32_t> getExecutableLoad() {
        for (int i = head, count = 0; count < this->count; i = (i + 1) % MAX_SIZE, ++count) {
            if (!buffer[i].is_store && buffer[i].execute && !buffer[i].complete && !buffer[i].pending) {
                return {true, buffer[i].address, buffer[i].halfword, buffer[i].byte, i, buffer[i].ROBID, buffer[i].valid_value, buffer[i].value}; 
            }
        }
        return {false, 0, false, false, -1, -1, false, 0}; // No executable load found
    }

    void updatePendingBit(int index) {
        if (index >= 0 && index < MAX_SIZE) {
            buffer[index].pending = true;
        }
    }
    
    uint32_t resolveStoreValue(int lsb_index, uint32_t memory_value) {
        uint32_t resolved_value = memory_value;
        uint32_t target_start = buffer[lsb_index].address;
        uint32_t target_end = target_start + (buffer[lsb_index].byte ? 1 : (buffer[lsb_index].halfword ? 2 : 4));

        for (int j = head; j != lsb_index; j = (j + 1) % MAX_SIZE) {
            if (buffer[j].is_store) {
                uint32_t store_start = buffer[j].address;
                uint32_t store_end = store_start + (buffer[j].byte ? 1 : (buffer[j].halfword ? 2 : 4));                
                if (!(store_end <= target_start || store_start >= target_end)) { 
                    if (buffer[j].byte) {
                        resolved_value = (resolved_value & ~(0xFF << ((target_start - store_start) * 8))) |
                                         ((buffer[j].value & 0xFF) << ((target_start - store_start) * 8));
                    } else if (buffer[j].halfword) {
                        resolved_value = (resolved_value & ~(0xFFFF << ((target_start - store_start) * 8))) |
                                         ((buffer[j].value & 0xFFFF) << ((target_start - store_start) * 8));
                    } else {
                        resolved_value = buffer[j].value;
                    }
                }
            }
        }
        buffer[lsb_index].complete = true;
        return resolved_value;
    }


    void advanceHeadIfComplete() {
        while (count > 0 && buffer[head].complete) {
            head = (head + 1) % MAX_SIZE;
            count--; 
        }
    }

    void flush() {
        head = 0;  // Reset the head pointer
        tail = 0;  // Reset the tail pointer
        count = 0; // Reset the count
        // Optionally, clear the buffer entries
        for (auto& entry : buffer) {
            entry = {false, false, -1, -1, 0, 0, false, false, false, -1, false, false};
        }
    }

};



class SchedulingQueue {
    private:
        static const int MAX_SIZE = sheduleing_queue_size; // Maximum size of the Scheduling Queue
        
    public:
        // Control flags and instruction details bundled together
        struct InstructionDetails {
            unsigned ALU_op;   // ALU operation code
            bool memory;       // Memory operation
            bool jump_reg;     // 1 if jr
            bool link;         // 1 if jal
            bool branch;       // 1 if branch
            bool bne;          // 1 if bne
            int opcode;        // Instruction opcode (6 bits in MIPS)
            int funct;         // Function code (6 bits in MIPS)
            int shamt;         // Shift amount (5 bits in MIPS)
        };
        
    private:
        struct SQEntry {
            bool allocated;           // Indicates if the entry is allocated
            bool valid1;              // Valid bit for the first value
            int tag1;                 // Tag for the first value
            uint32_t value1;          // Value for the first operand
            bool valid2;              // Valid bit for the second value
            int tag2;                 // Tag for the second value
            uint32_t value2;          // Value for the second operand
            int ROBID;           // Reorder Buffer ID
            InstructionDetails inst;  // All instruction details bundled together
        };
    
        std::array<SQEntry, MAX_SIZE> buffer; // Array to store the scheduling queue entries
    
    public:
        SchedulingQueue() {
            for (int i = 0; i < MAX_SIZE; ++i) {
                buffer[i] = {
                    .allocated = false,
                    .valid1 = false,
                    .tag1 = -1,
                    .value1 = 0,
                    .valid2 = false,
                    .tag2 = -1,
                    .value2 = 0,
                    .ROBID = 0,
                    .inst = {0, 0, 0, 0, 0, 0, 0, 0, 0}  
                };
            }
        }
    
        // Check if there is an unallocated entry
        bool hasUnallocatedEntry() const {
            for (const auto& entry : buffer) {
                if (!entry.allocated) {
                    return true;
                }
            }
            return false;
        }


        // Allocate an entry with bundled instruction details
        int allocateEntry(int tag1, uint32_t value1, bool valid1, int tag2, uint32_t value2, bool valid2, 
                          const InstructionDetails& inst, int ROBID) {
            for (int i = 0; i < MAX_SIZE; ++i) {
                if (!buffer[i].allocated) {
                    buffer[i].allocated = true;
                    buffer[i].valid1 = valid1;
                    buffer[i].tag1 = tag1;
                    buffer[i].value1 = value1;
                    buffer[i].valid2 = valid2;
                    buffer[i].tag2 = tag2;
                    buffer[i].value2 = value2;
                    buffer[i].inst = inst;
                    buffer[i].ROBID = ROBID;
                    return i; 
                }
            }
            return -1; 
        }
    
        // Return a tuple that includes the InstructionDetails
        std::tuple<bool, uint32_t, uint32_t, int, InstructionDetails, int> deallocateEntry() {
            for (int i = 0; i < MAX_SIZE; ++i) {
                if (buffer[i].allocated && buffer[i].valid1 && buffer[i].valid2) {
                    uint32_t value1 = buffer[i].value1;
                    uint32_t value2 = buffer[i].value2;
                    int ROBID = buffer[i].ROBID;
                    InstructionDetails inst = buffer[i].inst;
                    
                    // Deallocate the entry
                    buffer[i].allocated = false;
                    buffer[i].valid1 = false;
                    buffer[i].tag1 = -1;
                    buffer[i].valid2 = false;
                    buffer[i].tag2 = -1;
                    buffer[i].ROBID = 0;
                    buffer[i].inst = {0, 0, 0, 0, 0, 0, 0, 0, 0};
                    
                    return {true, value1, value2, ROBID, inst, i};
                }
            }

            InstructionDetails emptyInst = {0, 0, 0, 0, 0, 0, 0, 0, 0};
            return {false, 0, 0, 0, emptyInst, -1};
        }
    

        void update(int tag, uint32_t value) {
            for (int i = 0; i < MAX_SIZE; ++i) {
                if (buffer[i].allocated) {
                    if (buffer[i].tag1 == tag && !buffer[i].valid1) {
                        buffer[i].value1 = value;
                        buffer[i].valid1 = true;
                    }
                    
                    if (buffer[i].tag2 == tag && !buffer[i].valid2) {
                        buffer[i].value2 = value;
                        buffer[i].valid2 = true;
                    }
                }
            }
        }

        void flush() {
            for (auto& entry : buffer) {
                entry = {
                    .allocated = false,
                    .valid1 = false,
                    .tag1 = -1,
                    .value1 = 0,
                    .valid2 = false,
                    .tag2 = -1,
                    .value2 = 0,
                    .ROBID = 0,
                    .inst = {0, 0, 0, 0, 0, 0, 0, 0, 0}
                };
            }
        }
    };





void Processor:: optimized_processor_advance(){
    static uint32_t current_pc = 0;
    static InstructionQueue instruction_queue;
    static PredicativeRegisterFile predicative_reg_file; 
    static ReorderBuffer reorder_buffer;
    static LoadStoreBuffer load_store_buffer;
    static SchedulingQueue scheduling_queue;
    static BranchPredictor branch_predictor;

    // branch_predictor.printEntriesWithTarget();
    memory->tick();
    // memory->mshr.print();
    auto &entries = memory->mshr.entries;



    size_t i = 0;
    while (i < entries.size()) {
        auto &entry = entries[i];
        if (entry.success) {
            if (entry.is_write) {
                int index = reorder_buffer.commit(branch_predictor);
                load_store_buffer.commitByROBID(index);
            }
            load_store_buffer.resolvePendingState(entry.address, entry.write_value);
            instruction_queue.resolvePendingAddress(entry.address, entry.write_value);
            entries.erase(entries.begin() + i);
        } else {
            ++i;
        }
    }




// reorder_buffer.printAllEntries();
for (int i = 0; i < scalar_size; i++){
    {
        //commit & flush & memory store
        auto [index, entry] = reorder_buffer.getFrontEntryWithIndex();
            if (index != -1){
            if (entry.mem_write){   
                uint32_t read_data_mem;
                uint32_t write_data_mem = entry.value;
                if(!(memory->access(entry.address, read_data_mem, write_data_mem, false, true))){
                    reorder_buffer.updatePendingBit(index);
                    break;
                }
            }
    
            if(entry.reg_write){
                uint32_t read_data_1 = 0;
                uint32_t read_data_2 = 0;
                regfile.access(0, 0, read_data_1, read_data_2, entry.dest_reg, true, entry.value);
            }
            if(entry.flush){
                reorder_buffer.commit(branch_predictor);
                instruction_queue.flush();
                predicative_reg_file.syncWithRealRegisters(regfile);
                reorder_buffer.flush();
                load_store_buffer.flush();
                scheduling_queue.flush();
                memory->mshr.flush();
                current_pc = entry.address;
                regfile.pc = entry.pc;
            }else{
            int commitIndex = reorder_buffer.commit(branch_predictor);
            load_store_buffer.commitByROBID(commitIndex);
            }
            regfile.pc = entry.pc;
        }
        
    }
}

for (int i = 0; i < scalar_size; i++){
{
    // load 
    load_store_buffer.advanceHeadIfComplete();
    load_store_buffer.updateExecutionBit();
    load_store_buffer.processValidMemoryInstructions(reorder_buffer);
    auto [success, address, halfword, byte, index, ROBID, valid_value, value] = load_store_buffer.getExecutableLoad();
    if (success){
        uint32_t read_data_mem = value;
        uint32_t final_value = 0;
        if (valid_value||memory->access(address, read_data_mem, 0, true, false)){
            final_value = load_store_buffer.resolveStoreValue(index, read_data_mem);
            final_value &= control.halfword ? 0xffff : control.byte ? 0xff : 0xffffffff;
            load_store_buffer.update(index + 64, final_value);
            scheduling_queue.update(index + 64, final_value);
            predicative_reg_file.update(index + 64, final_value);
            reorder_buffer.update(ROBID, final_value, false, 0, false);
        }else{
            load_store_buffer.updatePendingBit(index);
        }
    }

}
}


for (int i = 0; i < scalar_size; i++){
{
    // execute 
    auto [success, operand1, operand2, robID, control, index] = scheduling_queue.deallocateEntry();
    if (success){
        alu.generate_control_inputs(control.ALU_op, control.funct, control.opcode);
        uint32_t alu_zero = 0;
        uint32_t alu_result = alu.execute(operand1, operand2, alu_zero);
        // update buffer 
        predicative_reg_file.update(index, alu_result);
        load_store_buffer.update(index, alu_result);
        scheduling_queue.update(index, alu_result);
        if(control.branch){
            if ((control.branch && !control.bne && alu_zero) || (control.branch && control.bne && !alu_zero)){
                reorder_buffer.update(robID, 0, true, 0, false);
            }else{
                // std::cout << "Branch not taken" << std::endl;
                reorder_buffer.update(robID, 0, false, alu_result, false);
            }
        }else if(control.jump_reg){
            reorder_buffer.update(robID, 0, true, alu_result, true);
        }
        else if (!control.memory){
            reorder_buffer.update(robID, alu_result, false, 0, false);
        }

    }
}
}


for (int i = 0; i < scalar_size; i++){
{

    if (!instruction_queue.is_empty() && reorder_buffer.hasSpace() && scheduling_queue.hasUnallocatedEntry()&&load_store_buffer.hasSpace()){
        // decode into control signals
        uint32_t decode_instruction;
        uint32_t decode_pc;
        uint32_t predicted_next_pc;
        bool taken;
        std::tuple<uint32_t, uint32_t, uint32_t, bool> decoded = instruction_queue.get();
        decode_instruction = std::get<0>(decoded);
        decode_pc = std::get<1>(decoded);
        predicted_next_pc = std::get<2>(decoded);
        taken = std::get<3>(decoded);
        control.decode(decode_instruction);

        // extract rs, rt, rd, imm, funct 
        int opcode = (decode_instruction >> 26) & 0x3f;
        int rs = (decode_instruction >> 21) & 0x1f;
        int rt = (decode_instruction >> 16) & 0x1f;
        int rd = (decode_instruction >> 11) & 0x1f;
        int shamt = (decode_instruction >> 6) & 0x1f;
        int funct = decode_instruction & 0x3f;
        uint32_t imm = (decode_instruction & 0xffff);
        int addr = decode_instruction & 0x3ffffff;
        imm = control.zero_extend ? imm : (imm >> 15) ? 0xffff0000 | imm : imm;

        //put instruction into reorder buffer
        int tag_1 = -1;
        int value_1 = 0;
        bool valid_1 = 1;
        int tag_2 = -1;
        int value_2 = 0;
        bool valid_2 = 1;


        if (!opcode){
            // Rtype 
            if (control.shift){
                tag_1 = 0;
                value_1 = shamt;
                valid_1 = true;
    
            }else{
                PredicativeReg reg_1 = predicative_reg_file.read(rs);
                tag_1 = reg_1.tag;
                value_1 = reg_1.value;
                valid_1 = reg_1.valid;
            }
    
            if(control.ALU_src==1){
                tag_2 = 0;
                value_2 = imm;
                valid_2 = true;
            }else{
                PredicativeReg reg_2 = predicative_reg_file.read(rt);
                tag_2 = reg_2.tag;
                value_2 = reg_2.value;
                valid_2 = reg_2.valid;
            }

        }
        else if (control.jump_reg){
            PredicativeReg reg_1 = predicative_reg_file.read(rs);
            tag_1 = reg_1.tag;
            value_1 = reg_1.value;
            valid_1 = reg_1.valid;
        }
        else if (control.branch){
            PredicativeReg reg_1 = predicative_reg_file.read(rs);
            tag_1 = reg_1.tag;
            value_1 = reg_1.value;
            valid_1 = reg_1.valid;

            PredicativeReg reg_2 = predicative_reg_file.read(rt);
            tag_2 = reg_2.tag;
            value_2 = reg_2.value;
            valid_2 = reg_2.valid;
        }
        else{
            PredicativeReg reg_1 = predicative_reg_file.read(rs);
            tag_1 = reg_1.tag;
            value_1 = reg_1.value;
            valid_1 = reg_1.valid;


            tag_2 = 0;
            value_2 = imm;
            valid_2 = true;
        }
        

        const SchedulingQueue::InstructionDetails control_detail = {
            .ALU_op = control.ALU_op,
            .memory = control.mem_read || control.mem_write,
            .jump_reg = control.jump_reg,
            .link = control.link,
            .branch = control.branch,
            .bne = control.bne,
            .opcode = opcode,
            .funct = funct,
            .shamt = shamt
        };

        if (control.jump && !control.jump_reg && !control.branch){
            addr = ((decode_pc + 4)& 0xf0000000) & (addr << 2);
            if (predicted_next_pc != addr){
                current_pc = addr;
                taken = true;
                instruction_queue.flush();
            }
            taken = true;
        }else if (control.branch){
            addr = decode_pc + 4 + (imm << 2);
            if (taken && addr != predicted_next_pc){
                current_pc = addr;
                instruction_queue.flush();
            }
        }

        int ROBID = reorder_buffer.put(control.link ? 31 : control.reg_dest ? rd : rt, 
            control.halfword, control.byte, decode_pc, control.mem_write, control.reg_write, 
            taken, control.jump && !control.jump_reg, control.link ? decode_pc + 8 : 0, (control.jump_reg ? predicted_next_pc : (taken ? decode_pc + 4 : addr)));
        // std::cout << "Taken: " << taken 
        //           << ", Decode PC: " << std::hex << decode_pc 
        //           << ", Jump Reg: " << control.jump_reg 
        //           << ", Predicted Next PC: " << predicted_next_pc 
        //           << ", Decode PC + 4: " << (decode_pc + 4) 
        //           << ", Addr: " << std::hex << addr 
        //           << " , save addr " << std::hex << (control.jump_reg ? predicted_next_pc : (taken ? decode_pc + 4 : addr)) << std::endl;
        if (!control.jump || control.link || control.jump_reg){
            int index = scheduling_queue.allocateEntry(tag_1, value_1, valid_1, tag_2, value_2, valid_2, control_detail, ROBID);
            if (control.mem_read) {
                index = load_store_buffer.put(false, index, -1, 0, control.byte, control.halfword, false, ROBID) + 64;
            } else if (control.mem_write) {
                PredicativeReg reg3 = predicative_reg_file.read(rt);
                load_store_buffer.put(reg3.valid, index, reg3.tag, reg3.value, control.byte, control.halfword, true, ROBID);
            }

            if (control.reg_write) {
                predicative_reg_file.updateTag(control.link ? 31 : control.reg_dest ? rd : rt, index);
            }
        }
        
    }

}

}


for (int i = 0; i < scalar_size; i++){
    {
        // fetch
        uint32_t fetch_instruction;
        if (instruction_queue.is_full()){
            break;
        }
        
        auto[taken, predicted_target] = branch_predictor.predict(current_pc);

        // std::cout << "Current PC: 0x" << std::hex << current_pc 
        //           << ", Taken: " << taken 
        //           << ", Predicted Target: 0x" << predicted_target 
        //           << std::dec << std::endl;
        if(memory->access(current_pc, fetch_instruction, 0, 1, 0)){
            instruction_queue.put(fetch_instruction, current_pc, false, predicted_target, taken);
        }else{
            instruction_queue.put(0, current_pc, true, predicted_target, taken);
        }
        current_pc = taken? predicted_target: current_pc + 4;
    }

}

    
}
