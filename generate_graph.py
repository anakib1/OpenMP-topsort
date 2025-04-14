import numpy as np
import random
from tqdm import tqdm

def generate_random_dag_efficient(num_nodes, avg_edges_per_node, output_file):
    print(f"Generating DAG with {num_nodes} nodes...")
    
    # Use numpy for efficient array operations
    edges_per_node = np.random.poisson(avg_edges_per_node, num_nodes)
    total_edges = np.sum(edges_per_node)
    
    print(f"Writing {total_edges} edges to file...")
    with open(output_file, 'w') as f:
        # Process nodes in chunks to save memory
        chunk_size = 10000
        for start in tqdm(range(0, num_nodes, chunk_size)):
            end = min(start + chunk_size, num_nodes)
            chunk_nodes = range(start, end)
            
            for u in chunk_nodes:
                # Generate edges for this node
                num_edges = edges_per_node[u]
                if num_edges > 0:
                    # Only connect to nodes with higher indices to ensure DAG property
                    possible_targets = range(u + 1, num_nodes)
                    if possible_targets:
                        # Sample without replacement
                        targets = random.sample(range(u + 1, num_nodes), 
                                             min(num_edges, num_nodes - u - 1))
                        for v in targets:
                            f.write(f"{u} {v}\n")

def generate_and_save_graphs():
    # Original sizes plus 10^6
    sizes = [100, 1000, 10000, 20000, 1000000]
    
    for size in sizes:
        print(f"\nGenerating graph with {size} vertices...")
        
        # Adjust average edges per node based on graph size
        if size <= 1000:
            avg_edges = 5
        elif size <= 10000:
            avg_edges = 4
        elif size <= 20000:
            avg_edges = 3
        else:
            avg_edges = 2  # Fewer edges per node for very large graphs
            
        output_file = f"random_dag_{size}.txt"
        generate_random_dag_efficient(size, avg_edges, output_file)
        print(f"Saved to {output_file}")

if __name__ == "__main__":
    generate_and_save_graphs()