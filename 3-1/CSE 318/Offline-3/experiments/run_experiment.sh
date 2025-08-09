#!/bin/bash

# Chain Reaction AI Experiment Runner
# Run from experiments directory

# Create directories and initialize CSV files immediately
mkdir -p "results"
mkdir -p "logs"

RESULTS_DIR="results"
GAME_INFO_DIR="../game-info"
AI_ENGINE="../game-logic/ai_engine"
HEURISTIC_CSV="$RESULTS_DIR/heuristic_comparison.csv"
RANDOM_CSV="$RESULTS_DIR/random_vs_ai.csv"

# Initialize CSV files with headers
echo "game_id,red_heuristic,blue_heuristic,red_depth,blue_depth,red_time_limit,blue_time_limit,winner,game_duration_sec,total_moves,red_avg_move_time,blue_avg_move_time,red_timeouts,blue_timeouts" > "$HEURISTIC_CSV"
echo "game_id,ai_heuristic,ai_depth,ai_time_limit,winner,game_duration_sec,total_moves,ai_avg_move_time,random_avg_move_time,ai_timeouts,random_timeouts" > "$RANDOM_CSV"

# Kill any existing AI processes
pkill -f "ai_engine" 2>/dev/null || true
sleep 1

# Custom timeout function for systems without timeout command
run_with_timeout() {
    local timeout_duration=$1
    local func_name=$2
    shift 2
    local func_args=("$@")
    
    # Create a temporary script to run the function
    local temp_script="/tmp/game_runner_$_$(date +%s).sh"
    
    # Export all necessary variables and functions to the temp script
    cat > "$temp_script" << 'SCRIPT_EOF'
#!/bin/bash

# Re-source all variables and functions
RESULTS_DIR="results"
GAME_INFO_DIR="../game-info"
AI_ENGINE="../game-logic/ai_engine"
HEURISTIC_CSV="$RESULTS_DIR/heuristic_comparison.csv"
RANDOM_CSV="$RESULTS_DIR/random_vs_ai.csv"
SCRIPT_EOF
    
    # Append the function definitions to temp script
    declare -f run_game >> "$temp_script"
    declare -f run_random_game >> "$temp_script"
    declare -f parse_game_results >> "$temp_script"
    declare -f parse_random_results >> "$temp_script"
    
    # Add the function call
    echo "$func_name $(printf '%q ' "${func_args[@]}")" >> "$temp_script"
    
    chmod +x "$temp_script"
    
    # Run the script in background
    "$temp_script" &
    local script_pid=$!
    
    # Wait for either completion or timeout
    local elapsed=0
    while kill -0 $script_pid 2>/dev/null; do
        if [ $elapsed -ge $timeout_duration ]; then
            echo "Game timed out after ${timeout_duration} seconds - killing and skipping"
            
            # Kill the script and any child processes
            pkill -P $script_pid 2>/dev/null || true
            kill $script_pid 2>/dev/null || true
            pkill -f "ai_engine" 2>/dev/null || true
            
            # Clean up temp script
            rm -f "$temp_script"
            return 124  # timeout exit code
        fi
        sleep 1
        ((elapsed++))
    done
    
    # Function completed normally
    wait $script_pid
    local exit_code=$?
    
    # Clean up temp script
    rm -f "$temp_script"
    return $exit_code
}

# Function to run a single AI vs AI game
run_game() {
    local red_heuristic=$1
    local blue_heuristic=$2
    local red_depth=$3
    local blue_depth=$4
    local red_time_limit=$5
    local blue_time_limit=$6
    local game_id=$7
    
    echo "Running game $game_id: H$red_heuristic(d$red_depth,${red_time_limit}ms) vs H$blue_heuristic(d$blue_depth,${blue_time_limit}ms)"
    
    # Kill any existing processes
    pkill -f "ai_engine" 2>/dev/null || true
    sleep 1
    
    # Set AI vs AI mode
    echo "AI_VS_AI" > "$GAME_INFO_DIR/game_mode.txt"
    
    # Initialize empty board
    echo "AI Move:" > "$GAME_INFO_DIR/gamestate.txt"
    for i in {1..9}; do
        echo "0 0 0 0 0 0" >> "$GAME_INFO_DIR/gamestate.txt"
    done
    
    # Start timing
    start_time=$(date +%s)
    
    # Start Red AI in background
    cd ../game-logic && ./ai_engine RED $red_depth $red_heuristic $red_time_limit > "../experiments/logs/red_${game_id}.log" 2>&1 &
    red_pid=$!
    
    sleep 2
    
    # Start Blue AI in foreground
    cd ../game-logic && ./ai_engine BLUE $blue_depth $blue_heuristic $blue_time_limit > "../experiments/logs/blue_${game_id}.log" 2>&1
    
    # End timing
    end_time=$(date +%s)
    game_duration=$((end_time - start_time))
    
    # Kill any remaining processes
    kill $red_pid 2>/dev/null || true
    pkill -f "ai_engine" 2>/dev/null || true
    
    # Parse results
    parse_game_results "$game_id" "$red_heuristic" "$blue_heuristic" "$red_depth" "$blue_depth" "$red_time_limit" "$blue_time_limit" "$game_duration"
}

# Function to run random vs AI game
run_random_game() {
    local ai_heuristic=$1
    local ai_depth=$2
    local ai_time_limit=$3
    local game_id=$4
    
    echo "Running random game $game_id: H7 vs H$ai_heuristic(d$ai_depth,${ai_time_limit}ms)"
    
    # Kill any existing processes
    pkill -f "ai_engine" 2>/dev/null || true
    sleep 1
    
    # Set AI vs AI mode
    echo "AI_VS_AI" > "$GAME_INFO_DIR/game_mode.txt"
    
    # Initialize empty board
    echo "AI Move:" > "$GAME_INFO_DIR/gamestate.txt"
    for i in {1..9}; do
        echo "0 0 0 0 0 0" >> "$GAME_INFO_DIR/gamestate.txt"
    done
    
    # Start timing
    start_time=$(date +%s)
    
    # Start Random AI (H7) as Red in background
    cd ../game-logic && ./ai_engine RED 1 7 1000 > "../experiments/logs/random_${game_id}.log" 2>&1 &
    random_pid=$!
    
    sleep 2
    
    # Start Smart AI as Blue in foreground
    cd ../game-logic && ./ai_engine BLUE $ai_depth $ai_heuristic $ai_time_limit > "../experiments/logs/ai_${game_id}.log" 2>&1
    
    # End timing
    end_time=$(date +%s)
    game_duration=$((end_time - start_time))
    
    # Kill any remaining processes
    kill $random_pid 2>/dev/null || true
    pkill -f "ai_engine" 2>/dev/null || true
    
    # Parse results for random game
    parse_random_results "$game_id" "$ai_heuristic" "$ai_depth" "$ai_time_limit" "$game_duration"
}

# Function to parse game results
parse_game_results() {
    local game_id=$1
    local red_h=$2
    local blue_h=$3
    local red_d=$4
    local blue_d=$5
    local red_t=$6
    local blue_t=$7
    local duration=$8
    
    # Change to experiments directory for correct log paths
    cd ../experiments
    
    # Parse winner from logs
    local winner="UNKNOWN"
    if grep -q "Game Over! Winner: Red" "logs/red_${game_id}.log" "logs/blue_${game_id}.log" 2>/dev/null; then
        winner="Red"
    elif grep -q "Game Over! Winner: Blue" "logs/red_${game_id}.log" "logs/blue_${game_id}.log" 2>/dev/null; then
        winner="Blue"
    fi
    
    # Count moves from logs
    local red_moves=$(grep -c "AI making move" "logs/red_${game_id}.log" 2>/dev/null || echo "0")
    local blue_moves=$(grep -c "AI making move" "logs/blue_${game_id}.log" 2>/dev/null || echo "0")
    local total_moves=$((red_moves + blue_moves))
    
    # Calculate average move times - extract from "making move at (x, y) in Xms" lines
    local red_avg_time=0
    local blue_avg_time=0
    
    if [ -f "logs/red_${game_id}.log" ]; then
        red_avg_time=$(grep "making move at.*in [0-9]*ms" "logs/red_${game_id}.log" 2>/dev/null | \
                      grep -oE "in [0-9]+ms" | \
                      grep -oE "[0-9]+" | \
                      awk '{sum+=$1; count++} END {if(count>0) print sum/count; else print 0}')
    fi
    
    if [ -f "logs/blue_${game_id}.log" ]; then
        blue_avg_time=$(grep "making move at.*in [0-9]*ms" "logs/blue_${game_id}.log" 2>/dev/null | \
                       grep -oE "in [0-9]+ms" | \
                       grep -oE "[0-9]+" | \
                       awk '{sum+=$1; count++} END {if(count>0) print sum/count; else print 0}')
    fi
    
    # Count timeouts
    local red_timeouts=$(grep -c "time limit" "logs/red_${game_id}.log" 2>/dev/null || echo "0")
    local blue_timeouts=$(grep -c "time limit" "logs/blue_${game_id}.log" 2>/dev/null || echo "0")
    
    # Default values if parsing fails
    red_avg_time=${red_avg_time:-0}
    blue_avg_time=${blue_avg_time:-0}
    
    # Write to CSV
    echo "$game_id,$red_h,$blue_h,$red_d,$blue_d,$red_t,$blue_t,$winner,$duration,$total_moves,$red_avg_time,$blue_avg_time,$red_timeouts,$blue_timeouts" >> "../experiments/$HEURISTIC_CSV"
}

# Function to parse random game results
parse_random_results() {
    local game_id=$1
    local ai_h=$2
    local ai_d=$3
    local ai_t=$4
    local duration=$5
    
    # Change to experiments directory for correct log paths
    cd ../experiments
    
    # Parse winner from logs
    local winner="UNKNOWN"
    if grep -q "Game Over! Winner: Red" "logs/random_${game_id}.log" "logs/ai_${game_id}.log" 2>/dev/null; then
        winner="Random"
    elif grep -q "Game Over! Winner: Blue" "logs/random_${game_id}.log" "logs/ai_${game_id}.log" 2>/dev/null; then
        winner="AI"
    fi
    
    # Count moves from logs
    local random_moves=$(grep -c "AI making move" "logs/random_${game_id}.log" 2>/dev/null || echo "0")
    local ai_moves=$(grep -c "AI making move" "logs/ai_${game_id}.log" 2>/dev/null || echo "0")
    local total_moves=$((random_moves + ai_moves))
    
    # Calculate average move times - extract from "making move at (x, y) in Xms" lines
    local ai_avg_time=0
    local random_avg_time=0
    
    if [ -f "logs/ai_${game_id}.log" ]; then
        ai_avg_time=$(grep "making move at.*in [0-9]*ms" "logs/ai_${game_id}.log" 2>/dev/null | \
                     grep -oE "in [0-9]+ms" | \
                     grep -oE "[0-9]+" | \
                     awk '{sum+=$1; count++} END {if(count>0) print sum/count; else print 0}')
    fi
    
    if [ -f "logs/random_${game_id}.log" ]; then
        random_avg_time=$(grep "making move at.*in [0-9]*ms" "logs/random_${game_id}.log" 2>/dev/null | \
                         grep -oE "in [0-9]+ms" | \
                         grep -oE "[0-9]+" | \
                         awk '{sum+=$1; count++} END {if(count>0) print sum/count; else print 0}')
    fi
    
    # Count timeouts
    local ai_timeouts=$(grep -c "time limit" "logs/ai_${game_id}.log" 2>/dev/null || echo "0")
    local random_timeouts=$(grep -c "time limit" "logs/random_${game_id}.log" 2>/dev/null || echo "0")
    
    # Default values if parsing fails
    ai_avg_time=${ai_avg_time:-0}
    random_avg_time=${random_avg_time:-0}
    
    # Write to CSV
    echo "$game_id,$ai_h,$ai_d,$ai_t,$winner,$duration,$total_moves,$ai_avg_time,$random_avg_time,$ai_timeouts,$random_timeouts" >> "../experiments/$RANDOM_CSV"
}

# Main execution
main() {
    echo "Starting Chain Reaction AI Experiments"
    echo "======================================="
    
    local game_counter=1
    local skipped_games=0
    
    # Heuristic Comparison (H1-H6 vs H1-H6)
    echo "Running Heuristic Comparison experiments..."
    
    local depths=(1 2 3)
    local time_limits=(5000 10000)
    
    for red_h in {1..6}; do
        for blue_h in {1..6}; do
            if [ $red_h -ne $blue_h ]; then  # Only different heuristics
                for depth in "${depths[@]}"; do
                    for time_limit in "${time_limits[@]}"; do
                        for game in {1..3}; do
                            echo "Starting heuristic game H_${game_counter} (timeout: 2 minutes)"
                            
                            # Run game with 2 minute timeout using custom function
                            run_with_timeout 30 run_game $red_h $blue_h $depth $depth $time_limit $time_limit "H_${game_counter}"
                            
                            if [ $? -eq 124 ]; then
                                echo "Game H_${game_counter} timed out after 2 minutes - skipping CSV write"
                                ((skipped_games++))
                                # Clean up any remaining processes
                                pkill -f "ai_engine" 2>/dev/null || true
                            else
                                echo "Game H_${game_counter} completed successfully"
                            fi
                            
                            ((game_counter++))
                        done
                    done
                done
            fi
        done
    done
    
    # Random vs AI (H7 vs H1-H6)
    echo "Running Random vs AI experiments..."
    
    game_counter=1
    
    for ai_h in {1..6}; do
        for depth in "${depths[@]}"; do
            for time_limit in "${time_limits[@]}"; do
                for game in {1..3}; do
                    echo "Starting random game R_${game_counter} (timeout: 2 minutes)"
                    
                    # Run random game with 2 minute timeout using custom function
                    run_with_timeout 30 run_random_game $ai_h $depth $time_limit "R_${game_counter}"
                    
                    if [ $? -eq 124 ]; then
                        echo "Random game R_${game_counter} timed out after 2 minutes - skipping CSV write"
                        ((skipped_games++))
                        # Clean up any remaining processes
                        pkill -f "ai_engine" 2>/dev/null || true
                    else
                        echo "Random game R_${game_counter} completed successfully"
                    fi
                    
                    ((game_counter++))
                done
            done
        done
    done
    
    echo "Experiments completed!"
    echo "Results saved to:"
    echo "  - $HEURISTIC_CSV"
    echo "  - $RANDOM_CSV"
    echo "Total games skipped due to timeout: $skipped_games"
    
    # Clean up any remaining processes
    pkill -f "ai_engine" 2>/dev/null || true
}

# Run main function
main "$@"