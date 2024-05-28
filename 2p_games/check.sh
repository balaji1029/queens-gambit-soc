for ((i=0; i<9; i++)); do
    out=$(./linux_executable.out < "testcases/input$i.txt")
    if [[ "$out" != $(cat "testcases/output$i.txt") ]]; then
        echo "Test $i failed"
        echo "Expected: $(cat testcases/output$i.txt)"
        echo "Got: $out"
    else
        echo "Test $i passed"
    fi
done