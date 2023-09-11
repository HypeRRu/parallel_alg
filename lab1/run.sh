print_usage ()
{
    echo "Usage: run.sh {processing|threading|threading-extended} [repeat-count]"
}

run_type="${1}"
reps_count="${2}"

case "${run_type}" in
    processing)
        TIMEFORMAT="Execution time (threading-extended) %R"
        time
        {
            for i in $(seq ${reps_count:-10}); do
                ./target/read_process &
                sleep 0.2
                ./target/calc_process &
                sleep 0.2
                ./target/write_process &
            done
        }
    ;;
    threading)
        TIMEFORMAT="Execution time (threading) %R"
        time {
            for i in $(seq ${reps_count:-100}); do
                ./target/threading
            done
            mv assets/out.txt assets/out.threading.txt
        }
    ;;
    threading-extended)
        TIMEFORMAT="Execution time (threading-extended) %R"
        time {
            for i in $(seq ${reps_count:-100}); do
                ./target/threading_extended
            done
            mv assets/out.txt assets/out.extended.txt
        }
    ;;
    *)
        print_usage
        exit 1
    ;;
esac
