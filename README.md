# trace-children
The 2k18 way to trace a process children in linux

Tired of checking top every time to see the stats on that pesky crypto node?  
Do you like cool-looking, accurate and meaningful stats?
### Then trace-children is for you!

### How to install 🥇
```
git clone https://github.com/alexvelea/trace-children
cd trace-children
cmake CMakeLists.txt
sudo make install
```

### How to use 🏃
```
tracechildren 4607
```
Possible output 🌳
```
 ╔═ pid   4607
 ║  wall  0.000
 ║  cpu   0.000
 ║  sys   0.000
 ╚═══╦═ pid   4608
     ║  wall  0.000
     ║  cpu   0.000
     ║  sys   0.000
     ╠═══╦═ pid   4610
     ║   ║  wall  0.000
     ║   ║  cpu   0.000
     ║   ║  sys   0.000
     ║   ╠═══╦═ pid   4612
     ║   ║   ║  wall  0.000
     ║   ║   ║  cpu   0.000
     ║   ║   ║  sys   0.000
     ║   ║   ╚═════ pid   4614
     ║   ║          wall  0.000
     ║   ║          cpu   0.000
     ║   ║          sys   0.000
     ║   ╚═══╦═ pid   4613
     ║       ║  wall  0.000
     ║       ║  cpu   0.000
     ║       ║  sys   0.000
     ║       ╚═════ pid   4616
     ║              wall  0.000
     ║              cpu   0.000
     ║              sys   0.000
     ╚═════ pid   4611
            wall  0.000
            cpu   0.000
            sys   0.000
```

### Want to use as a library? 📖
```
#include <trace_children.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    struct ProcessStats* root = getProcessStats(1);
    sleep(1); // wait 1 sec for that juicy percentage information
    updateProcessStatsRecursive(root); 
    printProcessStats(root);
    dealloc(root); // we don't like leaks
    return 0;
}
```

### Do you like low compile times or you have a bigger project?
Use static library to solve all your problems! 🎉  
Just add the 2 magic flags: `-Dtracechildrenuselib -ltracechildrenlib`

### Want to test trace-children in your house with a nice example?
After installing, in the repository directory, run
```
 make; mkfifo a; ./build/bin/fork_test >a & ./build/bin/tracechildren <a && pkill fork_test; && rm a
```
