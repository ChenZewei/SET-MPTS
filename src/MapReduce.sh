HDFS_HOME=hdfs://czw-laptop:9000/user/hadoop
PWD=/user/hadoop/exec
HADOOP_STREAMING=$HADOOP_HOME/share/hadoop/tools/lib/hadoop-streaming-2.7.3.jar
INPUT=$HDFS_HOME/input
OUTPUT=$HDFS_HOME/output

hadoop fs -rm -r /user/hadoop/output

hadoop jar $HADOOP_STREAMING \
-fs hdfs://czw-laptop:9000/	\
-files master,slave,config.xml	\
-input	$INPUT		\
-output	$OUTPUT		\
-mapper	master		\
-reducer "cat"
