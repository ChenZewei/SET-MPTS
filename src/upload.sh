DESTINATION=/home/hadoop/SET-MRTS
SET_MRTS_HOME=/home/hadoop/works/SET-MRTS
PWD=.
CONFIG_FOLDER=$PWD/config
EXEC_FILE=$PWD/SchedulingAnalysis

scp $CONFIG_FORDER hadoop@czw-pc:$DESTINATION
scp ./SchedulingAnalyusis hadoop@czw-pc:$DESTINATION
scp ./start.sh hadoop@czw-pc:$DESTINATION

#scp $CONFIG_FILE hadoop@czw-dorm:/home/hadoop/exec
#scp $EXEC_FILE hadoop@czw-dorm:/home/hadoop/exec

#ssh -t hadoop@czw-dorm "$DESTINATION/start.sh"



