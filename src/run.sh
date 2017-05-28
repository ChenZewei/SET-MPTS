SET_MRTS_HOME=/home/hadoop/works/SET-MRTS
CONFIG_FILE=$SET_MRTS_HOME/src/config.xml
EXEC_FILE=$SET_MRTS_HOME/src/SchedulingAnalysis
#scp $CONFIG_FILE hadoop@czw-dorm:/home/hadoop/exec
#scp $EXEC_FILE hadoop@czw-dorm:/home/hadoop/exec

CORE_NUM=`ssh -t hadoop@czw-dorm 'cat /proc/cpuinfo | grep "processor" | wc -l'`

echo $CORE_NUM

#ssh -t hadoop@czw-dorm 'exec/start.sh'



