print_usage ()
{
    echo "Usage: run.sh [repeat-count]"
}

reps_count="${1:-1}"

TIMEFORMAT="Execution time (threading, reps=${reps_count}) real=%R user=%U sys=%S"
time {
    for i in $(seq ${reps_count}); do
        ./target/threading
    done
}
