# Cache Simulator

## Mapping and Replacement Polices
- Directly Mapped Cache
- Fully Associative Cache (LRU)
- Two-Level 4-Way Set Associative Cache (LRU)

## Working of the Simulator
The Main Memory here is defined to be 1MB which can be changed in the macro `Main_mem` in `src/cache_sim.cpp`. The cache parameters are calculated using the parameters defined (in their respective sizes) in `config/cache_config.txt`. The trace input to the cache simulator is present in `test/testx.txt` along with a random N-bit hex address generator (locality of the addresses cannot be determined here due to randomness).

### Directly Mapped Cache
The hex memory addresses from the trace file are extracted one by one. For each address, the Block Offset bits, Index bits, and Tag bits are computed and extracted, and then converted to decimal values.

 Initially, all lines in the cache are set to `-1` to indicate they are empty. The cache line corresponding to the calculated Index is checked. If the cache line contains `-1`, indicating that it is empty, the cache is initialized with the current Tag, and the miss count is incremented. If the cache line is already occupied and the Tag matches the current address's Tag, a hit is recorded, and the hit counter is updated. If the Tag does not match, this results in a miss, the miss count is updated, and the Tag is stored in the cache line, replacing the previous Tag.

```
- If cache (index) == -1
   - cache (index) = tag
   - miss++   
- else if cache (index) == tag
   - hits++
- else
   - cache (index) == tag
   - miss++
```

### Fully Associative Cache
For each hex memory address, the Block Offset bits and Tag Bits are computed, extracted, and converted to decimal. Now for each address, the the cache is run through entirely (Worst case) . In this approach, there is no specific mapping order for cache lines.
 
 Initially, all the lines in cache is set to `-1`. If a cache line contains `-1`, the tag is assigned to that line, and the miss count is updated. The tag is also added to the end of the LRU tracker (recently accessed). If the tag matches an existing cache line, the hit count is incremented, and the tag's previous position in the LRU tracker is deleted and appended to the end (recently accessed). If both conditions are false, the cache is checked for the number of accesses (equal to the number of lines in the cache, if full). If the cache is full, the least recently used tag (front tag) is evicted using LRU eviction policy, and the new tag replaces it.

The disadvantage in fully associative cache is that, the cache has to be scanned through (Worst case) for each address, increasing its running time.

```
- Check every cache line for each address
  - If cache (line_number) == -1
   - cache (line_number) = tag
   - miss++
   - Update LRU tracker
  - If cache (line_number) == tag
   - hits++
   - Update LRU tracker
  - If both the above are not true
   - miss++
   - Use LRU eviction policy
```

### N-Ways Set Associative Cache
For each hex address, the Block Offset bits, the Set bits and tag bits are computed, extracted and converted to decimal, for both the cache levels. The entire cache is divided into sets, which each set consisting of N lines. All the cache lines are initialized with `-1`. Two-Level cache hierarchy is implemented with same associativity for both cache levels. Set in which the tag is to be placed is calculated and search is done in the lines in the particular set only. There are two LRU trackers, one specifically for each level.

The set in L1 is first checked. If the line in the set is `-1`, then cache line is updated with the tag, the LRU tracker is updated with the tag and miss count is updated. Then, the sets in L2 is checked, if the line in the set of L2 is `-1`, then that cache line is updated with the tag, the LRU tracker is updated and the miss count is updated (Both are compulsory misses). Then, the L1 cache is checked for hit and the LRU tracker is updated. If there is a L1 miss, then L2 is checked for the tag. If there is L2 hit, then the respective LRU tracker is updated and the tag is moved to L1 and stored based on recency of address in LRU tracker. If there is L2 miss, then miss count is updated and LRU eviction is performed.

Advantageous compared to both Directly mapped and Fully Associative cache, as it combines both of them and strikes balance between both.

'''
- Set number is computed for L1 and L2
- Check cache lines in L1 of the set
  - If L1 cache (set)(line_number) == -1 //L1 Compulsory miss
    - miss++
    - Update LRU tracker of L1
    - If L2 cache (set)(line_number) == -1 //L2 Compulsory miss
      - miss1++
      - Update LRU tracker of L2
  - If L1 cache (set)(line_number) == tag //L1 hit
    - hits++
    - Update LRU tracker of L1
  - If both the above are not true //L1 miss
    - miss++
    - If L1 cache (set)(line_number) == tag //L2 hit
      - hits1++
      - Update LRU tracker of L2
      - Move tag to L1
    - If above condition not true //L2 miss
      - miss1++
      - Move tag to L2
'''