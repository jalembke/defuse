from pyspark import SparkContext

sc = SparkContext()
rdd = sc.textFile("file:///mnt/test/wikipedia_50GB/*")
counts = rdd.flatMap(lambda line: line.split(" ")).map(lambda word: (word, 1)).reduceByKey(lambda a, b: a + b)
counts.saveAsTextFile("/mnt/test/spark/results")
