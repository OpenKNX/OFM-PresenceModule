
function PM_distanceToMeter(input, output, context) {
    output.Meter = '* 0.7m = ' + ('   ' + (input.Range * 0.7).toFixed(1)).slice(-4) + 'm';
}

function PM_meterToDistance(input, output, context) { } 
