#!/usr/bin/python

import os
import sys
from argparse import ArgumentParser

import networkx as nx
from jinja2 import Environment, FileSystemLoader

from cm_to_ma import cm_to_ma

TEMPLATE_DIR = 'files'
SRC_DIR = 'src'
MODEL_DIR = 'model'
MA_FILENAME = 'model.ma'


def parse_options():
    parser = ArgumentParser()

    parser.add_argument('-m', '--max-degree', dest='maxDegree', action='store',
                        default=10, type=int, help='Max node degree')

    parser.add_argument('-c', '--cdpp-directory', dest='kernel_dir', action='store',
                        default=None, type=str, help='CD++ kernel directory', required=True)

    parser.add_argument('-o', '--output-directory', dest='out_dir', action='store',
                        default='out', type=str, help='Output directory')

    return parser.parse_args()

def create_dirs_if_missing(out_dir):
    src_path = os.path.join(out_dir, SRC_DIR)
    if not os.path.exists(src_path):
        os.mkdir(src_path)


def generate_node_models(max_degree,
                         output_dir,
                         kernel_dir):
    env = Environment(
        loader=FileSystemLoader(TEMPLATE_DIR)
    )

    h_template = env.get_template('node-template.h')

    for i in range(1, max_degree+1):
        with open(output_dir+"/%s/node%d.h" % (SRC_DIR, i), "w") as f:
            f.write(h_template.render(n=i))

    cpp_template = env.get_template('node-template.cpp')

    for i in range(1, max_degree+1):
        with open(output_dir+"/%s/node%d.cpp" % (SRC_DIR, i), "w") as f:
            f.write(cpp_template.render(n=i))

    reg_template=env.get_template('reg-template.cpp')
    with open(output_dir+"/%s/reg.cpp" % SRC_DIR, "w") as f:
        f.write(reg_template.render(n=max_degree))

    makefile_template=env.get_template('Makefile-template')
    with open(output_dir+"/%s/Makefile" % SRC_DIR, "w") as f:
        f.write(makefile_template.render(kernel_dir=kernel_dir))
    pass


if __name__ == '__main__':
    options=parse_options()
    create_dirs_if_missing(options.out_dir)
    generate_node_models(options.maxDegree, options.out_dir, options.kernel_dir)