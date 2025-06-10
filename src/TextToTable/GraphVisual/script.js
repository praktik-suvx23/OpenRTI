fetch('../TablesJSON/averages.json')
  .then(response => response.json())
  .then(json => {
    // Sort averagesArray by numberOfShips
    const averagesArray = json.averages.slice().sort((a, b) => a.numberOfShips - b.numberOfShips);

    // All average types
    const keys = [
      "ActualTimeScale",
      "timeFlyingSimulationTime",
      "timeFlyingRealTime"
    ];

    // Prepare one dataset per average type (line)
    const datasets = keys.map((key, idx) => ({
      label: key,
      data: averagesArray.map(avgSet => avgSet[key]),
      borderColor: `hsl(${idx * 60}, 70%, 50%)`,
      backgroundColor: `hsl(${idx * 60}, 70%, 80%)`,
      fill: false,
      tension: 0.2,
      pointRadius: 4
    }));

    const data = {
      labels: averagesArray.map(avgSet => avgSet.numberOfShips !== undefined ? `Ships ${avgSet.numberOfShips}` : ""),
      datasets: datasets
    };

    const config = {
      type: 'line',
      data: data,
      options: {
        responsive: true,
        plugins: {
          legend: { display: true }
        },
        scales: {
          x: {
            title: { display: true, text: 'Number of Ships' }
          },
          y: {
            beginAtZero: true,
            title: { display: true, text: 'Timescale' }
          }
        }
      }
    };

    const ctx = document.getElementById('myChart').getContext('2d');
    new Chart(ctx, config);
  });