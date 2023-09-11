print_usage ()
{
    echo "Usage: run.sh {processing|processing-unix|threading|threading-extended} [repeat-count]"
}

run_type="${1}"
reps_count="${2}"

case "${run_type}" in
    processing)
        TIMEFORMAT="Execution time (processing) real=%R user=%U sys=%S"
        time {
            for i in $(seq ${reps_count:-100}); do
                ./target/processing
            done
            mv assets/out.txt target/assets/out.processing.txt
        }
    ;;
    processing-unix)
        TIMEFORMAT="Execution time (processing) real=%R user=%U sys=%S"
        time {
            for i in $(seq ${reps_count:-10}); do
                ./target/read_process &
                sleep 0.1
                ./target/calc_process &
                sleep 0.1
                ./target/write_process &
            done
            mv assets/out.txt target/assets/out.unix.txt
        }
    ;;
    threading)
        TIMEFORMAT="Execution time (threading) real=%R user=%U sys=%S"
        time {
            for i in $(seq ${reps_count:-100}); do
                ./target/threading
            done
            mv assets/out.txt target/assets/out.threading.txt
        }
    ;;
    threading-extended)
        TIMEFORMAT="Execution time (threading-extended) real=%R user=%U sys=%S"
        time {
            for i in $(seq ${reps_count:-100}); do
                ./target/threading_extended
            done
            mv assets/out.txt target/assets/out.extended.txt
        }
    ;;
    *)
        print_usage
        exit 1
    ;;
esac
