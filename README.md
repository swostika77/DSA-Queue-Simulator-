# 🚦 Queue Based Traffic Light Simulator
This project is a 2D traffic intersection simulator implemented in C using SDL2, developed as part of a programming assignment for COMP 202 course.

---
## 🎬 Simulation Output
![Queue Traffic Simulator gif](https://github.com/user-attachments/assets/017a8e32-fcd9-4087-ac94-9cadd8b17ace)

---

## ✨ Achievements
**1.Four-Way Traffic Intersection Simulation:**  
Implemented a fully functional four-way intersection with roads labeled A, B, C, and D.

**2.Lane-Specific Vehicle Movement:**  
Lane 0: Vehicles make left turns.  
Lane 1: Vehicles move straight through the intersection.

**3.Priority-Based Traffic Light System:**  
Green light duration adapts dynamically based on the number of waiting vehicles.
Lane 1 on Road A is treated as a priority lane, giving preference to higher vehicle counts.
Automatic cycling of traffic lights between roads (A → B → C → D → A).

**4.Vehicle Queue Implementation:**  
Vehicles are stored in a fixed-size queue (up to 500 vehicles).
Vehicles are added dynamically and removed once they exit the intersection.

**5.Vehicle Movement & Collision Logic:**  
Vehicles stop at red lights.
Vehicles follow their lane-specific turning rules.
Vehicles snap to lanes when turning to ensure proper alignment.

**6.Visual Representation:**  
Roads and lanes are graphically rendered using SDL2.
Lane-specific vehicles are shown in distinct colors.
Dashed lane markings clearly indicate lane boundaries.

**7.Dynamic Vehicle Generation:**  
Vehicles can be spawned from a file (vehicles.data).
Vehicles are generated randomly into allowed lanes.

---
## 🚦 Traffic Processing Algorithm
1. Initialize SDL window and vehicle queue.
2. Read vehicle data from an external file at fixed intervals.
3. Insert vehicles into a queue based on road and lane.
4. Count waiting vehicles for each road.
5. Assign green light dynamically based on queue size.
6. Stop vehicles at red lights.
7. Allow movement when green light is active.
8. Apply lane-specific rules (left turn / straight).
9. Remove vehicles once they exit the simulation area.
10. Repeat this process continuously for each frame.

---
## 🔄 Mode of Communication

### 📁 Sharing of Files

This project implements **inter-process communication** through **file sharing**.  
Two independent programs—**Traffic Generator** and **Traffic Simulator**—communicate using a common file named:vehicles.data

---

## 🧱 Data Structures

| 🗂️ Structure      | 🔧 Implementation | 🎯 Purpose                        |
|------------------|-----------------|---------------------------------|
| Queue            | Array-based      | Stores vehicles in each lane     |
| Vehicle Struct   | Struct           | Holds position, direction, lane info |
| Light Enum       | Enum             | Represents RED/GREEN signals    |

---

##  🕒 Time Complexity per Frame

| **Operation** | **Time Complexity** | **Explanation** |
|---------------|-------------------|----------------|
| Move Vehicles (`moveVehicles()`) | O(n) | Loops over all active vehicles to update position, turning, and lane alignment. |
| Update Traffic Lights (`updateLights()`) | O(n) | Counts vehicles in priority lanes and updates the green/red light states dynamically. |
| Draw Vehicles (`drawVehicles()`) | O(n) | Loops over all active vehicles for rendering. |
| Read Vehicles from File (`spawnVehiclesFromFile()`) | O(n) | Reads new vehicles from the file and enqueues them into the queue. |

**Total Time Complexity per Frame:** **O(n)**  where **n = number of active vehicles** in the simulation.

---

## 🚀 Installation & Setup

### **Prerequisites**
- C/C++ Compiler (GCC, MinGW, or similar)  
- SDL2 library  
- Git (optional, for cloning the repository)  

### **Setup Steps**

**1️⃣ Install SDL2:**  
- Download the SDL2 development library from [https://www.libsdl.org/](https://www.libsdl.org/).  
- Extract it to a convenient folder.  
- Configure your compiler/IDE to include SDL2 **header files** and **library paths**.  

**2️⃣ Prepare the Project:**  
- Optionally, clone the repository using Git:  
git clone https://github.com/YOUR_USERNAME/DSA-Queue-Simulator.git
cd DSA-Queue-Simulator
- Or simply download the project files into a working directory.  

**3️⃣ Build the Project:**  
- Compile the **traffic generator** and **simulator** source files using your compiler.  
- Ensure SDL2 runtime DLLs are accessible in the executable directory or system PATH.  

### **Running the Programs**

1. Start the **traffic generator**, which continuously writes vehicle data.  
2. Run the **simulator**, which reads the data and simulates the traffic intersection.  
3. Close the simulator window to safely stop the program.
   
