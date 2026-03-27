
function PM_distanceCheck(input, changed, prevValue, context) {

    if (input.RangeMin >= input.RangeMax)
        return "Zwischen minimaler und maximaler Entfernung muss immer mindestens 1 Meter liegen!";
    else
        return true;
}

function PM_hideWarning(device, online, progress, context) {
    var parHide = device.getParameterByName("PM_HlkSuppressWarning");
    parHide.value = 1;
}

var PM_diffArguments = ["Trigger", "Hold", "Cur1Max", "Cur1Std", "Cur1Avg", "Cur2Max", "Cur2Std", "Cur2Avg"]

function PM_calcParamName(parName, number) {
    var paramName = "PM_" + parName;
    var paramNumber = String(number);
    if (number < 10) paramNumber = "0" + paramNumber;
    return paramName + paramNumber;
}

function PM_diffValues(device, online, progress, context) {
    var parMeasure1 = device.getParameterByName("PM_HlkMeasure1");
    var parMeasure2 = device.getParameterByName("PM_HlkMeasure2");
    var diffName1 = PM_diffArguments[parMeasure1.value];
    var diffName2 = PM_diffArguments[parMeasure2.value];

    for (var i = 0; i < 16; i++) {
        var parDelta = device.getParameterByName(PM_calcParamName("Delta", i));
        var parArg1 = device.getParameterByName(PM_calcParamName(diffName1, i));
        var parArg2 = device.getParameterByName(PM_calcParamName(diffName2, i));
        parDelta.value = parArg1.value - parArg2.value;
    }
}

var PM_dataKindText = ["leer", "Stichprobe 1: Durchschnitt", "Stichprobe 1: Standardabweichung", "Stichprobe 1: Maximum", "Stichprobe 2: Durchschnitt", "Stichprobe 2: Standardabweichung", "Stichprobe 2: Maximum", "Halten", "Trigger"];

function PM_getCalibrationData(device, online, progress, context) {
    // context.dataKind - type of data to fetch
    // context.parName - Name of Parameter to fetch in ETS

    if (context.dataKind < 1 && context.dataKind > 6) return;

    progress.setText("PM: Lese '" + PM_dataKindText[context.dataKind] + "' ...");
    progress.setProgress(20);
    online.connect();
    progress.setProgress(50);
    PM_processCalibrationData(device, online, progress, context.dataKind, context.parName);
    progress.setProgress(80);
    online.disconnect();
    progress.setProgress(100);
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
    progress.setText("PM: Lese " + PM_dataKindText[dataKind] + " ...");
    PM_processCalibrationData(device, online, progress, 4, parName + "Avg");
    percent += lProgress;
    progress.setProgress(percent);
    progress.setText("PM: Lese " + PM_dataKindText[dataKind + 1] + " ...");
    PM_processCalibrationData(device, online, progress, 5, parName + "Std");
    percent += lProgress;
    progress.setProgress(percent);
    progress.setText("PM: Lese " + PM_dataKindText[dataKind + 2] + " ...");
    PM_processCalibrationData(device, online, progress, 6, parName + "Max");
    percent += lProgress;
    progress.setProgress(percent);
    progress.setText("PM: Werte erfolgreich gelesen");
}

function PM_processCalibrationData(device, online, progress, dataKind, parName) {
    var data = [1]; // command getCalibrationData
    // kind of data requested (1=cal raw, 2=cal std, 3=cal max, 4=curr raw, 5=curr std, 6=curr max, 7=hold, 8=trigger)
    data = data.concat(dataKind, 0); // ensure zero-terminated string

    var resp = BASE_invokeFunctionPropertyWrapper(160, 6, data, device, online, progress);
    // error handling
    if (resp[0] == 0) {
        // progress.setText("PM: " + PM_dataKindText[dataKind] + " gefunden");

        // we expect 16 x 2 Bytes
        for (var i = 0; i < 16; i++) {
            var paramName = PM_calcParamName(parName, i);
            var arrayIndex = i * 2 + 1;
            var paramValue = resp[arrayIndex] << 8 | resp[arrayIndex + 1];
            info("read paramName: " + paramName + ",  paramValue: " + paramValue);
            parGridCell = device.getParameterByName(paramName);
            parGridCell.value = paramValue > 100 ? paramValue : 100;
        }
    }
    else {
        if (resp[0] == 1 || resp.length != 33) {
            progress.setText("PM: " + PM_dataKindText[dataKind] + " nicht gefunden");
        } else {
            throw new Error("PM: Es ist ein Fehler aufgetreten!");
        }
    }
    return (resp[0] == 0);
}


function PM_setCalibrationData(device, online, progress, dataKind, parName, percent) {
    parHfDelayTime = device.getParameterByName("PM_HfDelayTime");
    parHfRangeGateMin = device.getParameterByName("PM_HfRangeGateMin");
    parHfRangeGateMax = device.getParameterByName("PM_HfRangeGateMax");
    progress.setProgress(percent + 10);
    var data = [3]; // command setCalibrationData
    data = data.concat(dataKind, parHfRangeGateMin.value, parHfRangeGateMax.value, parHfDelayTime.value >> 8, parHfDelayTime.value & 0xFF); // subcommand 7=hold, 8=trigger; zero-terminated

    // we write 16 x 2 Bytes
    for (var i = 0; i < 16; i++) {
        var paramValue;
        var paramCheckBoxName = PM_calcParamName("HlkActiveCol",i);
        var parCheckBox = device.getParameterByName(paramCheckBoxName);
        if (parCheckBox.value && i >= parHfRangeGateMin.value && i <= parHfRangeGateMax.value ) {
            var paramName = PM_calcParamName(parName, i);
            parGridCell = device.getParameterByName(paramName);
            paramValue = parGridCell.value;
        } else {
            paramValue = 9332;
        }
        info("write: paramName: " + paramName + ",  paramValue: " + paramValue);
        data = data.concat(paramValue >> 8, paramValue & 0xFF);
    }

    progress.setProgress(percent + 20);
    var resp = BASE_invokeFunctionPropertyWrapper(160, 6, data, device, online, progress);
    progress.setProgress(percent + 30);
    if (resp[0] == 0) {
        progress.setText("PM: " + PM_dataKindText[dataKind] + " geschrieben");
    }
    else {
        throw new Error("PM: " + PM_dataKindText[dataKind] + " konnte nicht geschrieben werden");
    }
}

function PM_setCalibrationDataSet(device, online, progress, context) {
    progress.setProgress(10);
    progress.setText("PM: Schreibe Halten und Trigger ...");
    online.connect();
    PM_setCalibrationData(device, online, progress, 7, "Hold", 10);
    PM_setCalibrationData(device, online, progress, 8, "Trigger", 50);
    // wait for Sensor restart
    var lPercent = 80;
    // poll for sensor restart finished
    var data = [0];
    var resp = [0];
    resp = resp.concat(0);
    var lCancelled = false;
    progress.setText("PM: HF-Sensor wird neu gestartet ...");
    while (resp[0] == 0 && resp[1] == 0 && !lCancelled) {
        data = [2];
        data = data.concat(3, 0); // subcommand 2=wait end; zero-terminated string
        lPercent += (100.0-lPercent)/5.0;
        if (lPercent <= 100) progress.setProgress(lPercent);
        PM_sleep(1000);
        resp = online.invokeFunctionProperty(160, 6, data); // short message (wait polling)
        lCancelled = progress.isCanceled();
    }
    online.disconnect();
    progress.setProgress(100);
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
    PM_copyParameterValues(device, context.dataSource, context.dataTarget);
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
    // context.dataKind = 1: Stichprobe 1
    // context.dataKind = 4: Stichprobe 2
    // context.delay: Verzögerung vor dem Start
    
    var lCancelled = false;
    if (context.delay) {
        for (var lDelay = context.delay; lDelay >= 0 && !lCancelled; lDelay--) {
            progress.setText("PM: Countdown " + lDelay + " Sekunden");
            progress.setProgress((100 / context.delay) * lDelay);
            PM_sleep(1000);
            lCancelled = progress.isCanceled();
        }
    }

    if (!lCancelled) {
        var lText = "Lese Stichprobe ";
        lText += (context.dataKind == 1) ? "1" : "2";
        progress.setText("PM: " + lText + " ...");
        
        online.connect();
        var data = [2]; // command startCalibration
        data = data.concat(4, 0); // subcommand 1=cal, 4=calt; zero-terminated
        
        var resp = online.invokeFunctionProperty(160, 6, data); // short messsage (start calibration)
        
        var lPercent = 0;
        if (resp[0] == 0) {
            // poll for calibration finished
            while (resp[0] == 0 && resp[1] == 0 && !lCancelled) {
                data = [2];
                data = data.concat(2, 0); // subcommand 2=wait end; zero-terminated string
                lPercent += (70.0-lPercent)/20.0;
                if (lPercent <= 100) progress.setProgress(lPercent);
                PM_sleep(1000);
                resp = online.invokeFunctionProperty(160, 6, data); // short message (wait polling)
                lCancelled = progress.isCanceled();
            }
            if (resp[0] == 0 && resp[1] == 1 && !lCancelled) {
                // calibration finished, everything is ok, we can read calibration data now
                PM_processCalibrationDataSet(device, online, progress, context.dataKind, context.parName, lPercent);
            }
        }
    }
    if (lCancelled) {
        progress.setText("PM: " + lText + " vom Benutzer abgebrochen");
    } else { 
        progress.setText("PM: " + lText + " abgeschlossen");
    }
    online.disconnect();

}

var PM_log10Values = [0.0, 0.0, 0.30102999566398119521373889472449, 0.47712125471966243729502790325512, 0.60205999132796239042747778944899, 0.69897000433601880478626110527551, 0.77815125038364363250876679797961, 0.84509804001425683071221625859264, 0.90308998699194358564121668417348, 0.95424250943932487459005580651023, 1.0, 1.041392685158225040750199971243];
var PM_hfTriggerValues = [4778,4477,3477,3301,2699,2602,2602,2477,2477,2477,2477,2398,2398,2301,2301,2301];
var PM_hfHoldValues =    [4602,4301,2602,2477,2477,2301,2301,2176,2176,2000,2000,2000,2000,2000,2000,2000];
var PM_hfFields = ["Cur1Max", "Cur1Std", "Cur1Avg", "Cur2Max", "Cur2Std", "Cur2Avg", "Delta"];

function PM_processFunction(device, online, progress, context, target) {
    // we derive the correct function from according parameters
    var parFunction = device.getParameterByName("PM_HlkFormula");
    var parSensitivity = device.getParameterByName("PM_HlkFormulaSensitivity");
    var parOffset = device.getParameterByName("PM_HlkFormulaOffset");
    var result;
    var paramSourceName;
    var parGridCell;
    var paramTargetName;
    var checked = false;

    info("parFunction: " + parFunction.value + ", target: " + target + ", parOffset: " + parOffset.value);
    if (parFunction.value == 3) {
        // check if "stichprobe 1" existiert
        for (var checkLoop = 0; checkLoop < 16; checkLoop++) {
            paramSourceName = PM_calcParamName("Cur1Avg", checkLoop);
            parGridCell = device.getParameterByName(paramSourceName);
            // find at least 1 fetched value in stichprobe 1
            if (parGridCell.value > 100) {
                checked = true;
                checkLoop = 16;
            }
        }
        if (!checked)
            throw new Error("Erst Stichprobe 1 starten");
    }
    for (var i = 0; i < 16; i++) {
        switch (parFunction.value) {
            case 1:
                // Standwerte
                if (target == 0) {
                    // factory values for trigger
                    result = PM_hfTriggerValues[i];
                } else {
                    // factory values for hold
                    result = PM_hfHoldValues[i];
                }
                paramSourceName = "defaults";
                break;
            case 2:
                // konstanter Offset
                result = parOffset.value;
                paramSourceName = "constants";
                break;
            case 3:
                // Standardformel
                if (target == 0) {
                    // standard formula for trigger
                    result = 6 / PM_log10Values[parSensitivity.value / 10 + 1] * 100;
                } else {
                    // standard formula for hold
                    result = (3 * (1 / PM_log10Values[parSensitivity.value / 10 + 1]) - 1.5) * 100;
                }
                paramSourceName = PM_calcParamName("Cur1Avg", i);
                parGridCell = device.getParameterByName(paramSourceName);
                result = parGridCell.value + result;
                paramSourceName = "function";
                break;
            default:
                paramSourceName = PM_calcParamName(PM_hfFields[parFunction.value-8], i);
                parGridCell = device.getParameterByName(paramSourceName);
                result = parGridCell.value + parOffset.value;
                break;
        }
        paramTargetName = target ? "Hold" : "Trigger";
        paramTargetName = PM_calcParamName(paramTargetName, i);
        info("Using " + paramSourceName + " to calculate " + paramTargetName + " with value " + result);
        device.getParameterByName(paramTargetName).value = result;
    }
}

function PM_executeFunction(device, online, progress, context) {
    var parTarget = device.getParameterByName("PM_HlkFormulaTarget");
    if (parTarget.value == 0 || parTarget.value == 2)
        PM_processFunction(device, online, progress, context, 0);
    if (parTarget.value == 1 || parTarget.value == 2)
        PM_processFunction(device, online, progress, context, 1);
}


