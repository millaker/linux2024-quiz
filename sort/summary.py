# Define dictionaries to store sums and counts for each region
region_sums = {'td_sort': {'cpu-cycles': 0, 'cache-misses': 0, 'cache-references': 0, 'time-elapsed': 0, 'comparisons': 0, 'l1d-read-accesses': 0, 'l1d-read-misses':0},
               'list_sort': {'cpu-cycles': 0, 'cache-misses': 0, 'cache-references': 0, 'time-elapsed': 0, 'comparisons': 0, 'l1d-read-accesses': 0, 'l1d-read-misses':0}}
region_counts = {'td_sort': 0, "list_sort": 0}

# Open and read the file
with open('results.txt', 'r') as file:
    # Skip the header line
    next(file)
    next(file)
    next(file)
    
    # Process each line in the file
    for line in file:
        if line[0] == '+':
            continue
        # Split the line into fields
        fields = line.split('|')
        region = fields[1].strip()
        cpu_cycles = int(fields[2].strip())
        cache_misses = int(fields[3].strip())
        cache_references = int(fields[4].strip())
        l1d_accesses = int(fields[5].strip())
        l1d_misses = int(fields[6].strip())
        time_elapsed = float(fields[7].strip()[:-2])  # Remove 'ms' from the time
        
        # Update sums and counts for the corresponding region
        region_sums[region]['cpu-cycles'] += cpu_cycles
        region_sums[region]['cache-misses'] += cache_misses
        region_sums[region]['cache-references'] += cache_references
        region_sums[region]['l1d-read-accesses'] += l1d_accesses
        region_sums[region]['l1d-read-misses'] += l1d_misses
        region_sums[region]['time-elapsed'] += time_elapsed
        region_counts[region] += 1
    
with open('cmps_results.txt', 'r') as file:
    for line in file:
        fields = line.split(':')
        region = fields[0].strip()
        cmps = int(fields[1].strip())
        region_sums[region]['comparisons'] += cmps


# Calculate averages for each region
region_averages = {}
for region, sum_values in region_sums.items():
    count = region_counts[region]
    averages = {key: value / count for key, value in sum_values.items()}
    region_averages[region] = averages

# Print the results
for region, averages in region_averages.items():
    print(f"Region: {region}")
    for key, value in averages.items():
        print(f"Average {key}: {value:.2f}")
    print()
