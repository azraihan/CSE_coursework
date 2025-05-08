#!/bin/bash

# Function to display usage message
display_usage() {
    echo "Usage: $0 <submission_folder> <target_folder> <test_folder> <answer_folder> [options]"
    echo "Options:"
    echo "  -v           Verbose mode, print detailed information"
    echo "  -noexecute   Skip task C (execution and output matching)"
    echo "  -nolc        Do not calculate line count"
    echo "  -nocc        Do not calculate comment count"
    echo "  -nofc        Do not calculate function count"
    exit 1
}

# Checking if mandatory arguments are provided
if [ $# -lt 4 ]; then
    display_usage
fi

# Assigning mandatory arguments
submission_folder="$1"
target_folder="$2"
test_folder="$3"
answer_folder="$4"
shift 4


# Processing optional arguments
verbose=false
execute=true
calculate_lc=true
calculate_cc=true
calculate_fc=true

for arg in "$@"; do
    case $arg in
        -v)
            verbose=true
            ;;
        -noexecute)
            execute=false
            ;;
        -nolc)
            calculate_lc=false
            ;;
        -nocc)
            calculate_cc=false
            ;;
        -nofc)
            calculate_fc=false
            ;;
        *)
            echo "Unknown option: $arg"
            display_usage
            ;;
    esac
done

# Checking if target folder exists and removing
if [ -d "$target_folder" ]; then
    rm -rf "$target_folder"
fi

# Creating target directory and language subdirectories
mkdir -p "$target_folder/C" "$target_folder/C++" "$target_folder/Python" "$target_folder/Java"

# Initializing CSV headers
csv_header="student_id,student_name,language"
if [ "$execute" = true ]; then
    csv_header="$csv_header,matched,not_matched"
fi
if [ "$calculate_lc" = true ]; then
    csv_header="$csv_header,line_count"
fi
if [ "$calculate_cc" = true ]; then
    csv_header="$csv_header,comment_count"
fi
if [ "$calculate_fc" = true ]; then
    csv_header="$csv_header,function_count"
fi

# Writing CSV headers
echo "$csv_header" > "$target_folder/result.csv"

# Function to count lines in a file
count_lines() {
    wc -l < "$1" | tr -d ' '
}

# function to count comments
count_comments() {
    local file="$1"
    local extension="${file##*.}"
    
    case "$extension" in
        c|cpp|java)
            sed 's/"[^"]*"//g' "$file" | grep -c "//"
            ;;
        py)
            sed "s/\"[^\"]*\"//g; s/'[^']*'//g" "$file" | grep -c "#"
            ;;
        *)
            echo "0"
            ;;
    esac
}

# Function to count functions in a file
count_functions() {
    local file="$1"
    local extension="${file##*.}"
    
    case "$extension" in
        c)
            grep -c -E "^[[:alnum:]_]+[[:space:]]+[[:alnum:]_]+[[:space:]]*\([^;]*\)[[:space:]]*{" "$file"
            ;;
        cpp)
            grep -c -E "^[[:alnum:]_:]+[[:space:]]+[[:alnum:]_:]+[[:space:]]*\([^;]*\)[[:space:]]*{" "$file"
            ;;
        py)
            grep -c -E "^[[:space:]]*def[[:space:]]+[[:alnum:]_]+[[:space:]]*\(" "$file"
            ;;
        java)
            grep -c -E "^[[:space:]]*(public|private|protected|static|final|abstract)?[[:space:]]*(public|private|protected|static|final|abstract)?[[:space:]]*[[:alnum:]_]+[[:space:]]+[[:alnum:]_]+[[:space:]]*\([^;]*\)[[:space:]]*{" "$file"
            ;;
        *)
            echo "0"
            ;;
    esac
}

# temp dir for unzipped files
temp_dir=$(mktemp -d)

# total no of test cases
test_count=$(ls "$test_folder"/test*.txt 2>/dev/null | wc -l)

# Process each zip
for zip_file in "$submission_folder"/*.zip; do
    # Extracting the filename
    zip_filename=$(basename "$zip_file")
    
    # Extract student name and id
    if [[ $zip_filename =~ ([^_]+)_([0-9]{7})_submission_([0-9]{7})\.zip ]]; then
        student_name="${BASH_REMATCH[1]}"
        student_id="${BASH_REMATCH[3]}"
    else
        [ "$verbose" = true ] && echo "Skipping $zip_filename: Does not match expected format"
        continue
    fi
    
    # verbose output while processing files
    [ "$verbose" = true ] && echo "Organizing files of $student_id"
    
    # clean temp dir
    rm -rf "$temp_dir"/*
    
    # Unzip submission to temp dir
    unzip -q "$zip_file" -d "$temp_dir"
    
    # code files
    c_file=$(find "$temp_dir" -name "*.c")
    cpp_file=$(find "$temp_dir" -name "*.cpp")
    py_file=$(find "$temp_dir" -name "*.py")
    java_file=$(find "$temp_dir" -name "*.java")
    
    # determining the language and relevant attributes
    if [ -n "$c_file" ]; then
        language="C"
        source_file="$c_file"
        target_dir="$target_folder/C/$student_id"
        target_file="$target_dir/main.c"
        mkdir -p "$target_dir"
        cp "$source_file" "$target_file"
    elif [ -n "$cpp_file" ]; then
        language="C++"
        source_file="$cpp_file"
        target_dir="$target_folder/C++/$student_id"
        target_file="$target_dir/main.cpp"
        mkdir -p "$target_dir"
        cp "$source_file" "$target_file"
    elif [ -n "$py_file" ]; then
        language="Python"
        source_file="$py_file"
        target_dir="$target_folder/Python/$student_id"
        target_file="$target_dir/main.py"
        mkdir -p "$target_dir"
        cp "$source_file" "$target_file"
    elif [ -n "$java_file" ]; then
        language="Java"
        source_file="$java_file"
        target_dir="$target_folder/Java/$student_id"
        target_file="$target_dir/Main.java"
        mkdir -p "$target_dir"
        cp "$source_file" "$target_file"
    else
        [ "$verbose" = true ] && echo "No valid code file found for $student_id"
        continue
    fi
    
    # Initialize attributes to be entered into the csv results
    line_count=0
    comment_count=0
    function_count=0
    matched=0
    not_matched=0
    
    # calculate attributes (if needed)
    if [ "$calculate_lc" = true ]; then
        line_count=$(count_lines "$target_file")
    fi
    
    if [ "$calculate_cc" = true ]; then
        comment_count=$(count_comments "$target_file")
    fi
    
    if [ "$calculate_fc" = true ]; then
        function_count=$(count_functions "$target_file")
    fi
    
    # execute extracted code and match with expected output (if needed)
    if [ "$execute" = true ]; then
        # verbose output when executing files
        [ "$verbose" = true ] && echo "Executing files of $student_id"
        
        # compile
        if [ "$language" = "C" ]; then
            gcc "$target_file" -o "$target_dir/main.out" 2>/dev/null
            if [ $? -ne 0 ]; then
                continue
            fi
        elif [ "$language" = "C++" ]; then
            g++ "$target_file" -o "$target_dir/main.out" 2>/dev/null
            if [ $? -ne 0 ]; then
                continue
            fi
        elif [ "$language" = "Java" ]; then
            (cd "$target_dir" && javac Main.java 2>/dev/null)
            if [ $? -ne 0 ]; then
                continue
            fi
        fi
        
        # run the provided test cases
        for ((i=1; i<=test_count; i++)); do
            test_file="$test_folder/test$i.txt"
            ans_file="$answer_folder/ans$i.txt"
            out_file="$target_dir/out$i.txt"
            
            # Run program with input from test file
            if [ "$language" = "C" ] || [ "$language" = "C++" ]; then
                "$target_dir/main.out" < "$test_file" > "$out_file" 2>/dev/null
            elif [ "$language" = "Python" ]; then
                python3 "$target_file" < "$test_file" > "$out_file" 2>/dev/null
            elif [ "$language" = "Java" ]; then
                current_dir=$(pwd)
                cd "$target_dir" && java Main < "$current_dir/$test_file" > "out$i.txt" 2>/dev/null
                cd "$current_dir"
            fi
            
            # compare
            if diff "$out_file" "$ans_file" >/dev/null 2>&1; then
                matched=$((matched + 1))
            else
                not_matched=$((not_matched + 1))
            fi
        done
    fi
    
    csv_row="$student_id,\"$student_name\",$language"
    if [ "$execute" = true ]; then
        csv_row="$csv_row,$matched,$not_matched"
    fi
    if [ "$calculate_lc" = true ]; then
        csv_row="$csv_row,$line_count"
    fi
    if [ "$calculate_cc" = true ]; then
        csv_row="$csv_row,$comment_count"
    fi
    if [ "$calculate_fc" = true ]; then
        csv_row="$csv_row,$function_count"
    fi
    
    # append to csv results
    echo "$csv_row" >> "$target_folder/result.csv"
done

# clean up temp dir
rm -rf "$temp_dir"

# verbose output after finishing
[ "$verbose" = true ] && echo "All submissions processed successfully"