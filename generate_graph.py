import networkx as nx
import random

def generate_random_dag(num_nodes, num_edges, output_file):
    # Create a random directed graph
    G = nx.gn_graph(num_nodes, seed=random.randint(0, 1000), create_using=nx.DiGraph)

    # Add random edges to make it a DAG
    for _ in range(num_edges - num_nodes + 1):
        u, v = random.sample(range(num_nodes), 2)
        if not G.has_edge(u, v) and not G.has_edge(v, u):
            G.add_edge(u, v)

    # Ensure the graph is a DAG
    G = nx.DiGraph([(u, v) for u, v in G.edges() if u < v])

    # Write the graph to a file
    with open(output_file, 'w') as f:
        for u, v in G.edges():
            f.write(f"{u} {v}\n")

def generate_and_save_graphs():
    sizes = [100, 1000, 10000, 20000]
    for size in sizes:
        num_edges = size * 5  # Example: 5 edges per node
        output_file = f"random_dag_{size}.txt"
        generate_random_dag(size, num_edges, output_file)

if __name__ == "__main__":
    generate_and_save_graphs()