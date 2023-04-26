# Define a function to print the call stack
define print_call_stack
  # Print a header
  printf "\nCall stack:\n\n"
  # Use the "backtrace" command to print the call stack
  backtrace full
end

# Set a breakpoint at program exit
break exit

# Define a function to handle the breakpoint
define exit_handler
  # Print the call stack
  print_call_stack
  # Resume program execution
  continue
end

# Set the breakpoint handler
command
  # Call the exit handler when the breakpoint is hit
  silent
  break 1
  commands
    exit_handler
  end
end

# Start the program
run
