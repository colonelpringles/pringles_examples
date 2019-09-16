import networkx as nx
import random
import time
from typing import List


class RandomGraphGenerator():
    def __init__(self, size: int):
        """Instantiates a random configuration model based graph generator.

        :param size: The number of vertexes
        :type size: int
        """
        self.size = size

    def generate(self) -> nx.Graph:
        """Do generate the graph.

        :return: The randomly generated graph.
        :rtype: nx.Graph
        """
        # Generate seed taking into account seconds, and exagerating
        # randomness with sift left
        seed = int(time.time() * 1000) << 2
        return nx.configuration_model(self.__generate_random_degree_sequence(),
                                      seed=seed)

    def __generate_random_degree_sequence(self) -> List[float]:
        alpha = 10
        beta = 1
        degree_sequence = [int(random.gammavariate(alpha, beta))
                           for i in range(self.size)]
        if sum(degree_sequence) % 2 == 1:
            degree_sequence[-1] += 1
        return degree_sequence
