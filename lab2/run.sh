print_usage ()
{
    echo "Usage: run.sh [threads-count] [repeat-count]"
}

threads="${1:-1}"
reps_count="${2:-1}"

TIMEFORMAT="Execution time (threading, threads=${threads}, reps=${reps_count}) real=%R sec, user=%U sec, sys=%S sec"
time {
    for i in $(seq ${reps_count}); do
        ./target/threading "${threads}"
    done
}
