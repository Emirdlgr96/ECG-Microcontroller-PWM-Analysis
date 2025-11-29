# ECG-Microcontroller-PWM-Analysis
Real-time ECG signal processing simulation using C language, involving PWM control and GPIO logic handling under corrupt data conditions.
#  ECG & PWM Microcontroller Analysis (C-Language)

##  Project Overview
This project simulates a microcontroller-based system designed for basic vital sign monitoring. It reads simulated ECG data, determines heart rate, and controls an output peripheral (PWM) based on signal analysis. Implemented using **C Language** principles (GPIO and PWM logic).

##  Key Features
* **Data Parsing:** Reads and processes time-series data from a CSV file.
* **PWM Control Logic:** Calculates and adjusts Pulse Width Modulation values based on the simulated heart rate (BPM).
* **Output Logging:** Utilizes command-line redirection for verifiable data analysis.
* **Robustness Check:** Designed to handle initial data corruption issues.

---

##  Engineering Challenges & Solutions (Crisis Management)

This section documents the issues encountered and the engineering workarounds implemented—the most valuable part of this project.

### 1. Data Integrity & Parsing Error
* **The Problem:** The provided `patient_data.csv` contained critical formatting errors (e.g., date strings instead of numerical time values like "22.Eyl") and inconsistent line data, causing the parsing logic to fail or enter infinite loops.
* **The Solution:** Instead of relying on the flawed raw time column, a **`simulated_time`** variable was implemented in the C code. This structural change ensured continuous and accurate monitoring, bypassing the corrupt input stream.

### 2. Terminal Output Logging
* **The Problem:** The code ran too fast, making it impossible to analyze the millisecond-level data outputs (BPM, SpO2) directly in the console for verification.
* **The Solution:** Command-line redirection was utilized (`.\odev.exe > output.txt`) to capture the entire runtime log into the verifiable **`output.txt`** file. This allowed for comprehensive, post-mortem data analysis.

---

##  Proof & Documentation
| File | Explanation |
| :--- | :--- |
| `22017751_EmirDulger.c` | Main C source code. |
| `patient_data.csv` | Raw data file used (Including erroneous version). |
| `output.txt` | Terminal log output that proves the code is running successfully. |
| `README_FOR_TEACHER.png` | Summary created for identifying and solving problems (Engineering Story). |

##  How to Run
1.  Compile the C file using GCC.
2.  Execute with logging: `.\odev.exe > output.txt`
