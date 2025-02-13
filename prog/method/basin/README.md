# **Basin Simulation**  

This program calculates the **first flux** of molecular diffusion into the nuclear pore complex (NPC). The simulation is built upon the core code located in `prog/sim`.  

## **Compilation**  
To compile the program, run:  
```bash
make
```

## **Running the Simulation**  
Execute the following command to run the simulation:  
```bash
./a.out
```


## **Input Files**  
The program requires the following input files:  
- **Cargo coordinates**: `input/cargo.vtk`  
- **FG-Nups coordinates**: `input/fgnup.vtk`  

## **Output Files**  
The simulation generates the following output files:  

- **Cargo at pore entrance**: `output/cg_***.vtk`  
- **FG-Nups at pore entrance**: `output/fg_***.vtk`  
- **Cargo trajectory throughout the simulation**: `traj/cg_***.vtk`  
- **FG-Nups trajectory throughout the simulation**: `traj/fg_***.vtk`  

The `***` in file names represents simulation-specific identifiers.  


