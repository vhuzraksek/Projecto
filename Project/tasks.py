from invoke import task

@task
def build(c):
    c.run("python3.8.5 setup.py build_ext --inplace")

@task(aliases=["del"])
def delete(c):
    c.run("rm *mykmeanssp*.so")

@task
def run(c, k, n, random=False):
	#c.run("python3.8.5 setup.py build_ext --inplace")
	c.run("python3.8.5 main.py {k} {n} {random}".format(k=k, n=n, random=random))
