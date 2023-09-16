print_usage ()
{
    echo "Usage: run.sh {threading|threading-extended} [repeat-count]"
}

run_type="${1}"
reps_count="${2}"

case "${run_type}" in
    threading)
        TIMEFORMAT="Execution time (threading) real=%R user=%U sys=%S"
        time {
            for i in $(seq ${reps_count:-100}); do
                ./target/threading
            done
        }
    ;;
    *)
        print_usage
        exit 1
    ;;
esac
