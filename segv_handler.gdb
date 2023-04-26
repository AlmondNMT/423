set args test.py

define segv_handler
    break *$siginfo->si_addr
    continue
end

handle SIGSEGV stop print pass nostop noprint
handle SIGSEGV stop print pass nostop noprint segv_handler

run
