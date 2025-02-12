# Brownian Dynamics Simulation Core Code

This repository contains the core code for our Brownian dynamics simulation.

## **Compilation**
The compilation process is managed through the `Makefile`.

- **Compile the program**:  
  ```sh
  make
  ```
- **Clean compiled files**:  
  ```sh
  make clean
  ```

## **Code Structure**
The key classes of the simulation are defined in the `src` directory:
- `_bead`  : general bead object
- `_fgnup` : FG-Nups (bead-springs)
- `_cargo` : cargo (rigid body composed of beads)
- `_grid`  : simulation grid (optional)
- `_bcs`   : boundary condition


## **Simulation Parameters**  
Simulation parameters are specified in `src/input`:

| **Variable** | **Description** | **Unit** |
|-------------|----------------|----------|
| `l_segm` | Segment length of FG-Nups | nm |
| `t_delt` | Simulation time step | ns |
| `T_body` | Temperature | K |
| `h_visc` | Viscosity of water at room temperature | cP |
| `r_stks` | Stokes radius of each bead | segm |
| `D_diff` | Diffusion coefficient | nm²/ns |
| `eps_rp` | Height of repulsion potential | kBT₀ |
| `sgm_rp` | Characteristic length of repulsion potential | segm |
| `cut_rp` | Cutoff distance of repulsion potential | segm |
| `eps_fg` | Interfacial energy of hydrophobic attraction (not used) | kBT₀ |
| `sgm_fg` | Characteristic length of hydrophobic attraction (not used) | segm |
| `cut_fg` | Cutoff distance of hydrophobic attraction | segm |
| `kon_fg` | On rate of hydrophobic attraction | /ns |
| `kof_fg` | Off rate of hydrophobic attraction | /ns |
| `k_tens` | Spring constant for tensile force | kBT₀/segm² |
| `l_kuhn` | Kuhn length of FG-Nups | segm |
| `R_cargo` | Radius of the cargo | nm |
| `R_pore` | Pore radius | nm |
| `cy_top` | Top coordinate of the pore | nm |
| `cy_btm` | Bottom coordinate of the pore | nm |
| `L_x` | Simulation box length in x direction | nm |
| `L_y` | Simulation box length in y direction | nm |
| `L_z` | Simulation box length in z direction | nm |



## **Running a Simulation**
An example simulation run is provided in `main.cpp`.

- **Run the simulation**:  
  ```sh
  ./a.out
  ```

### **Input Files**  
This program requires the following input files:  
- **Cargo coordinates**: `input/cargo.vtk`  
- **FG-Nups coordinates**: `input/fgnup.vtk`  

To customize your simulation, replace these files with the appropriate objects. Various cargo and FG-Nups configurations, each with different parameter values, can be found in:  
- **Cargo files**: `data/cargo/`  
- **FG-Nups files**: `data/fgnup/`  

The parameter values are indicated in the file names.  

#### **Examples:**
- `cargo_r00_5p00.vtk`: Cargo with **radius (d/2) = 0.5 nm** and **attraction area (S) = 0**.  
- `fgnup_equiv_p36_l150.vtk`: FG-Nup configuration with **pore diameter (D) = 36 nm** and **length (N) = 150**.  


### **Output**
The simulation outputs the trajectory of cargo and FG-Nups in the `test` directory.
