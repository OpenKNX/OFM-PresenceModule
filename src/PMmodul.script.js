
function PM_distanceToMeter(input, output, context) {
    output.Meter = '* 0.7m = ' + ('   ' + (input.Range * 0.7).toFixed(1)).slice(-4) + 'm';
}

function PM_meterToDistance(input, output, context) { }

function PM_hlkDeltaAbsToRel(input, output, context) {
    output.Rel = input.Abs - input.Ref;
}

function PM_hlkDeltaRelToAbs(input, output, context) {
    output.Abs = input.Rel + output.Ref;
}

var PM_dataKindText = ["leer", "Basiswerte der Kalibrierung", "Standardabweichung der Kalibrierung", "Maximalwerte der Kalibrierung", "aktuelle Basiswerte", "aktuelle Standardabweichung", "aktuelle Maximalwerte", "Haltewerte", "Triggerwerte"];

function PM_calcParamName(parName, number) {
    var paramName = "PM_" + parName;
    var paramNumber = String(number);
    if (number < 10) paramNumber = "0" + paramNumber;
    return paramName + paramNumber;
}

function PM_getCalibrationData(device, online, progress, context) {
    // context.dataKind - type of data to fetch
    // context.parName - Name of Parameter to fetch in ETS

    if (context.dataKind < 1 && context.dataKind > 8) return;

    progress.setText("PM: Lese Daten zu '" + PM_dataKindText[context.dataKind] + "' ...");
    online.connect();
    PM_processCalibrationData(device, online, progress, context.dataKind, context.parName);
    online.disconnect();
}

function PM_getCalibrationDataSet(device, online, progress, context) {
    online.connect();
    PM_processCalibrationDataSet(device, online, progress, context.dataKind, context.parName, 1);
    online.disconnect();
}

function PM_processCalibrationDataSet(device, online, progress, dataKind, parName, percent) {
    // dataKind - type of data to fetch
    // parName - Name of Parameter to fetch in ETS


    // var dataKind = Number(context.dataKind);
    info("dataKind: " + dataKind);
    info("parName: " + parName);
    if (dataKind != 1 && dataKind != 4) return;

    var lProgress = (100 - percent) / 3;
    progress.setProgress(percent);
    progress.setText("PM: Lese Range-Werte ...");
    PM_processCalibrationData(device, online, progress, dataKind, parName + "Rng");
    percent += lProgress;
    progress.setProgress(percent);
    progress.setText("PM: Lese Standardabweichung ...");
    PM_processCalibrationData(device, online, progress, dataKind + 1, parName + "Std");
    percent += lProgress;
    progress.setProgress(percent);
    progress.setText("PM: Lese Maximalwerte ...");
    PM_processCalibrationData(device, online, progress, dataKind + 2, parName + "Max");
    percent += lProgress;
    progress.setProgress(percent);
    progress.setText("PM: Werte erfolgreich gelesen");
}

function PM_processCalibrationData(device, online, progress, dataKind, parName) {
    var data = [1]; // command getCalibrationData
    // kind of data requested (1=cal raw, 2=cal std, 3=cal max, 4=curr raw, 5=curr std, 6=curr max, 7=hold, 8=trigger)
    data = data.concat(dataKind, 0); // ensure zero-terminated string

    var resp = online.invokeFunctionProperty(160, 6, data);
    // error handling
    if (resp[0] == 0) {
        progress.setText("PM: Daten zu '" + PM_dataKindText[dataKind] + "' gefunden");

        // we expect 16 x 2 Bytes
        for (var i = 0; i < 16; i++) {
            var paramName = PM_calcParamName(parName, i);
            var arrayIndex = i * 2 + 1;
            var paramValue = resp[arrayIndex] << 8 | resp[arrayIndex + 1];
            info("read paramName: " + paramName + ",  paramValue: " + paramValue);
            parGridCell = device.getParameterByName(paramName);
            parGridCell.value = paramValue;
        }
    }
    else {
        if (resp[0] == 1 || resp.length != 33) {
            progress.setText("PM: Keine Daten zu '" + PM_dataKindText[dataKind] + "' gefunden");
        } else {
            throw new Error("PM: Es ist ein unerwarteter Fehler aufgetreten!");
        }
    }
    return (resp[0] == 0);
}


function PM_setCalibrationData(device, online, progress, context) {

    progress.setProgress(10);
    progress.setText("PM: Schreibe " + PM_dataKindText[context.dataKind] + " ...");
    online.connect();
    var data = [3]; // command setCalibrationData
    data = data.concat(context.dataKind); // subcommand 7=hold, 8=trigger; zero-terminated

    // we write 16 x 2 Bytes
    for (var i = 0; i < 16; i++) {
        var paramName = PM_calcParamName(context.parName, i);
        parGridCell = device.getParameterByName(paramName);
        var paramValue = parGridCell.value;
        info("write: paramName: " + paramName + ",  paramValue: " + paramValue);
        data = data.concat(paramValue >> 8, paramValue & 0xFF);
    }

    progress.setProgress(30);
    var resp = online.invokeFunctionProperty(160, 6, data);
    progress.setProgress(100);
    online.disconnect();
    if (resp[0] == 0) {
        progress.setText("PM: " + PM_dataKindText[context.dataKind] + " geschrieben");
    }
    else {
        throw new Error("PM: " + PM_dataKindText[context.dataKind] + " konnte nicht geschrieben werden");
    }
}

function PM_copyParameterValues(device, dataSource, dataTarget) {
    info("dataSource: " + dataSource);
    info("dataTarget: " + dataTarget);
    for (var i = 0; i < 16; i++) {
        var paramNameSource = PM_calcParamName(dataSource, i);
        var paramNameTarget = PM_calcParamName(dataTarget, i);
        parSource = device.getParameterByName(paramNameSource);
        parTarget = device.getParameterByName(paramNameTarget);
        var paramValue = parSource.value;
        info("Copy: " + paramNameTarget + " = " + paramNameSource + " = " + paramValue);
        parTarget.value = paramValue;
    }
}


function PM_clearMemory(device, online, progress, context) {
    device.getParameterByName("PM_" + context.dataSource + "Trigger").value = 0;
    device.getParameterByName("PM_" + context.dataSource + "Hold").value = 0;
}

function PM_getMemoryData(device, online, progress, context) {
    PM_copyParameterValues(device, context.dataSource, context.dataTarget);
}

function PM_setMemoryData(device, online, progress, context) {
    info("dataSource: " + context.dataSource);
    info("dataTarget: " + context.dataTarget);
    PM_copyParameterValues(device, context.dataSource + "Abs", context.dataTarget + "Abs");
    PM_copyParameterValues(device, context.dataSource + "Delta", context.dataTarget + "Delta");
    parCheckbox = device.getParameterByName("PM_" + context.dataTarget);
    parCheckbox.value = 1;
}

function PM_sleep(milliseconds) {
    var currentTime = new Date().getTime();
    while (currentTime + milliseconds >= new Date().getTime()) {
    }
}

function PM_startCalibration(device, online, progress, context) {
    // Start calibration, wait until finished and get data afterwards
    // context.dataKind = 1: Calibration
    // context.dataKind = 4: Test Calibration
    var lText = "Sensorkalibrierung";
    if (context.dataKind == 4) lText = "Lesen der aktuellen Werte";
    progress.setText("PM: " + lText + " ...");

    online.connect();
    var data = [2]; // command startCalibration
    data = data.concat(context.dataKind, 0); // subcommand 1=cal, 4=calt; zero-terminated

    var resp = online.invokeFunctionProperty(160, 6, data);

    var lPercent = 0;
    if (resp[0] == 0) {
        // poll for calibration finished
        while (resp[0] == 0 && resp[1] == 0) {
            data = [2];
            data = data.concat(2, 0); // subcommand 2=wait end; zero-terminated string
            lPercent += 3;
            if (lPercent <= 100) progress.setProgress(lPercent);
            PM_sleep(1000);
            resp = online.invokeFunctionProperty(160, 6, data);
        }
        if (resp[0] == 0 && resp[1] == 1) {
            // calibration finished, everything is ok, we can read calibration data now
            PM_processCalibrationDataSet(device, online, progress, context.dataKind, context.parName, lPercent);
        }
    }
    progress.setText("PM: " + lText + " abgeschlossen");

    online.disconnect();

}
