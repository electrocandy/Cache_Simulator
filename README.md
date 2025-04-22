# Cache Simulator

## Mapping and Replacement Polices
- Directly Mapped Cache
- Fully Associative Cache (LRU)
- Two-Level 4-Way Set Associative Cache (LRU)

## Working of the Simulator
The Main Memory here is defined to be 1MB which can be changed in the macro `Main_mem` in `src/cache_sim.cpp`. The cache parameters are calculated using the parameters defined (in their respective sizes) in `config/cache_config.txt`. The trace input to the cache simulator is present in `test/testx.txt` along with a random N-bit hex address generator (locality of the addresses cannot be determined here due to randomness).

### Directly Mapped Cache
The hex memory addresses from the trace file are extracted one by one. For each address, the Block Offset bits, Index bits, and Tag bits are computed and extracted, and then converted to decimal values. Initially, all lines in the cache are set to `-1` to indicate they are empty. The cache line corresponding to the calculated Index is checked. If the cache line contains `-1`, indicating that it is empty, the cache is initialized with the current Tag, and the miss count is incremented. If the cache line is already occupied and the Tag matches the current address's Tag, a hit is recorded, and the hit counter is updated. If the Tag does not match, this results in a miss, the miss count is updated, and the Tag is stored in the cache line, replacing the previous Tag.

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
For each hex memory address, the Block Offset bits and Tag Bits are computed, extracted, and converted to decimal. Now for each address, the the cache is run through entirely (Worst case) . In this approach, there is no specific mapping order for cache lines. Initially, all the lines in cache is set to `-1`. If a cache line contains `-1`, the tag is assigned to that line, and the miss count is updated. The tag is also added to the end of the LRU tracker (recently accessed). If the tag matches an existing cache line, the hit count is incremented, and the tag's previous position in the LRU tracker is deleted and appended to the end (recently accessed). If both conditions are false, the cache is checked for the number of accesses (equal to the number of lines in the cache, if full). If the cache is full, the least recently used tag (front tag) is evicted using LRU eviction policy, and the new tag replaces it.

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