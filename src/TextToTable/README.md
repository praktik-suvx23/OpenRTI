# TextToTable

This program parses simulation log files and generates summary tables and JSON files for ship and missile data.

## Features

- Reads simulation logs to extract statistics for ships and missiles (flight times, distances, time scales, etc.).
- Calculates averages and summary statistics across all entries.
- Outputs results as:
  - A CSV table with detailed data for each ship/missile.
  - A JSON file with all entries.
  - An `averages.json` file that accumulates average statistics over multiple runs.

## Usage

1. Place your simulation log file at the expected path (`DATA_LOG_PATH`).
2. Run the program:
    ```bash
    ./TextToTable
    ```
3. Output files will be created in:
    - `src/TextToTable/Tables/` (CSV)
    - `src/TextToTable/TablesJSON/` (JSON and averages)

## Output

- **CSV Table:** Contains per-ship/missile statistics.
- **JSON File:** Contains all parsed data in structured format.
- **Averages JSON:** Maintains a running log of average statistics for all processed runs.

---

This tool helps you analyze and visualize simulation results for ships and missiles in your federation project.