for ((i=0; i<=10; i++)); do
    echo "--- Test $i ---"
    out=$(python3 -u "/home/balaji/Desktop/queens_gambit/greedy_or_not/$1.py" < "testcases/input$i.txt")
    if [[ "$out" != $(cat "testcases/output$i.txt") ]]; then
        echo "Test $i failed"
        echo "Expected: $(cat testcases/output$i.txt)"
        echo "Got: $out"
    else
        echo "Test $i passed"
    fi
    echo "---"
done