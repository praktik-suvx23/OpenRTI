# TextToTable

This program parses simulation log files and generates summary tables and JSON files for ship and missile data.

## Features

- Reads simulation logs to extract statistics for ships and missiles (flight times, distances, time scales, etc.).
- Calculates averages and summary statistics across all entries.
- Outputs results as:
  - A CSV table with detailed data for each ship/missile.
  - A JSON file with all entries.
  - An `averages.json` file that accumulates average statistics over multiple runs.

## Output

- **CSV Table:** Contains per-ship/missile statistics.  
  *Saved in*: `src/TextToTable/Tables/`
- **JSON File:** Contains all parsed data in structured format.  
  *Saved in*: `src/TextToTable/TablesJSON/`
- **Averages JSON:** Maintains a running log of average statistics for all processed runs.  
  *Saved in*: `src/TextToTable/TablesJSON/averages.json`

---

This tool helps you analyze and visualize simulation results for ships and missiles in your federation project.