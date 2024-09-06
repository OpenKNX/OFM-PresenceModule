#include "Presence.h"
#include "KnxHelper.h"
#include "OpenKNX.h"
#include "PresenceChannel.h"
#include "Sensor.h"
#include "SensorDevices.h"
#include "SensorHLKLD2420.h"
#include "SensorXenD107H.h"
#include "SensorMR24xxB1.h"
#include "SensorOPT300x.h"
#include "SensorVEML7700.h"
#include "SmartMF.h"

#include "ModuleVersionCheck.h"

Presence openknxPresenceModule;

Presence::Presence()
{
    // init KoMap (robustness)
    addKoMap(0, 0, 0);
    mNumKoMap = 0;
}

Presence::~Presence()
{
}

const std::string Presence::name()
{
    return "Presence";
}

const std::string Presence::version()
{
    return MODULE_PresenceModule_Version;
}

void Presence::savePower()
{
#ifdef HF_POWER_PIN
    if (ParamPM_HfPresence == VAL_PM_PS_Hf_HLKLD2420)
        static_cast<SensorHLKLD2420 *>(mPresenceSensor)->switchPower(false);
    else if (ParamPM_HfPresence == VAL_PM_PS_Hf_XenD107H)
        static_cast<SensorXenD107H *>(mPresenceSensor)->switchPower(false);
#endif
}

bool Presence::restorePower()
{
#ifdef HF_POWER_PIN
    if (ParamPM_HfPresence == VAL_PM_PS_Hf_HLKLD2420)
        static_cast<SensorHLKLD2420 *>(mPresenceSensor)->switchPower(true);
    else if (ParamPM_HfPresence == VAL_PM_PS_Hf_XenD107H)
        static_cast<SensorXenD107H *>(mPresenceSensor)->switchPower(true);
#endif

    return true;
}

void Presence::addKoMap(uint16_t iKoNumber, uint8_t iChannelId, uint8_t iKoIndex)
{
    // first implementation, in future we use sorted insert
    mKoMap[mNumKoMap].koNumber = iKoNumber;
    mKoMap[mNumKoMap].channelIndex = iChannelId;
    mKoMap[mNumKoMap].koIndex = iKoIndex;
    if (mNumKoMap < cCountKoMap)
        mNumKoMap++;
}

// search for a given KO index for an other
// KO, which uses also this KO as an internal input
bool Presence::mapKO(uint16_t iKoNumber, sKoMap **iKoMap)
{
    sKoMap *lIterator = *iKoMap;
    uint16_t lIterationCount = 0;
    if (*iKoMap == 0)
        lIterator = &mKoMap[0];
    else
        lIterator++;
    while (lIterator->koNumber > 0 && lIterationCount < mNumKoMap)
    {
        if (lIterator->koNumber == iKoNumber)
        {
            *iKoMap = lIterator;
            return true;
        }
        else
            lIterator++;
    }
    return false;
}

void Presence::processAfterStartupDelay()
{
    logInfoP("afterStartupDelay");

    if (ParamPM_ReadLed)
    {
        if (ParamPM_LEDPresence == VAL_PM_LedKnx)
            KoPM_LEDPresence.requestObjectRead();
        if (ParamPM_LEDMove == VAL_PM_LedKnx)
            KoPM_LEDMove.requestObjectRead();
    }
}

void Presence::showHelp()
{
    if (!knx.configured())
        return;

    openknx.console.printHelpLine("vpm hw", "Print hardware move and presence signal");
    openknx.console.printHelpLine("vpm chNN pres", "Print presence time info for channel NN");
    openknx.console.printHelpLine("vpm chNN leave", "Print leave room info for channel NN");
    openknx.console.printHelpLine("vpm chNN state", "Print state flags for channel NN");
    openknx.console.printHelpLine("vpm chNN all", "Exec all channel commands for channel NN");

#ifdef HF_POWER_PIN
    if (ParamPM_HfPresence == VAL_PM_PS_Hf_HLKLD2420)
        static_cast<SensorHLKLD2420 *>(mPresenceSensor)->showHelp();
    else if (ParamPM_HfPresence == VAL_PM_PS_Hf_XenD107H)
        static_cast<SensorXenD107H *>(mPresenceSensor)->showHelp();
#endif
}

bool Presence::processCommand(const std::string iCmd, bool iDebugKo)
{
    bool lResult = false;

    if (!knx.configured())
        return lResult;

    if (iCmd.length() < 5)
        return lResult;

    if (iCmd.substr(0, 3) == "vpm")
    {
        if (iCmd.length() == 5 && iCmd.substr(4, 1) == "h")
        {
            // Command help
            openknx.console.writeDiagenoseKo("-> hw");
            openknx.console.writeDiagenoseKo("");
            openknx.console.writeDiagenoseKo("-> chNN pres");
            openknx.console.writeDiagenoseKo("");
            openknx.console.writeDiagenoseKo("-> chNN leave");
            openknx.console.writeDiagenoseKo("");
            openknx.console.writeDiagenoseKo("-> chNN state");
            openknx.console.writeDiagenoseKo("");
            openknx.console.writeDiagenoseKo("-> chNN all");
            openknx.console.writeDiagenoseKo("");
        }
        else if (iCmd.length() >= 8 || iCmd.substr(4, 2) == "ch")
        {
            // Command ch<nn>:
            // find channel and dispatch
            uint16_t lIndex = std::stoi(iCmd.substr(6, 2)) - 1;
            if (lIndex < mNumChannels)
            {
                // this is a channel request
                lResult = mChannel[lIndex]->processCommand(iCmd, iDebugKo);
            }
        }
        else if (iCmd.length() == 6 || iCmd.substr(4, 2) == "hw")
        {
            // output hardware move/presence state
            logInfoP("Move %d, Presence %d", mMove, mPresence);
            if (iDebugKo)
            {
                openknx.console.writeDiagenoseKo("Move %d, Pres %d", mMove, mPresence);
            }
            lResult = true;
        }
        else
        {
            // Commands starting with vpm are presence diagnose commands
            logInfoP("VPM command with bad args");
            if (iDebugKo)
            {
                openknx.console.writeDiagenoseKo("VPM: bad args");
            }
            lResult = true;
        }
    }
    else if (iCmd.substr(0, 3) == "hlk")
    {
#ifdef HF_POWER_PIN
        if (ParamPM_HfPresence == VAL_PM_PS_Hf_HLKLD2420)
            lResult = static_cast<SensorHLKLD2420 *>(mPresenceSensor)->processCommand(iCmd, iDebugKo);
        else if (ParamPM_HfPresence == VAL_PM_PS_Hf_XenD107H)
            static_cast<SensorXenD107H *>(mPresenceSensor)->processCommand(iCmd, iDebugKo);
#endif
    }

    return lResult;
}

void Presence::processInputKo(GroupObject &iKo)
{

    // we have to check first, if external KO are used
    sKoMap *lKoMap = nullptr;
    uint16_t lAsap = iKo.asap();
    while (mapKO(lAsap, &lKoMap))
    {
        uint16_t lKoIndex = lKoMap->koIndex;
        // here we check the range of internal KO per channel (KoIndex, not KoNumber)
        if ((lKoIndex >= PM_KoKOpLux && lKoIndex <= PM_KoKOpDayPhase) || lKoIndex == PM_KoKOpScene)
        {
            // we are in the Range of presence KOs
            uint8_t lChannelIndex = lKoMap->channelIndex;
            PresenceChannel *lChannel = mChannel[lChannelIndex];
            lChannel->processInputKo(iKo, lKoMap->koIndex);
        }
    }
    switch (lAsap)
    {
        case PM_KoPirSensitivity:
        {
            mPirSensitivity = iKo.value(getDPT(VAL_DPT_5));
            break;
        }
        case PM_KoHfSensitivity:
        {
            int8_t lHfSensitivity = iKo.value(getDPT(VAL_DPT_5));
            if (mHfSensitivity != lHfSensitivity)
            {
#ifdef HF_POWER_PIN
                switch (ParamPM_HfPresence)
                {
                    case VAL_PM_PS_Hf_MR24xxB1:
                        static_cast<SensorMR24xxB1 *>(mPresenceSensor)->sendCommand(RadarCmd_WriteSensitivity, lHfSensitivity);
                        break;
                    case VAL_PM_PS_Hf_HLKLD2420:
                        static_cast<SensorHLKLD2420 *>(mPresenceSensor)->writeSensitivity(lHfSensitivity);
                        break;
                    case VAL_PM_PS_Hf_XenD107H:
                        static_cast<SensorXenD107H *>(mPresenceSensor)->writeSensitivity(lHfSensitivity);
                        break;
                    default:
                        break;
                }
#endif
            }
            break;
        }
        case PM_KoScenario:
        {
            int8_t lScenario = iKo.value(getDPT(VAL_DPT_5));
            if (mScenario != lScenario)
            {
#ifdef HF_POWER_PIN
                switch (ParamPM_HfPresence)
                {
                    case VAL_PM_PS_Hf_MR24xxB1:
                        static_cast<SensorMR24xxB1 *>(mPresenceSensor)->sendCommand(RadarCmd_WriteScene, lScenario);
                        break;
                    default:
                        break;
                }
#endif
            }
            break;
        }
        case PM_KoHfReset:
#ifdef HF_POWER_PIN
            switch (ParamPM_HfPresence)
            {
                case VAL_PM_PS_Hf_MR24xxB1:
                    logDebugP("Do power cycle for MR24xxB1");
                    startPowercycleHfSensor();
                    break;
                case VAL_PM_PS_Hf_HLKLD2420:
                    logDebugP("Start calibration for HLKLD2420");
                    static_cast<SensorHLKLD2420 *>(mPresenceSensor)->forceCalibration();
                    break;
                case VAL_PM_PS_Hf_XenD107H:
                    logDebugP("Start calibration for XenD107H");
                    static_cast<SensorXenD107H *>(mPresenceSensor)->forceCalibration();
                    break;
                default:
                    break;
            }
#endif
            break;
        case PM_KoLEDMove:
            processLED(iKo.value(getDPT(VAL_DPT_1)), CallerKnxMove);
            break;
        case PM_KoLEDPresence:
            processLED(iKo.value(getDPT(VAL_DPT_1)), CallerKnxPresence);
            break;
        default:
            if (lAsap >= PM_KoOffset && lAsap < PM_KoOffset + mNumChannels * PM_KoBlockSize)
            {
                // we are in the Range of presence KOs
                uint8_t lChannelIndex = (lAsap - PM_KoOffset) / PM_KoBlockSize;
                PresenceChannel *lChannel = mChannel[lChannelIndex];
                lChannel->processInputKo(iKo);
            }
            break;
    }
}

bool Presence::getHardwarePresence()
{
    return mPresence;
}

bool Presence::getHardwareMove()
{
    return mMove;
}

// Starting all required sensors, this call may be blocking (with delay)
void Presence::startSensors()
{
#ifdef HF_POWER_PIN
    switch (ParamPM_HfPresence)
    {
        case VAL_PM_PS_Hf_MR24xxB1:
            logDebugP("Using HF sensor MR24xxB1");

            mPresenceSensor = openknxSensorDevicesModule.factory(SENS_MR24xxB1, MeasureType::Pres);
            static_cast<SensorMR24xxB1 *>(mPresenceSensor)->defaultSensorParameters(ParamPM_HfScenario - 1, ParamPM_HfSensitivity);
            break;
        case VAL_PM_PS_Hf_HLKLD2420:
            logDebugP("Using HF sensor HLKLD2420");

            mPresenceSensor = openknxSensorDevicesModule.factory(SENS_HLKLD2420, MeasureType::Pres);
            static_cast<SensorHLKLD2420 *>(mPresenceSensor)->defaultSensorParameters(ParamPM_HfSensitivity, ParamPM_HfDelayTime, ParamPM_HfRangeGateMin, ParamPM_HfRangeGateMax);
            break;
        case VAL_PM_PS_Hf_XenD107H:
            logDebugP("Using HF sensor XenD107H");

            mPresenceSensor = openknxSensorDevicesModule.factory(SENS_XEND107H, MeasureType::Pres);
            static_cast<SensorXenD107H *>(mPresenceSensor)->defaultSensorParameters(ParamPM_HfSensitivity, ParamPM_HfDelayTime, ParamPM_HfRangeGateMin, ParamPM_HfRangeGateMax);
            break;
        default:
            break;
    }

    switch (ParamPM_PirPresence)
    {
        case VAL_PM_PS_Pir_Digital:
            logDebugP("Using PIR sensor (digital)");
            break;
        case VAL_PM_PS_Pir_Analog:
            logDebugP("Using PIR sensor (analog)");
            mPirSensitivity = ParamPM_PirSensitivity;
            break;
    }
#endif

    switch (ParamPM_HWLux)
    {
        case VAL_PM_LUX_VEML:
            mBrightnessSensor = openknxSensorDevicesModule.factory(SENS_VEML7700, Lux);
            break;
        case VAL_PM_LUX_OPT:
            mBrightnessSensor = openknxSensorDevicesModule.factory(SENS_OPT300X, Lux);
            break;
        default:
            break;
    }
    // now start all sensors at the correct speed
    openknxSensorDevicesModule.beginSensors();
}

void Presence::switchHfSensor(bool iOn)
{
#ifdef HF_POWER_PIN
    switch (ParamPM_HfPresence)
    {
        case VAL_PM_PS_Hf_MR24xxB1:
            if (smartmf.hardwareRevision() == 1)
            {
                iOn = !iOn;
            }
            else
            {
                // we check für specific serial numbers, which have an inverted HF_POWER_PIN (hardware bug)
                const uint8_t specialCount = 11;
                const uint64_t special[specialCount] = {
                    // 0x1334842F,  // test - Devel Board Waldemar, where power pin has no function
                    // 0x47591F2E,  // Waldemar Wohnzimmer
                    0x23534121,
                    0x23364521,
                    0x23503321,
                    0x23464121,
                    0x23534821,
                    0x17493927,
                    0x17265A22,
                    0x173C1627,
                    0x175A3527,
                    0x173C1E27};

                uint32_t lSerial = knx.platform().uniqueSerialNumber();
                SERIAL_DEBUG.printf("\nswitchHfSensor: Turning Sensor on: %i\n", iOn);
                SERIAL_DEBUG.printf("Serial HEX 32: %08lX\n", lSerial);
                // if (0x2F843413 == lSerial) {
                //     Serial.println("Match Waldemar");
                // }
                for (uint8_t i = 0; i < specialCount; i++)
                    if (lSerial == special[i])
                    {
                        SERIAL_DEBUG.printf("switchHfSensor: Special board number %i found\n", i);
                        iOn = !iOn;
                        break;
                    }
            }

            SERIAL_DEBUG.printf("switchHfSensor: HF_POWER_PIN will be set to: %i\n", iOn);
            digitalWrite(HF_POWER_PIN, iOn ? HIGH : LOW);
            break;
        case VAL_PM_PS_Hf_HLKLD2420:
        case VAL_PM_PS_Hf_XenD107H:
            // sensor does not always connect correctly after power cycle,
            // let's keep it always on for now
            break;
        default:
            break;
    }
#endif
}

void Presence::startPowercycleHfSensor()
{
    switchHfSensor(false);
    mHfPowerCycleDelay = delayTimerInit();
}

void Presence::processPowercycleHfSensor()
{
    if (mHfPowerCycleDelay > 0 && delayCheck(mHfPowerCycleDelay, 15000))
    {
        switchHfSensor(true);
        mHfPowerCycleDelay = 0;
    }
}

// handles the following situations for hardware LEDs (presence- and move-LED):
// - turn on and off by hardware if selected in settings
// - turn off on lock through day phase
// - be aware of multiple channels creating led locks
// - turn on and off by knx
// - restore old led state on lock removal
void Presence::processLED(bool iOn, LedCaller iCaller)
{
    static int8_t sLedsLocked = false;
    static bool sLedMove = false;
    static bool sLedPresence = false;

    bool lLedMove = sLedMove;
    bool lLedPresence = sLedPresence;
    uint8_t lMoveLedParam = ParamPM_LEDMove;
    uint8_t lPresenceLedParam = ParamPM_LEDPresence;
    // we implement all led cases in one method
    switch (iCaller)
    {
        case CallerLock:
            sLedsLocked += (iOn) ? 1 : -1;
            // LEDs will keep the old values
            if (sLedsLocked <= 0)
                sLedsLocked = 0;
            break;
        case CallerMove:
            if (lMoveLedParam == VAL_PM_LedMove)
                lLedMove = iOn;
            if (lPresenceLedParam == VAL_PM_LedMove)
                lLedPresence = iOn;
            break;
        case CallerPresence:
            if (lMoveLedParam == VAL_PM_LedPresence)
                lLedMove = iOn;
            if (lPresenceLedParam == VAL_PM_LedPresence)
                lLedPresence = iOn;
            break;
        case CallerKnxMove:
            if (lMoveLedParam == VAL_PM_LedKnx)
                lLedMove = iOn;
            break;
        case CallerKnxPresence:
            if (lMoveLedParam == VAL_PM_LedKnx)
                lLedPresence = iOn;
            break;
        default:
            // both LEDs are switched
            lLedMove = iOn;
            lLedPresence = iOn;
            break;
    }
#ifdef MOVE_LED_PIN
    digitalWrite(MOVE_LED_PIN, MOVE_LED_PIN_ACTIVE_ON == (lLedMove && sLedsLocked == 0));
#endif
#ifdef PRESENCE_LED_PIN
    digitalWrite(PRESENCE_LED_PIN, PRESENCE_LED_PIN_ACTIVE_ON == (lLedPresence && sLedsLocked == 0));
#endif
    // store the current values in memory
    sLedMove = lLedMove;
    sLedPresence = lLedPresence;
}

void Presence::processHardwarePresence()
{
#ifdef HF_POWER_PIN
    if (mPresenceSensor != 0)
    {
        float lValue = 0;
        switch (ParamPM_HfPresence)
        {
            case VAL_PM_PS_Hf_MR24xxB1:
                if (openknxSensorDevicesModule.measureValue(MeasureType::Pres, lValue) && lValue != mPresenceCombined)
                {
                    mPresenceCombined = lValue;
                    bool lPresence = false;
                    uint8_t lMove;
                    uint8_t lFall;
                    uint8_t lAlarm;
                    if (SensorMR24xxB1::decodePresenceResult((uint8_t)lValue, lPresence, lMove, lFall, lAlarm))
                    {
                        if (lPresence != mPresence)
                        {
                            mPresence = lPresence;
                            processLED(mPresence, CallerPresence);
                            knx.getGroupObject(PM_KoPresenceOut).value(mPresence, getDPT(VAL_DPT_1));
                            if (mPresence)
                                PresenceTrigger = true;
                        }
                        if (lMove != mMove)
                        {
                            mMove = lMove;
                            processLED(mMove > 0, CallerMove);
                            knx.getGroupObject(PM_KoMoveOut).value(mMove, getDPT(VAL_DPT_5));
                            if (mMove)
                                MoveTrigger = true;
                        }
                    }
                }
                if (openknxSensorDevicesModule.measureValue(MeasureType::Speed, lValue))
                {
                    GroupObject &lKo = knx.getGroupObject(PM_KoMoveSpeedOut);
                    if ((uint8_t)lKo.value(getDPT(VAL_DPT_5001)) != (uint8_t)lValue)
                        lKo.value(lValue, getDPT(VAL_DPT_5001));
                }
                if (openknxSensorDevicesModule.measureValue(MeasureType::Scenario, lValue))
                {
                    if (mScenario != (int8_t)lValue)
                    {
                        mScenario = (int8_t)lValue;
                        GroupObject &lKo = knx.getGroupObject(PM_KoScenario);
                        lKo.value(mScenario, getDPT(VAL_DPT_5));
                    }
                }
                if (openknxSensorDevicesModule.measureValue(MeasureType::Sensitivity, lValue))
                {
                    if (mHfSensitivity != (int8_t)lValue)
                    {
                        mHfSensitivity = (int8_t)lValue;
                        GroupObject &lKo = knx.getGroupObject(PM_KoHfSensitivity);
                        lKo.value(mHfSensitivity, getDPT(VAL_DPT_5));
                    }
                }
                break;
            case VAL_PM_PS_Hf_HLKLD2420:
                if (openknxSensorDevicesModule.measureValue(MeasureType::Pres, lValue) && lValue != mPresenceCombined)
                {
                    mPresenceCombined = lValue;
                    bool lPresence = lValue == 1;
                    if (lPresence != mPresence)
                    {
                        mPresence = lPresence;
                        processLED(mPresence, CallerPresence);
                        knx.getGroupObject(PM_KoPresenceOut).value(mPresence, getDPT(VAL_DPT_1));
                        if (mPresence)
                            PresenceTrigger = true;
                    }
                }
                if (openknxSensorDevicesModule.measureValue(MeasureType::Sensitivity, lValue))
                {
                    if (mHfSensitivity != (int8_t)lValue)
                    {
                        mHfSensitivity = (int8_t)lValue;
                        GroupObject &lKo = knx.getGroupObject(PM_KoHfSensitivity);
                        lKo.value(mHfSensitivity, getDPT(VAL_DPT_5));
                    }
                }
                if (openknxSensorDevicesModule.measureValue(MeasureType::Distance, lValue))
                {
                    if (mDistance != lValue)
                    {
                        mDistance = lValue;
                        GroupObject &lKo = knx.getGroupObject(PM_KoMoveSpeedOut);
                        lKo.value(mDistance, getDPT(VAL_DPT_14));
                    }
                }
                break;
            case VAL_PM_PS_Hf_XenD107H:
                if (openknxSensorDevicesModule.measureValue(MeasureType::Pres, lValue) && lValue != mPresenceCombined)
                {
                    mPresenceCombined = lValue;
                    bool lPresence = lValue == 1;
                    if (lPresence != mPresence)
                    {
                        mPresence = lPresence;
                        processLED(mPresence, CallerPresence);
                        knx.getGroupObject(PM_KoPresenceOut).value(mPresence, getDPT(VAL_DPT_1));
                        if (mPresence)
                            PresenceTrigger = true;
                    }
                }
                if (openknxSensorDevicesModule.measureValue(MeasureType::Speed, lValue))
                {
                    GroupObject &lKo = knx.getGroupObject(PM_KoMoveSpeedOut);
                    if ((uint8_t)lKo.value(getDPT(VAL_DPT_5001)) != (uint8_t)lValue)
                        lKo.value(lValue, getDPT(VAL_DPT_5001));
                }
                if (openknxSensorDevicesModule.measureValue(MeasureType::Sensitivity, lValue))
                {
                    if (mHfSensitivity != (int8_t)lValue)
                    {
                        mHfSensitivity = (int8_t)lValue;
                        GroupObject &lKo = knx.getGroupObject(PM_KoHfSensitivity);
                        lKo.value(mHfSensitivity, getDPT(VAL_DPT_5));
                    }
                }
                if (openknxSensorDevicesModule.measureValue(MeasureType::Distance, lValue))
                {
                    if (mDistance != lValue)
                    {
                        mDistance = lValue;
                        GroupObject &lKo = knx.getGroupObject(PM_KoMoveSpeedOut);
                        lKo.value(mDistance, getDPT(VAL_DPT_14));
                    }
                }
                break;
            default:
                break;
        }
    }
#endif
#ifdef PIR_PIN
    bool pirTriggered = false;
    switch (ParamPM_PirPresence)
    {
        case VAL_PM_PS_Pir_Digital:
            pirTriggered = digitalRead(PIR_PIN) == PinStatus::HIGH;
            break;
        case VAL_PM_PS_Pir_Analog:
            uint32_t threshold = VAL_PM_PIR_Analog_Trigger_Max - (VAL_PM_PIR_Analog_Trigger_Max - VAL_PM_PIR_Analog_Trigger_Min) * (mPirSensitivity / 10.0);
            pirTriggered = analogRead(PIR_PIN) > threshold;
            break;
    }
    if (delayCheck(mPresenceStartupDelay, 30000))
        mPresenceStartupDelay = 0;
    pirTriggered = pirTriggered && mPresenceStartupDelay == 0;
    if (pirTriggered)
    {
        if (mMove == 0)
        {
            mPresenceDelay = millis();
            mMove = 1;
            mPresenceChanged = true;
        }
    }
    else
    {
        if (mMove > 0 && delayCheck(mPresenceDelay, 500))
        {
            mMove = 0;
            mPresenceChanged = true;
        }
    }
    if (mPresenceChanged)
    {
        mPresenceChanged = false;
        processLED(mMove > 0, CallerMove);
        knx.getGroupObject(PM_KoMoveOut).value(mMove, getDPT(VAL_DPT_1));
    }
    // add Trigger for any channel which registered for Hardware-PIR
#endif
}

void Presence::processHardwareLux()
{
    if (mBrightnessSensor != 0)
    {
        float lValue = 0;
        // we check for brightness only every second
        if (delayCheck(mBrightnessProcess, 1000) && openknxSensorDevicesModule.measureValue(MeasureType::Lux, lValue))
        {
            mBrightnessProcess = delayTimerInit();
            bool lSend = false;
            mLux = lValue;
            KoPM_LuxOut.valueNoSend(getHardwareBrightness(), getDPT(VAL_DPT_9));
            uint32_t lTimeDelta = ParamPM_LuxSendCycleDelayTimeMS;
            bool lDeltaAbsRel = ParamPM_LuxSendDeltaAbsRel;
            lSend = lTimeDelta > 0 && delayCheck(mBrightnessDelay, lTimeDelta);
            uint16_t lDelta = ParamPM_LuxSendDelta;
            if (lDelta > 0)
            {
                if (lDeltaAbsRel)
                {
                    // Rel
                    if (abs(mLux - mLuxLast) > 0.1)
                        lSend = lSend || (mLuxLast == 0) ? true : (abs((mLux - mLuxLast) / mLuxLast) * 100 >= lDelta); // Rel
                }
                else
                    lSend = lSend || abs(mLux - mLuxLast) >= lDelta; // Abs
            }
            if (lSend)
            {
                mLuxLast = mLux;
                mBrightnessDelay = delayTimerInit();
                KoPM_LuxOut.objectWritten();
            }
        }
    }
}

float Presence::getHardwareBrightness()
{
    return mLux + ParamPM_LuxOffsetPM;
}

#define NUM_CHANNELS_TO_PROCESS 5

void Presence::loop()
{
    if (!openknx.afterStartupDelay())
        return;

    static uint32_t sLoopTime = 0;
    sLoopTime = millis();
    if (mDoPresenceHardwareCycle)
    {
        processHardwarePresence();
        processHardwareLux();
        processPowercycleHfSensor();
        // Sensor::sensorLoop();
    }
    // we iterate through all channels and execute state logic
    uint8_t lChannelsProcessed = 0;
    // for (uint8_t lIndex = 0; lIndex < mNumChannels; lIndex++)
    while (lChannelsProcessed < mChannelsToProcess && openknx.freeLoopTime())
    {
        PresenceChannel *lChannel = mChannel[mChannelIterator++];
        lChannel->loop();
        lChannelsProcessed++;
        // the following operations are done only once after iteration of all channels
        if (mChannelIterator >= mNumChannels)
        {
            mChannelIterator = 0;
            // here we do actions which happen after all channels are iterated
            PresenceTrigger = false;
            MoveTrigger = false;
        }
    }
    if (lChannelsProcessed < mChannelsToProcess)
        logDebugP("PM did not process %i channels during loop as expected, just %i channels", mChannelsToProcess, lChannelsProcessed);
    if (millis() - sLoopTime > 3)
        logDebugP("PM LoopTime: %i", millis() - sLoopTime);
}

void Presence::setup()
{
    if (knx.configured())
    {
#ifdef HF_POWER_PIN
        pinMode(HF_POWER_PIN, OUTPUT);

        switch (ParamPM_HfPresence)
        {
            case VAL_PM_PS_Hf_MR24xxB1:
                // at startup, we turn HF-Sensor off
                digitalWrite(HF_POWER_PIN, LOW);
                break;
            case VAL_PM_PS_Hf_HLKLD2420:
            case VAL_PM_PS_Hf_XenD107H:
                // at startup, we turn HF-Sensor on
#ifndef HF_POWER_BCU
                digitalWrite(HF_POWER_PIN, HIGH);
#endif

                // ensure no data lost even for sensor raw data
                // up to 1288 bytes are send by the sensor at once
                HF_SERIAL.setFIFOSize(13000);
                break;
            default:
                break;
        }

        HF_SERIAL.setRX(HF_UART_RX_PIN);
        HF_SERIAL.setTX(HF_UART_TX_PIN);
        pinMode(PRESENCE_LED_PIN, OUTPUT);
        pinMode(MOVE_LED_PIN, OUTPUT);

        switch (ParamPM_HfPresence)
        {
            case VAL_PM_PS_Hf_MR24xxB1:
            case VAL_PM_PS_Hf_HLKLD2420:
                HF_SERIAL.begin(115200);
                break;
            case VAL_PM_PS_Hf_XenD107H:
                HF_SERIAL.begin(921600);
                break;
        }

#endif

#ifdef PIR_PIN
        pinMode(PIR_PIN, INPUT_PULLDOWN);
        mPresenceStartupDelay = delayTimerInit();

        if (ParamPM_PirPresence != VAL_PM_PS_None)
            digitalWrite(HF_POWER_PIN, HIGH);
#endif

        // setup channels, not possible in constructor, because knx is not configured there
        // get number of channels from knxprod
        mNumChannels = ParamPM_VisibleChannels; // knx.paramByte(PM_PMChannels);
        mChannelsToProcess = MIN(mNumChannels, NUM_CHANNELS_TO_PROCESS);
        // calculate parameter block size for day phase parameters
        PresenceChannel::setDayPhaseParameterSize(PM_pBBrightnessAuto - PM_pABrightnessAuto);
        for (uint8_t lIndex = 0; lIndex < mNumChannels; lIndex++)
        {
            mChannel[lIndex] = new PresenceChannel(lIndex);
            mChannel[lIndex]->setup();
        }
        mDoPresenceHardwareCycle = (ParamPM_HfPresence > 0) || (ParamPM_HWLux > 0);
        if (mDoPresenceHardwareCycle)
            startPowercycleHfSensor();
        startSensors();
    }
}