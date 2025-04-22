# Cache Simulator

## Mapping and Replacement Polices
- Directly Mapped Cache
- Fully Associative Cache (LRU)
- Two-Level 4-Way Set Associative Cache (LRU)

## Working of the Simulator
The Main Memory here is defined to be 1MB which can be changed in the macro 'Main_mem' in 'src/cache_sim.cpp'. The cache parameters are calculated using the parameters defined (in their respective sizes) in 'config/cache_config.txt'. The trace input to the cache simulator is present in 'test/testx.txt' along with a random N-bit hex address generator (locality of the addresses cannot be determined here due to randomness).

### Directly Mapped Cache
The hex memory addresses from the trace file are extracted one by one. For each address, the Block Offset bits, Index bits, and Tag bits are computed and extracted, and then converted to decimal values. Initially, all lines in the cache are set to -1 to indicate they are empty. The cache line corresponding to the calculated Index is checked. If the cache line contains -1, indicating that it is empty, the cache is initialized with the current Tag, and the miss count is incremented. If the cache line is already occupied and the Tag matches the current address's Tag, a hit is recorded, and the hit counter is updated. If the Tag does not match, this results in a miss, the miss count is updated, and the Tag is stored in the cache line, replacing the previous Tag.

'''
- If cache (index) == -1
   - cache (index) = tag
   - miss++   
- else if cache (index) == tag
   - hits++
- else
   - cache (index) == tag
   - miss++
'''