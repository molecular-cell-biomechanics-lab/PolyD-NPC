# **Forward Flux Sampling Simulation**  

This program performs **forward flux sampling** of molecular diffusion through the nuclear pore complex (NPC). The simulation is built upon the core code located in `prog/sim`.  

## **Compilation**  
To compile the program, run:  
```bash
make
```

## **Running the Simulation**  
Execute the following command to run the simulation:  
```bash
./a.out {seed-number}
```

## **Input Files**  
The program requires the following input files:  
- **Cargo coordinates**: `input/s{seed}_cargo.vtk`  
- **FG-Nups coordinates**: `input/s{seed}_fgnup.vtk`  

## **Output Files**  
The simulation generates the following output files:  
- `rate_s{seed}.txt` – Calculated probability of passing through each interface  
- `output/tmp_cg_***.vtk` – Trajectory of cargo  
- `output/tmp_fg_***.vtk` – Trajectory of FG-Nups  

## **Simulation Parameters**  
Simulation parameters are specified in `src/input`:  
- `divs.txt` – Defines the z-positions that determine the edges of interfaces  
- `kbra.txt` – Defines the number of branches to launch at each interface  
- `pend.txt` – Defines the pruning probability at each interface  
