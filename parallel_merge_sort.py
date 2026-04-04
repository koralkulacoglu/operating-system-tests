import concurrent.futures
import time

def merge(left, right):
    result = []
    i = j = 0
    while i < len(left) and j < len(right):
        if left[i] < right[j]:
            result.append(left[i])
            i += 1
        else:
            result.append(right[j])
            j += 1
    result.extend(left[i:])
    result.extend(right[j:])
    return result

def merge_sort(arr):
    if len(arr) <= 1:
        return arr
    mid = len(arr) // 2
    return merge(merge_sort(arr[:mid]), merge_sort(arr[mid:]))

def parallel_merge_sort(data, n_parts=4):
    """
    Splits the data into n_parts, sorts them in parallel, 
    then merges the results in the main process.
    """
    chunk_size = len(data) // n_parts
    chunks = [data[i:i + chunk_size] for i in range(0, len(data), chunk_size)]

    with concurrent.futures.ProcessPoolExecutor() as executor:
        # Submit the chunks to the pool for sorting
        sorted_chunks = list(executor.map(merge_sort, chunks))

    # Merge the sorted chunks back together
    while len(sorted_chunks) > 1:
        new_merged = []
        for i in range(0, len(sorted_chunks), 2):
            if i + 1 < len(sorted_chunks):
                new_merged.append(merge(sorted_chunks[i], sorted_chunks[i+1]))
            else:
                new_merged.append(sorted_chunks[i])
        sorted_chunks = new_merged

    return sorted_chunks[0]

if __name__ == "__main__":
    data = [i for i in range(10000, 0, -1)]
    
    start = time.time()
    sorted_data = merge_sort(data)
    end = time.time()
    sequential_time = end - start

    start = time.time()
    parallel_sorted_data = parallel_merge_sort(data, n_parts=4)
    end = time.time()
    parallel_time = end - start

    print(sorted_data == parallel_sorted_data)
    print(sequential_time)
    print(parallel_time)

