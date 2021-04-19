from setuptools import setup, find_packages, Extension

setup(
    name='mykmeanssp',
    version='0.1.0',
    author="Guy Dekel and David Molina",
    description="Api for Clang implemented Kmeans algorithm without initialization of centroids",
    install_requires=['invoke'],
    packages=find_packages(),

    ext_modules=[
        Extension(
            'mykmeanssp',
            ['kmeans.c'],
        ),
    ]
)