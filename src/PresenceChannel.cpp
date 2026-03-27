#include "PresenceChannel.h"
#include "Presence.h"

uint8_t PresenceChannel::sDayPhaseParameterSize = 0;

PresenceChannel::PresenceChannel(uint8_t iChannelNumber)
{
    _channelIndex = iChannelNumber; // Zero based
    pCurrentState = 0;
    pCurrentValue = 0;
}

PresenceChannel::~PresenceChannel() {}

void PresenceChannel::setDayPhaseParameterSize(uint8_t iSize)
{
    sDayPhaseParameterSize = iSize;
}

/******************************
 * Parameter helper
 * ***************************/
uint32_t PresenceChannel::calcParamIndex(uint16_t iParamIndex, bool iWithPhase)
{
    uint16_t lParamIndex = iParamIndex + (iWithPhase ? sDayPhaseParameterSize * mCurrentDayPhase : 0);
    uint32_t lResult = lParamIndex + channelIndex() * PM_ParamBlockSize + PM_ParamBlockOffset;
    return lResult;
}

uint16_t PresenceChannel::calcKoNumber(uint8_t iKoIndex)
{
    uint16_t lResult = iKoIndex + channelIndex() * PM_KoBlockSize + PM_KoOffset;
    return lResult;
}

// calculates correct KoIndex for this channel, returns -1 for invalid !!!
int8_t PresenceChannel::calcKoIndex(uint16_t iKoNumber)
{
    int16_t lResult = (iKoNumber - PM_KoOffset);
    // check if channel is valid
    if ((int8_t)(lResult / PM_KoBlockSize) == channelIndex())
        lResult = lResult % PM_KoBlockSize;
    else
        lResult = -1;
    return (int8_t)lResult;
}

// this function also respects internal KO, even though they are addressed with external KO addresses
GroupObject *PresenceChannel::getKo(uint8_t iKoIndex)
{
    uint8_t lParamIndex;
    uint16_t lKoNumber;
    // internal objects have to be enumerated here explicitly
    switch (iKoIndex)
    {
        case PM_KoKOpLux:
        case PM_KoKOpPresence1:
        case PM_KoKOpPresence2:
        case PM_KoKOpSetAuto:
        case PM_KoKOpSetManual:
        case PM_KoKOpAktorState:
        case PM_KoKOpLock:
        case PM_KoKOpReset:
        case PM_KoKOpDayPhase:
            lParamIndex = PM_pNumLux + iKoIndex * 2;
            break;
        case PM_KoKOpScene:
            lParamIndex = PM_pNumScene;
            break;
        default:
            lParamIndex = 0;
            break;
    }
    if (lParamIndex && paramBit(lParamIndex, 0x80))
        lKoNumber = paramWord(lParamIndex) & 0x7FFF;
    else
        lKoNumber = calcKoNumber(iKoIndex);
    return &knx.getGroupObject(lKoNumber);
}

bool PresenceChannel::paramBit(uint16_t iParamIndex, uint8_t iParamMask, bool iWithPhase /* = false */)
{
    return (knx.paramByte(calcParamIndex(iParamIndex, iWithPhase)) & iParamMask);
}

uint8_t PresenceChannel::paramByte(uint16_t iParamIndex, uint8_t iParamMask, uint8_t iParamShift, bool iWithPhase /* = false */)
{
    return (knx.paramByte(calcParamIndex(iParamIndex, iWithPhase)) & iParamMask) >> iParamShift;
}

uint8_t PresenceChannel::paramByte(uint16_t iParamIndex, bool iWithPhase /* = false */)
{
    return knx.paramByte(calcParamIndex(iParamIndex, iWithPhase));
}

uint16_t PresenceChannel::paramWord(uint16_t iParamIndex, bool iWithPhase /* = false */)
{
    return knx.paramWord(calcParamIndex(iParamIndex, iWithPhase));
}

uint32_t PresenceChannel::paramInt(uint16_t iParamIndex, bool iWithPhase /* = false */)
{
    return knx.paramInt(calcParamIndex(iParamIndex, iWithPhase));
}

uint32_t PresenceChannel::paramTimeDelay(uint16_t iParamIndex, bool iWithPhase /* = false */, bool iAsSeconds /* = false */)
{
    uint32_t lTime = paramWord(iParamIndex, iWithPhase);
    lTime = paramDelay(lTime);
    lTime = iAsSeconds ? lTime / 1000 : lTime;
    return lTime;
}

bool PresenceChannel::processCommand(const std::string iCmd, bool iDebugKo)
{
    bool lResult = false;
    if (iCmd.substr(0, 6) != "vpm ch" || iCmd.length() < 8)
        return lResult;
    if (iCmd.length() > 8)
    {
        bool lIsAll = iCmd.substr(9, 1) == "a";
        if (lIsAll || iCmd.substr(9, 1) == "p") // pres or all
        {
            // output remaining presence time
            int16_t lPresence = 0;
            int16_t lPresenceShort = 0;
            if (pCurrentState & STATE_PRESENCE)
            {
                // we present both, long and short presence
                lPresence = getKo(PM_KoKOpPresenceDelay)->value(DPT_Value_2_Ucount);
                if (pPresenceDelayTime > 0)
                    lPresence = lPresence - (uint16_t)((millis() - pPresenceDelayTime) / 1000);

                if (pCurrentState & STATE_PRESENCE_SHORT)
                {
                    lPresenceShort = paramTimeDelay(PM_pAPresenceShortDurationBase, true, true);
                    lPresenceShort = lPresenceShort - (uint16_t)((millis() - pPresenceShortDelayTime) / 1000);
                    if (lPresenceShort >= 0)
                    {
                        // Short presence duration output is "L mm:ss S m:ss"
                        logInfoP("Long %02d:%02d, Short %1d:%02d", (lPresence / 60) % 60, lPresence % 60, (lPresenceShort / 60) % 10, lPresenceShort % 60);
                        if (iDebugKo)
                            openknx.console.writeDiagnoseKo("L %02d:%02d S %1d:%02d", (lPresence / 60) % 60, lPresence % 60, (lPresenceShort / 60) % 10, lPresenceShort % 60);
                    }
                    else
                    {
                        // Short presence evaluation output is "L mm:ss D m:ss"
                        logInfoP("Long %02d:%02d, Delay %1d:%02d", (lPresence / 60) % 60, lPresence % 60, (-lPresenceShort / 60) % 10, -lPresenceShort % 60);
                        if (iDebugKo)
                            openknx.console.writeDiagnoseKo("L %02d:%02d D %1d:%02d", (lPresence / 60) % 60, lPresence % 60, (-lPresenceShort / 60) % 10, -lPresenceShort % 60);
                    }
                }
                else
                {
                    // Long presence output is "L mm:ss "
                    logInfoP("Long %02d:%02d ", (lPresence / 60) % 60, lPresence % 60);
                    if (iDebugKo)
                        openknx.console.writeDiagnoseKo("L %02d:%02d ", (lPresence / 60) % 60, lPresence % 60);
                }
            }
            else if (pCurrentState & STATE_RUNNING)
            {
                logInfoP("There is no presence");
                if (iDebugKo)
                    openknx.console.writeDiagnoseKo("No presence");
            }
            else
            {
                logInfoP("This channel is inactive");
                if (iDebugKo)
                    openknx.console.writeDiagnoseKo("Inactive");
            }
            if (iDebugKo && lIsAll)
                openknx.console.writeDiagnoseKo("");
            lResult = true;
        }
        if (lIsAll || iCmd.substr(9, 1) == "l") // leave or all
        {
            // output leave room modes
            // p<nn>l
            // L (OFF|TOT|T+R|B+T|BTR) T mm:ss
            if (isLeaveRoom())
            {
                uint8_t lIndex = 0;
                int16_t lTime = 0;
                const char *lModes = "OFFTOTT+RB+TBTR";
                char lOutput[15] = {0};
                // we present leave room mode
                lOutput[lIndex++] = 'L';
                lOutput[lIndex++] = ' ';
                PT_LeaveRoomMode lLeaveRoomMode = ParamPM_pLeaveRoomModeAll;
                for (uint8_t lCount = 0; lCount < 3; lCount++)
                    lOutput[lIndex++] = lModes[((uint8_t)lLeaveRoomMode * 3 + lCount)];
                lOutput[lIndex++] = ' ';
                lOutput[lIndex] = 0; // do not increment lIndex here
                if (pDowntimeDelayTime > 0)
                {
                    lTime = paramTimeDelay(PM_pDowntimeOffBase, false, true);
                    lTime = lTime - (uint16_t)((millis() - pDowntimeDelayTime) / 1000);
                    // Downtime output is "T mm:ss "
                    if (lTime >= 0)
                        snprintf(lOutput + lIndex, 8, "T %02d:%02d", (lTime / 60) % 60, lTime % 60);
                    else
                        snprintf(lOutput + lIndex, 8, "T-%02d:%02d", (-lTime / 60) % 60, -lTime % 60);
                }
                logInfoP(lOutput);
                if (iDebugKo)
                    openknx.console.writeDiagnoseKo(lOutput);
            }
            else if (pCurrentState & STATE_RUNNING)
            {
                logInfoP("Leave room is off");
                if (iDebugKo)
                    openknx.console.writeDiagnoseKo("No leave room");
            }
            else
            {
                logInfoP("Inactive");
                if (iDebugKo)
                    openknx.console.writeDiagnoseKo("Inactive");
            }
            if (iDebugKo && lIsAll)
                openknx.console.writeDiagnoseKo("");
            lResult = true;
        }
        if (lIsAll || iCmd.substr(9, 1) == "s") // state or all
        {
            // short version
            // "[NAM][01] D[1-4][1-4] [L-][H-][X-][R-][T-]"
            // N=normal, A=auto, M=manual
            // 0=off, 1=on
            // D=day phase
            // 1-4=current phase
            // 1-4=next phase
            // L=is lock, -=unlock
            // H=in helligkeitsberechnung, -=normal
            // X=disable brightness handling, -=normal
            // R=leave Room is active, -=not active
            // T=in totzeit, -=normal
            uint8_t lIndex = 0;
            char lOutput[15] = {0};
            if (pCurrentState & STATE_MANUAL)
                lOutput[lIndex++] = 'M';
            else if (pCurrentState & STATE_AUTO)
                lOutput[lIndex++] = 'A';
            else
                lOutput[lIndex++] = 'N';

            lOutput[lIndex++] = (pCurrentValue & PM_BIT_OUTPUT_SET) ? '1' : '0';
            lOutput[lIndex++] = ' ';
            lOutput[lIndex++] = 'D';
            lOutput[lIndex++] = ((mCurrentDayPhase >= 0) ? mCurrentDayPhase : 0) + 49;
            lOutput[lIndex++] = ((mNextDayPhase >= 0) ? mNextDayPhase : 0) + 49;
            lOutput[lIndex++] = ' ';
            lOutput[lIndex++] = (pCurrentState & STATE_LOCK) ? 'L' : '-';
            lOutput[lIndex++] = (pCurrentState & STATE_ADAPTIVE) ? 'H' : '-';
            lOutput[lIndex++] = (pCurrentValue & PM_BIT_DISABLE_BRIGHTNESS) ? 'X' : ((pCurrentState & STATE_AUTO) && paramBit(PM_pABrightnessSuppress, PM_pABrightnessSuppressMask, true)) ? 'M' : '-';
            lOutput[lIndex++] = (pCurrentState & STATE_LEAVE_ROOM) ? 'R' : '-';
            lOutput[lIndex++] = (pCurrentState & STATE_DOWNTIME) ? 'T' : '-';
            // 3 char free
            lOutput[lIndex++] = 0;
            logInfoP(lOutput);
            if (iDebugKo)
                openknx.console.writeDiagnoseKo(lOutput);
            lResult = true;
        }
    }
    return lResult;
}

void PresenceChannel::processInputKo(GroupObject &iKo, int8_t iKoIndex)
{
    // we process KO only if we are running
    if (pCurrentState & (STATE_RUNNING | STATE_READ_REQUESTS))
    {
        // search for correct KO
        uint8_t lKoIndex = (iKoIndex >= 0) ? iKoIndex : calcKoIndex(iKo.asap());
        switch (lKoIndex)
        {
            case PM_KoKOpLuxOn:
                startBrightnessOff();
                // a startup brightness is triggered by a new value to KO(LuxOff)
                break;
            case PM_KoKOpLux:
            case PM_KoKOpLuxOff:
                startBrightnessPrepare();
                break;
            case PM_KoKOpPresence1:
                startPresencePrepare(STATE_KO_PRESENCE1);
                break;
            case PM_KoKOpPresence2:
                startPresencePrepare(STATE_KO_PRESENCE2);
                break;
            case PM_KoKOpSetAuto:
                startAutoPrepare();
                break;
            case PM_KoKOpSetManual:
                startManualPrepare();
                break;
            case PM_KoKOpAktorState:
                // Actor state changed
                startActorState();
                break;
            case PM_KoKOpLock:
                // lock mode
                startLock();
                break;
            case PM_KoKOpReset:
                // reset PM, we just react on ON-telegrams
                startReset();
                break;
            case PM_KoKOpDayPhase:
                startDayPhasePrepare();
                break;
            case PM_KoKOpScene:
                startSceneCommand();
                break;
            case PM_KoKOpChangeDimAbs:
            case PM_KoKOpChangeDimRel:
            case PM_KoKOpChangeSwitch:
                startAdaptiveBrightness(); // this is already decoupled
                break;
            default:
                // do nothing
                break;
        }
    }
}

void PresenceChannel::startSceneCommand()
{
    if (pCurrentState & STATE_KO_SCENE)
    {
        uint8_t lValue = getKo(PM_KoKOpScene)->value(DPT_Bool);
        logInfoP("CH %i: Zu viele Szenen direkt aufeinanderfolgend!\n", channelIndex() + 1);
        logInfoP("   Die Szene %i wurde per KO gesetzt, ohne dass die vorhergehende Szene ausgeführt werden konnte!\n", lValue + 1);
    }
    pCurrentState |= STATE_KO_SCENE;
}

void PresenceChannel::processSceneCommand()
{
    pCurrentState &= ~STATE_KO_SCENE;
    // get scene number
    uint8_t lSceneFromKo = (uint8_t)getKo(PM_KoKOpScene)->value(DPT_SceneNumber) + 1;
    // check if scene is used
    for (uint8_t lIndex = 0; lIndex < 10; lIndex++)
    {
        uint8_t lSceneFromParam = paramByte(PM_pScene0 + lIndex);
        if (lSceneFromParam == lSceneFromKo)
        {
            PT_SceneAction lAction = (PT_SceneAction)paramByte(PM_pSceneAction0 + lIndex);
            switch (lAction)
            {
                case PT_SceneAction::aendert_Helligkeit_im_Raum:
                    startAdaptiveBrightness();
                    break;
                case PT_SceneAction::Automatik_uebersteuern_mit_AUS:
                    startAuto(false, false);
                    break;
                case PT_SceneAction::Automatik_uebersteuern_mit_EIN:
                    startAuto(true, false);
                    break;
                case PT_SceneAction::Manuell_uebersteuern_mit_AUS:
                    startManual(false, false);
                    break;
                case PT_SceneAction::Manuell_uebersteuern_mit_EIN:
                    startManual(true, false);
                    break;
                case PT_SceneAction::Manuell_aktivieren:
                    startManual(pCurrentValue & PM_BIT_OUTPUT_SET, true);
                    break;
                case PT_SceneAction::Manuell_deaktivieren:
                    startAuto(pCurrentValue & PM_BIT_OUTPUT_SET, true);
                    break;
                case PT_SceneAction::Sperren_und_AUS_senden:
                    onLock(true, PT_PMLock::AUS_gesendet, PT_PMLock::nichts_gesendet);
                    break;
                case PT_SceneAction::Sperren_und_EIN_senden:
                    onLock(true, PT_PMLock::EIN_gesendet, PT_PMLock::nichts_gesendet);
                    break;
                case PT_SceneAction::Sperren_und_nichts_senden:
                    onLock(true, PT_PMLock::nichts_gesendet, PT_PMLock::nichts_gesendet);
                    break;
                case PT_SceneAction::Sperre_aufheben_und_Zustand_senden:
                    onLock(false, PT_PMLock::nichts_gesendet, PT_PMLock::Aktueller_Zustand_gesendet);
                    break;
                case PT_SceneAction::Sperre_aufheben_und_nichts_senden:
                    onLock(false, PT_PMLock::nichts_gesendet, PT_PMLock::nichts_gesendet);
                    break;
                case PT_SceneAction::Sperre_aufheben_und_AUS_senden:
                    onLock(false, PT_PMLock::nichts_gesendet, PT_PMLock::AUS_gesendet);
                    break;
                case PT_SceneAction::Sperre_aufheben_und_EIN_senden:
                    onLock(false, PT_PMLock::nichts_gesendet, PT_PMLock::EIN_gesendet);
                    break;
                case PT_SceneAction::Raum_verlassen:
                    startLeaveRoom(false);
                    break;
                case PT_SceneAction::Reset_ausloesen:
                    onReset();
                    break;
                case PT_SceneAction::Zur_Tagesphase_1_wechseln:
                    startDayPhase(0);
                    break;
                case PT_SceneAction::Zur_Tagesphase_2_wechseln:
                    startDayPhase(1);
                    break;
                case PT_SceneAction::Zur_Tagesphase_3_wechseln:
                    startDayPhase(2);
                    break;
                case PT_SceneAction::Zur_Tagesphase_4_wechseln:
                    startDayPhase(3);
                    break;
                case PT_SceneAction::Zur_Tagesphase_1_sofort_wechseln:
                    startDayPhase(0, true);
                    break;
                case PT_SceneAction::Zur_Tagesphase_2_sofort_wechseln:
                    startDayPhase(1, true);
                    break;
                case PT_SceneAction::Zur_Tagesphase_3_sofort_wechseln:
                    startDayPhase(2, true);
                    break;
                case PT_SceneAction::Zur_Tagesphase_4_sofort_wechseln:
                    startDayPhase(3, true);
                    break;
                default:
                    break;
            }
        }
    }
}

// channel startup delay
void PresenceChannel::startStartupDelay()
{
    pOnDelay = delayTimerInit();
    pCurrentState |= STATE_STARTUP;
}

void PresenceChannel::processStartupDelay()
{
    if (delayCheck(pOnDelay, ParamPM_pChannelDelayTimeMS))
    {
        // we waited enough, remove State marker
        pCurrentState &= ~STATE_STARTUP;
        // set running state if the channel is active
        if (ParamPM_pChannelActive == PT_ChannelActive::Aktiv)
            startReadRequests();
        pOnDelay = 0;
    }
}

void PresenceChannel::sendReadRequest(uint8_t iKoIndex)
{
    GroupObject *lKo = getKo(iKoIndex);
    // we handle input KO and we send only read requests, if KO is uninitialized
    // in case the KO is transmitting (as in input), we know that an read request was already sent by someone else
    if (!lKo->initialized())
        lKo->requestObjectRead();
    // else if (lKo->commFlag() != ComFlag::Transmitting)
    // {
    //     // if we suppress a read request because the input value already exists, we nevertheless have
    //     // to react on this value as if a telegram would be received (because we are in a startup phase and we have to init correctly)
    //     processInputKo(*lKo, iKoIndex);
    // }
}

// send states after channel startup time, do this only once
void PresenceChannel::startReadRequests()
{
    // in this phase there will be no output processing
    pCurrentState |= STATE_READ_REQUESTS;
}

void PresenceChannel::processReadRequests()
{
    // this method is called after startup delay and executes read requests, which should just happen once after startup

    if (pReadRequestCounter < 255 && delayCheck(pReadRequestDelay, pReadRequestPause))
    {
        pReadRequestCounter += 1;
        pReadRequestDelay = delayTimerInit();
        switch (pReadRequestCounter)
        {
            case 1:
                if (ParamPM_pStartReadDayPhase)
                    sendReadRequest(PM_KoKOpDayPhase);
                break;
            case 2:
                if (ParamPM_pStartReadLux)
                    sendReadRequest(PM_KoKOpLux);
                break;
            case 3:
                if (ParamPM_pStartReadPresence1)
                    sendReadRequest(PM_KoKOpPresence1);
                break;
            case 4:
                if (ParamPM_pStartReadPresence2)
                    sendReadRequest(PM_KoKOpPresence2);
                break;
            case 5:
                if (ParamPM_pStartReadAktorState)
                    sendReadRequest(PM_KoKOpAktorState);
                break;
            case 6:
                if (ParamPM_pStartReadLock)
                    sendReadRequest(PM_KoKOpLock);
                break;
            case 7:
                if (ParamPM_pStartReadScene)
                    sendReadRequest(PM_KoKOpScene);
                break;
            case 8:
                pReadRequestPause = 1500;
            default:
                pReadRequestCounter = 255; // all read requests processed
                startRunning();
                break;
        }
    }
}

void PresenceChannel::startRunning()
{
    // current idea: We initialize all KO which were not red after reading
    pCurrentState &= ~STATE_READ_REQUESTS;

    GroupObject *lKo = getKo(PM_KoKOpLux);
    // external brightness is 0 (dark, pm works also in fallback mode)
    if (!lKo->initialized())
        lKo->value(0.0f, DPT_Value_Lux);
    // presence and move are false
    lKo = getKo(PM_KoKOpPresence1);
    if (!lKo->initialized())
        lKo->value(false, DPT_Bool);
    lKo = getKo(PM_KoKOpPresence2);
    if (!lKo->initialized())
        lKo->value(false, DPT_Bool);
    // actor state has to be handled in ProcessActorState !!!
    lKo = getKo(PM_KoKOpIsManual);
    if (!lKo->initialized())
        lKo->value(false, DPT_Bool);

    lKo = getKo(PM_KoKOpLock);
    if (!lKo->initialized())
        switch (ParamPM_pLockType)
        {
            case VAL_PM_LockTypePriority:
                lKo->value((uint8_t)0, DPT_Switch_Control);
                break;
            case VAL_PM_LockTypeLock:
                lKo->value((uint8_t)0, DPT_Bool);
                break;
            default:
                // do nothing
                break;
        }
    // scene KO must not be initialized

    // at the beginning we are on day phase according to KO
    uint8_t lDayPhase = 0;

    lKo = getKo(PM_KoKOpDayPhase);
    if (lKo->initialized())
        lDayPhase = getDayPhaseFromKO();
    if (lDayPhase == 255) // invalid phase
        lDayPhase = 0;
    onDayPhase(lDayPhase, false);
    pCurrentState |= STATE_RUNNING;
}

int8_t PresenceChannel::getDayPhaseFromKO()
{
    // derive day phase from scene number
    int8_t lPhaseCount = ParamPM_pPhaseCount;
    if (lPhaseCount == 1 && ParamPM_pPhaseBool) // PhaseCount is zero based (0 = 1 Phase)
    {
        lPhaseCount = (uint8_t)getKo(PM_KoKOpDayPhase)->value(DPT_Bool);
    }
    else if (lPhaseCount >= 1) // no calculation if only one phase defined
    {
        uint8_t lScene = (uint8_t)getKo(PM_KoKOpDayPhase)->value(DPT_SceneNumber) + 1;
        for (; lPhaseCount >= 0; lPhaseCount--)
            if (paramByte(PM_pPhase1Scene + lPhaseCount) == lScene)
                break;
    }
    return lPhaseCount;
}

void PresenceChannel::startDayPhasePrepare()
{
    // if (pCurrentState & STATE_KO_DAY_PHASE) {
    //     uint8_t lValue = getKo(PM_KoKOpDayPhase)->value(getDPT(VAL_DPT_1));
    //     logInfoP("CH %i: Zu viele Tagesphasen direkt aufeinanderfolgend!\n", channelIndex() + 1);
    //     logInfoP("   Die Tagesphase %i wurde per KO gesetzt, ohne dass die vorhergehende Tagesphase ausgeführt werden konnte!\n", lValue + 1);
    // }
    pCurrentState |= STATE_KO_DAY_PHASE;
}

void PresenceChannel::processDayPhasePrepare()
{
    pCurrentState &= ~STATE_KO_DAY_PHASE;
    startDayPhase(255);
}

void PresenceChannel::startDayPhase(uint8_t iPhase, bool iForce /* = false*/)
{
    // derive day phase from scene number or from parameter
    if (iPhase == 255) 
    {
        int8_t lDayPhase = getDayPhaseFromKO();
        if (lDayPhase < 0) // invalid phase-KO, do nothing
            return; 
        mNextDayPhase = lDayPhase;
    }
    else
        mNextDayPhase = iPhase;

    // get the number of Phases defined
    int8_t lPhaseCount = ParamPM_pPhaseCount;

    // first check, if day phase is valid and if it really changed
    if (mNextDayPhase < 0 || mCurrentDayPhase == mNextDayPhase || mNextDayPhase > lPhaseCount)
        return;

    // check if delayed day phase execution is requested
    if (iForce || ParamPM_pPhaseChange)
    {
        // we change immediately
        onDayPhase(mNextDayPhase, false);
    }
    else
    {
        // we change on next (internal) off at output
        pCurrentState |= STATE_DAY_PHASE_CHANGE;
    }
}

void PresenceChannel::processDayPhase()
{
    if (!(pCurrentValue & PM_BIT_OUTPUT_SET))
    {
        // output is OFF, we can safely change day phase
        pCurrentState &= ~STATE_DAY_PHASE_CHANGE;

        // first check, if day phase is valid and if it really changed
        if (mNextDayPhase < 0 || mCurrentDayPhase == mNextDayPhase)
            return;
        onDayPhase(mNextDayPhase, false);
    }
}

void PresenceChannel::onDayPhase(uint8_t iPhase, bool iIsStartup /* = false */)
{
    // cleanup old day phase, here we are still in old day phase
    // remove hardware LED lock
    if (paramBit(PM_pALockHardwareLEDs, PM_pALockHardwareLEDsMask, true))
        openknxPresenceModule.processLED(false, Presence::CallerLock);

    // set the new day phase
    mCurrentDayPhase = iPhase;

    // process hardware led lock
    if (paramBit(PM_pALockHardwareLEDs, PM_pALockHardwareLEDsMask, true))
        openknxPresenceModule.processLED(true, Presence::CallerLock);

    // Here we preset KO values, which directly influence PM processing
    // with according parameters from day phase.
    // presence delay
    uint32_t lPresenceDelay = paramTimeDelay(PM_pAPresenceDelayBase, true, true);
    getKo(PM_KoKOpPresenceDelay)->value(lPresenceDelay, DPT_Value_2_Ucount);
    // brightness to turn on light
    uint32_t lBrightness = paramWord(PM_pABrightnessOn, true);
    getKo(PM_KoKOpLuxOn)->value(lBrightness, DPT_Value_Lux);
    // brightness to turn off light
    processBrightnessOff();
    // if short presence is off, we stop a potential running short presence from other phase
    if (!paramBit(PM_pAPresenceShort, PM_pAPresenceShortMask, true))
        endPresenceShort();

    // day phase change should resend except on startup
    if (!iIsStartup)
        forceOutput((ParamPM_pOutput1SendAdditional & VAL_PM_Force_DayPhase) || (ParamPM_pOutput2SendAdditional & VAL_PM_Force_DayPhase), VAL_PM_Force_DayPhase);

    // day phase change should also trigger presence processing
    startPresence(false, false);
}

bool PresenceChannel::getRawPresence(bool iJustMove /* false */)
{
    if (!knx.configured()) return false;
    // iJustMove is ignored, if there is only presence available
    bool lJustMove = false;
    // are external inputs offering presence and move?
    if (ParamPM_pPresenceInputs == VAL_PM_PI_PresenceMove)
        lJustMove = iJustMove;
    bool lPresence = getKo(PM_KoKOpPresence2)->value(DPT_Bool);
    if (!lPresence && !lJustMove)
        lPresence = getKo(PM_KoKOpPresence1)->value(DPT_Bool);
    // if hardware presence sensor is available, we evaluate its value
    if (!lPresence)
        lPresence = getHardwarePresence(iJustMove); // for internal sensors, we use iJustMove !!!
    return lPresence;
}

float PresenceChannel::getRawBrightness()
{
    float lResult = NO_NUM;
    if (ParamPM_pBrightnessIntern)
        lResult = openknxPresenceModule.getHardwareBrightness();
    else
    {
        GroupObject *lKo = getKo(PM_KoKOpLux);
        ComFlag lComFlag = lKo->commFlag();
        if (lComFlag != ComFlag::Uninitialized && lComFlag != ComFlag::Transmitting)
            lResult = lKo->value(DPT_Value_Lux);
    }
    // returns NO_NUM if brightness-ko was never set
    return lResult;
}

bool PresenceChannel::getHardwareMove()
{
    bool lPresence = false;
    if (ParamPM_pMoveUsage == VAL_PM_MoveUsageBoth || ParamPM_pMoveUsage == VAL_PM_MoveUsageHF)
        lPresence = openknxPresenceModule.getHardwareMoveHF();
    if (!lPresence && (ParamPM_pMoveUsage == VAL_PM_MoveUsageBoth || ParamPM_pMoveUsage == VAL_PM_MoveUsagePIR))
        lPresence = openknxPresenceModule.getHardwareMovePIR();
    return lPresence;
}

bool PresenceChannel::getHardwarePresence(bool iJustMove /* false */)
{
    // if hardware presence sensor is available, we evaluate its value
    bool lPresence = false;
    if (ParamPM_pPresenceUsage >= VAL_PM_PresenceUsageMove) {
        lPresence = getHardwareMove();
    }
    if (!iJustMove && !lPresence && ParamPM_pPresenceUsage == VAL_PM_PresenceUsagePresence)
        lPresence = openknxPresenceModule.getHardwarePresence();
    return lPresence;
}

// entry point for presence trigger (means start or restart presence delay)
void PresenceChannel::startPresenceTrigger(bool iManual)
{
    startPresence(true, iManual);
    startPresence(false, iManual);
    // pPresenceDelayTime = delayTimerInit(); // diese Variante führt zu Issue #12
}

void PresenceChannel::startHardwarePresence()
{
    // we derive here a presence trigger from hardware sensor by polling
    // we have to eval Hardware move if required
    bool lValue = false;
    bool lTrigger = false;
    if (ParamPM_pPresenceUsage >= VAL_PM_PresenceUsageMove)
    {
        lValue = getHardwareMove();
        if (mHardwareMove != lValue)
        {
            mHardwareMove = lValue;
            lTrigger = true;
        }
    }
    // and we eval hardware presence if required
    if (!lValue && ParamPM_pPresenceUsage == VAL_PM_PresenceUsagePresence)
    {
        lValue = openknxPresenceModule.getHardwarePresence();
        if (mHardwarePresence != lValue)
        {
            mHardwarePresence = lValue;
            lTrigger = true;
        }
    }
    // but we trigger just once
    if (lTrigger)
        startPresence(lValue, false);
}

void PresenceChannel::startHardwareBrightness()
{
    // if hardware brightness is handled, no external brightness is available
    if (ParamPM_HWLux > 0 && !ParamPM_pBrightnessIndependent)
    {
        // we have to poll here
        if (delayCheck(mBrightnessPollDelay, 1500))
        {
            mBrightnessPollDelay = delayTimerInit();
            startBrightness();
        }
    }
}

void PresenceChannel::startPresencePrepare(uint32_t iState)
{
    pCurrentState |= iState;
}

void PresenceChannel::processPresencePrepare(uint32_t iState)
{
    bool lIsTrigger = false;
    bool lIsKeepAlive = false;
    GroupObject *lKo = nullptr;
    if (iState == (STATE_KO_PRESENCE1))
    {
        // which kind of presence information do we get
        lIsTrigger = ParamPM_pPresenceType;
        lIsKeepAlive = ParamPM_pPresenceKeepAlive;
        lKo = getKo(PM_KoKOpPresence1);
    }
    else if (iState == (STATE_KO_PRESENCE2))
    {
        // change of presence information
        lIsTrigger = ParamPM_pMoveType;
        lIsKeepAlive = ParamPM_pMoveKeepAlive;
        lKo = getKo(PM_KoKOpPresence2);
    }
    pCurrentState &= ~iState;
    startPresence(lIsTrigger, lIsKeepAlive, lKo);
}

// helper entry point for presence calculation initiated by a KO
void PresenceChannel::startPresence(bool iIsTrigger, bool iIsKeepAlive, GroupObject *iKo)
{
    bool lPresenceValue = iKo->value(DPT_Bool);
    bool lAllowStartPresence = !iIsKeepAlive || (pCurrentState & STATE_PRESENCE);
    // we ignore explicitly OFF telegrams of triggered input
    if (iIsTrigger && !lPresenceValue)
        return;
    // and we ignore OFF telegrams if we are not in presence state
    if ((pCurrentState & STATE_PRESENCE) == 0 && !lPresenceValue)
        return;
    // and we ignore OFF telegrams if delay time is already started
    if (pPresenceDelayTime > 0 && !lPresenceValue)
        return;
    if (lAllowStartPresence)
        startPresence(false, false);
    if (iIsTrigger && lPresenceValue)
    {
        // triggered input sent a 1, we immediately set it to 0
        iKo->value(false, DPT_Bool);
        // afterwards we call ourself to evaluate 0 action
        if (lAllowStartPresence)
            startPresence(false, false);
    }
}

// main entry point for presence calculation (KO independent), implemented as switching presence channel
void PresenceChannel::startPresence(bool iForce, bool iManual)
{
    // during leave room we ignore any presence processing
    if (isLeaveRoom())
        return;

    // this method handles both presence inputs. We handle both combined by an OR.
    if (iForce || getRawPresence())
    {
        // do according actions if presence changes
        if (!(pCurrentState & STATE_PRESENCE))
        {
            // the following is just allowed for full automatic on (Vollautomat)
            // or if manual on is requested an manual on is done.
            uint8_t lDayPhaseFunction = paramByte(PM_pADayPhaseFunction, PM_pADayPhaseFunctionMask, PM_pADayPhaseFunctionShift, true);
            bool lTurnOn = (lDayPhaseFunction == VAL_PM_PHASE_FULL || lDayPhaseFunction == VAL_PM_PHASE_HALF_OFF);
            if (!lTurnOn) lTurnOn = (lDayPhaseFunction == VAL_PM_PHASE_HALF_ON && iManual);
            if (lTurnOn)
            {
                if (!iManual) startPresenceShort();
                onPresenceBrightnessChange(true);
                // presence is turned on, we set the state and delete delay timer
                pCurrentState |= STATE_PRESENCE;
            }
        }
        else if (pCurrentState & STATE_PRESENCE_SHORT)
        {
            processPresenceShort();
        }
        pPresenceDelayTime = 0; // there is no action because of this, the delay time is just "off"
    }
    else if (pCurrentState & STATE_PRESENCE)
    {
        // presence is turned off, we start delay timer, but just if we are in presence mode
        // Why check for presence state here? Because multiple off would restart presence delay even if there is no presence
        pPresenceDelayTime = delayTimerInit();
    }
}

void PresenceChannel::processPresence()
{
    // we just do something if timer is really running
    if (pPresenceDelayTime > 0)
    {
        uint32_t lPresenceDelay = (uint32_t)getKo(PM_KoKOpPresenceDelay)->value(DPT_Value_2_Ucount) * 1000;
        if (delayCheck(pPresenceDelayTime, lPresenceDelay))
        {
            // time is over, we turn everything off
            endPresence();
        }
    }
}

void PresenceChannel::endPresence(bool iSend /* = true */)
{
    // presence and short presence determination are stopped
    endPresenceShort();
    pCurrentState &= ~(STATE_PRESENCE | STATE_AUTO);
    pCurrentValue &= ~PM_BIT_DISABLE_BRIGHTNESS;
    pPresenceDelayTime = 0;
    if (iSend) onPresenceChange(false);
}

void PresenceChannel::startPresenceShort()
{
    // during leave room we ignore any presence processing
    if (isLeaveRoom())
        return;

    if (paramBit(PM_pAPresenceShort, PM_pAPresenceShortMask, true))
    {
        // set the mark for short presence mode
        pCurrentState |= STATE_PRESENCE_SHORT;
        pPresenceShortDelayTime = delayTimerInit();
    }
}

void PresenceChannel::processPresenceShort()
{
    // short presence is measured immediately
    if (delayCheck(pPresenceShortDelayTime, paramTimeDelay(PM_pAPresenceShortDurationBase, true)))
    {
        // now short presence duration passed, check of also short presence delay passed
        uint32_t lEndTime = paramTimeDelay(PM_pAPresenceShortDurationBase, true) + paramTimeDelay(PM_pAPresenceShortDelayBase, true);
        if (delayCheck(pPresenceShortDelayTime, lEndTime + 300))
        {
            // there was no new presence evaluation during short presence delay, so we know, short presence is valid here
            endPresence();
        }
        else
        {
            // if we get any further presence information, we stop short presence without ending normal presence
            // in addition, we check if move or move+presence is used for presence evaluation
            bool lUsePresence = paramBit(PM_pAPresenceShortCalculation, PM_pAPresenceShortCalculationMask, true);
            bool lPresence = getRawPresence(!lUsePresence);
            if (lPresence)
            {
                endPresenceShort();
                // in case of passageway we turn on ouput after short presence
                bool lPassageway = paramBit(PM_pAPresenceShortNoSwitch, PM_pAPresenceShortNoSwitchMask, true);
                if (lPassageway)
                    onPresenceBrightnessChange(true);
            }
        }
    }
}

void PresenceChannel::endPresenceShort()
{
    // end short presence without ending presence
    pCurrentState &= ~STATE_PRESENCE_SHORT;
    pPresenceShortDelayTime = 0;
}

void PresenceChannel::onPresenceBrightnessChange(bool iOn)
{
    // during leave room we ignore any presence processing
    if (isLeaveRoom())
        return;

    // calculate output dependent on brightness and auto mode
    if (iOn && !ParamPM_pBrightnessIndependent)
    {
        float lBrightness = getRawBrightness();
        if (lBrightness != NO_NUM)
        {
            // check brightness in case of turning on
            if ((uint32_t)lBrightness < (uint32_t)getKo(PM_KoKOpLuxOn)->value(DPT_Value_Lux))
                onPresenceChange(iOn);
        }
    }
    else // turn off if brightness independent
        onPresenceChange(iOn);
}

void PresenceChannel::onPresenceChange(bool iOn)
{

    // Output is only influenced in auto mode
    if (!(pCurrentState & STATE_MANUAL))
    {
        // We turn on or off, but only if automatic off is not forbidden
        uint8_t lDayPhaseFunction = paramByte(PM_pADayPhaseFunction, PM_pADayPhaseFunctionMask, PM_pADayPhaseFunctionShift, true);
        bool lTurnOff = (lDayPhaseFunction == VAL_PM_PHASE_FULL || lDayPhaseFunction == VAL_PM_PHASE_HALF_ON);
        lTurnOff = lTurnOff || iOn; // turn on is always allowed
        if (lTurnOff)
        {
            bool lPassageway = paramBit(PM_pAPresenceShortNoSwitch, PM_pAPresenceShortNoSwitchMask, true);
            // in a passageway we do not turn on during short presence
            if (!(iOn && lPassageway && (pCurrentState & STATE_PRESENCE_SHORT)))
                startOutput(iOn);
        }
    }
}

void PresenceChannel::startLeaveRoom(bool iSuppressOutput)
{
    pLeaveRoomMode = ParamPM_pLeaveRoomModeAll;
    bool lOn = pCurrentValue & PM_BIT_OUTPUT_SET;
    // we have to send an OFF signal if requested or current output value is on
    bool lSend = !iSuppressOutput || lOn;
    bool lIsLeaveRoom = false;
    switch (pLeaveRoomMode)
    {
        case PT_LeaveRoomMode::Totzeit:
        case PT_LeaveRoomMode::Totzeit_Reset:
            // in this case we just wait until downtime passed and afterwards we wait for the first Move
            startDowntime(); // has to be first to set correct state
            lIsLeaveRoom = true;
            onManualChange(false);
            endPresence(lSend);
            break;
        case PT_LeaveRoomMode::Bewegung_Totzeit:
        case PT_LeaveRoomMode::Bewegung_Totzeit_Reset:
            // dispatch to process handler
            pCurrentState |= STATE_LEAVE_ROOM;
            lIsLeaveRoom = true;
            break;

        default:
            // if there is no leave room configured, we process auto off
            startAuto(false, iSuppressOutput);
            break;
    }
    if (lIsLeaveRoom)
    {
        // we need to reset manual mode
        onManualChange(false);
        // and end presence
        endPresence(!iSuppressOutput);
        if (iSuppressOutput)
        {
            // if we suppress output, we need to align internal output state
            pCurrentValue &= ~PM_BIT_OUTPUT_SET;
            syncOutput();
        }
    }
}

void PresenceChannel::processLeaveRoom()
{
    switch (pLeaveRoomMode)
    {
        case PT_LeaveRoomMode::Totzeit:
            // we wait for the next move, which also starts new presence cycle
            if (getRawPresence(true))
            {
                endLeaveRoom();
                startPresence(false, false);
            }
            // if somewhen presence vanishes, we also continue in normal mode
            if (!getRawPresence())
                endLeaveRoom();
            break;
        case PT_LeaveRoomMode::Totzeit_Reset:
        {
            // we send a reset to external PM and go to normal mode
            uint8_t lResetTrigger = ParamPM_pExternalSupportsReset;
            if (lResetTrigger)
                getKo(PM_KoKOpResetExternalPM)->value((lResetTrigger == 1), DPT_Bool);
            // TODO: reset internal PIR, as soon as implemented
            endLeaveRoom();
        }
        break;
        case PT_LeaveRoomMode::Bewegung_Totzeit:
        case PT_LeaveRoomMode::Bewegung_Totzeit_Reset:
            // we wait until current move gets inactive
            if (!getRawPresence(true))
            {
                // from now on it is the same like downtime processing
                pLeaveRoomMode = (pLeaveRoomMode == PT_LeaveRoomMode::Bewegung_Totzeit) ? PT_LeaveRoomMode::Totzeit : PT_LeaveRoomMode::Totzeit_Reset;
                pCurrentState &= ~STATE_LEAVE_ROOM;
                startDowntime();
            }
            break;

        default:
            break;
    }
}

void PresenceChannel::endLeaveRoom()
{
    pCurrentState &= ~(STATE_DOWNTIME | STATE_LEAVE_ROOM);
    pDowntimeDelayTime = 0;
}

bool PresenceChannel::isLeaveRoom()
{
    return (pCurrentState & (STATE_DOWNTIME | STATE_LEAVE_ROOM));
}

// downtime during leave room
void PresenceChannel::startDowntime()
{
    // set state
    pCurrentState |= STATE_DOWNTIME;
    // set downtime timer
    pDowntimeDelayTime = delayTimerInit();
}

void PresenceChannel::processDowntime()
{
    if (pDowntimeDelayTime > 0 && delayCheck(pDowntimeDelayTime, paramTimeDelay(PM_pDowntimeOffBase, false)))
    {
        pDowntimeDelayTime = 0;
        // we end downtime handling
        pCurrentState &= ~STATE_DOWNTIME;
        // and wait for next move
        pCurrentState |= STATE_LEAVE_ROOM;
    }
}

void PresenceChannel::startAutoPrepare()
{
    pCurrentState |= STATE_KO_SET_AUTO;
}

void PresenceChannel::processAutoPrepare()
{
    // Automatic mode
    bool lValue = getKo(PM_KoKOpSetAuto)->value(DPT_Bool);
    pCurrentState &= ~STATE_KO_SET_AUTO;
    startAuto(lValue, false);
}

void PresenceChannel::startAuto(bool iOn, bool iSuppressOutput)
{
    // end any leave room processing
    endLeaveRoom();
    // ensure auto mode
    onManualChange(false);
    // check if we have to go to leave room
    bool lAutoOffIsLeave = ParamPM_pAutoOffIsLeave;
    // here we have to use a local variable, not pLeaveRoomMode!
    PT_LeaveRoomMode lLeaveRoomMode = ParamPM_pLeaveRoomModeAll;
    if (!iOn && lAutoOffIsLeave && lLeaveRoomMode > PT_LeaveRoomMode::Raum_verlassen_inaktiv)
        startLeaveRoom(iSuppressOutput);
    else
    {
        // sollte Auto EIN/AUS auch adaptive Berechnung starten?

        // we start presence delay
        startPresenceTrigger(true);
        // and we also start/reset short presence here
        // new: for manual usage we don't want short presence
        // startPresenceShort();
        // disable brightness handling according to current brightness
        disableBrightness(iOn);
        // set state
        pCurrentState |= STATE_AUTO;
        // set output according to input value
        startOutput(iOn);
        if (iSuppressOutput)
            syncOutput();
        else
            forceOutput((ParamPM_pOutput1SendAdditional & VAL_PM_Force_ActorState) || (ParamPM_pOutput2SendAdditional & VAL_PM_Force_ActorState), VAL_PM_Force_ActorState);
    }
}

void PresenceChannel::processAuto()
{
}

void PresenceChannel::startManualPrepare()
{
    pCurrentState |= STATE_KO_SET_MANUAL;
}

void PresenceChannel::processManualPrepare()
{
    // Manual mode
    bool lValue = getKo(PM_KoKOpSetManual)->value(DPT_Bool);
    pCurrentState &= ~STATE_KO_SET_MANUAL;
    // check for two button mode
    if (paramBit(PM_pManualModeKeyCount, PM_pManualModeKeyCountMask))
        startManual(lValue, false);
    else // use single button mode
        if (lValue)
            startManual(pCurrentValue & PM_BIT_OUTPUT_SET, true);
        else
            startAuto(pCurrentValue & PM_BIT_OUTPUT_SET, true);
}

void PresenceChannel::startManual(bool iOn, bool iSuppressOutput)
{
    // end any leave room processing
    endLeaveRoom();
    // ensure manual mode
    onManualChange(true);
    // we stop presence delay
    endPresence();
    // set output according to input value
    startOutput(iOn);
    if (iSuppressOutput)
        syncOutput();
    else
        forceOutput((ParamPM_pOutput1SendAdditional & VAL_PM_Force_ActorState) || (ParamPM_pOutput2SendAdditional & VAL_PM_Force_ActorState), VAL_PM_Force_ActorState);
}

void PresenceChannel::processManual()
{
    // we just do something if timer is really running
    if (pManualFallbackTime > 0)
    {
        // in case manual mode is presence dependent, any presence resets manual fallback time
        if (paramBit(PM_pAManualWithPresence, PM_pAManualWithPresenceMask, true) && getRawPresence())
        {
            pManualFallbackTime = delayTimerInit();
        }
        else if (delayCheck(pManualFallbackTime, paramTimeDelay(PM_pAManualFallbackDelayBase, true)))
        {
            // time is over, we turn everything off
            onManualChange(false);
            onPresenceChange(false);
        }
    }
}

void PresenceChannel::onManualChange(bool iOn)
{
    if (iOn)
    {
        pCurrentState |= STATE_MANUAL; // we set manual state
        pCurrentState &= ~STATE_AUTO;  // and end auto state
        if (paramTimeDelay(PM_pAManualFallbackDelayBase, true) > 0)
            pManualFallbackTime = delayTimerInit();
    }
    else
    {
        pCurrentState &= ~STATE_MANUAL;
        pManualFallbackTime = 0;
    }
    // set StateKO
    if ((bool)(getKo(PM_KoKOpIsManual)->value(DPT_Bool)) != iOn)
        getKo(PM_KoKOpIsManual)->value(iOn, DPT_Bool);
}

void PresenceChannel::startLock()
{
    pCurrentState |= STATE_KO_LOCK;
}

void PresenceChannel::processLockPrepare()
{
    pCurrentState &= ~STATE_KO_LOCK;
    // check lock mode (simple lock or priority control)
    uint8_t lLockType = ParamPM_pLockType;
    if (lLockType == VAL_PM_LockTypePriority)
    {
        uint8_t lPriority = getKo(PM_KoKOpLock)->value(DPT_Value_1_Ucount);
        bool lValue = (lPriority & 2);
        PT_PMLock lLockSend = (PT_PMLock)((lPriority & 1) + 1);
        onLock(lValue, lLockSend, lLockSend);
    }
    else if (lLockType == VAL_PM_LockTypeLock)
    {
        // simple lock
        bool lValue = getKo(PM_KoKOpLock)->value(DPT_Bool);
        bool lInvert = ParamPM_pLockActive;
        if (lInvert)
            lValue = !lValue;
        PT_PMLock lLockOnSend = ParamPM_pLockOn;
        PT_PMLock lLockOffSend = ParamPM_pLockOff;
        onLock(lValue, lLockOnSend, lLockOffSend);
    }
}

void PresenceChannel::processLock()
{
    // check if there is a time given to reset lock
    if ((pCurrentState & STATE_LOCK) && ParamPM_pLockFallback)
    {
        if (pLockDelayTime > 0 && delayCheck(pLockDelayTime, ParamPM_pLockFallbackTimeMS))
        {
            // end lock
            pLockDelayTime = 0;
            uint8_t lLockType = ParamPM_pLockType;
            if (lLockType == VAL_PM_LockTypePriority)
            {
                // priority
                onLock(false, PT_PMLock::AUS_gesendet, PT_PMLock::AUS_gesendet);
            }
            else if (lLockType == VAL_PM_LockTypeLock)
            {
                // simple lock
                PT_PMLock lLockOffSend = ParamPM_pLockOff;
                onLock(false, lLockOffSend, lLockOffSend);
            }
        }
    }
}

void PresenceChannel::onLock(bool iLockOn, PT_PMLock iLockOnSend, PT_PMLock iLockOffSend)
{

    if (iLockOn)
    {
        // should we send something?
        if (iLockOnSend > PT_PMLock::nichts_gesendet)
        {
            startOutput(iLockOnSend == PT_PMLock::EIN_gesendet);
            forceOutput(true);
        }
        pCurrentState |= STATE_LOCK;
        pLockDelayTime = delayTimerInit();
    }
    else if (pCurrentState & STATE_LOCK)
    {
        pCurrentState &= ~STATE_LOCK;
        pLockDelayTime = 0;
        uint32_t lPresenceDelayTime;
        switch (iLockOffSend)
        {
            case PT_PMLock::nichts_gesendet:
                // nothing should be send, so we set the output state to current state
                syncOutput();
                break;
            case PT_PMLock::AUS_gesendet:
                startAuto(false, false);
                forceOutput(true);
                break;
            case PT_PMLock::EIN_gesendet:
                startAuto(true, false);
                forceOutput(true);
                break;
            case PT_PMLock::Aktueller_Zustand_gesendet:
                // we send current state by reevaluation
                lPresenceDelayTime = pPresenceDelayTime;
                startPresence(false, false);
                pPresenceDelayTime = lPresenceDelayTime;
                forceOutput(true);
                break;
            default:
                break;
        }
    }
    // we have to sync KO according to lock state
    uint8_t lLockType = ParamPM_pLockType;
    uint8_t lLockValue = (iLockOn << 1);
    PT_PMLock lLockSend = lLockValue ? iLockOnSend : iLockOffSend;
    Dpt lDpt = DPT_Value_Temp;
    switch (lLockType)
    {
        case VAL_PM_LockTypePriority:
            if (lLockSend == PT_PMLock::nichts_gesendet || lLockSend == PT_PMLock::Aktueller_Zustand_gesendet)
                lLockValue |= ((pCurrentValue & PM_BIT_OUTPUT_SET) > 0);
            else
                lLockValue |= (lLockSend == PT_PMLock::EIN_gesendet);
            lDpt = DPT_Value_1_Ucount;
            break;
        case VAL_PM_LockTypeLock:
            lLockValue = iLockOn;
            lDpt = DPT_Bool;
            break;
        default:
            // do nothing
            break;
    }
    if (lDpt != DPT_Value_Temp)
    {
        // send new state only if lock state changed
        if (lLockValue != pLastLockState)
        {
            getKo(PM_KoKOpLock)->value(lLockValue, lDpt);
            pLastLockState = lLockValue;
        }
    }
}

void PresenceChannel::startReset()
{
    pCurrentState |= STATE_KO_RESET;
}

void PresenceChannel::processReset()
{
    pCurrentState &= ~STATE_KO_RESET;
    // reset PM, we just react on ON-telegrams
    bool lValue = getKo(PM_KoKOpReset)->value(DPT_Bool);
    if (lValue)
        onReset();
}

void PresenceChannel::onReset()
{
    // We reset PM so that next (or an existing) presence signal immediately turns output on
    endLeaveRoom();
    onManualChange(false);
    endPresence(true);

    // Unclear: how to handle lock?
    // Unclear: how to handle slave PMs
    // Unclear: do we restore here also day phase settings?

    // finally we look if current presence signal is there
    startPresence(false, false);
    forceOutput(true);
}

void PresenceChannel::startActorState()
{
    pCurrentState |= STATE_KO_SET_ACTOR_STATE;
}

void PresenceChannel::processActorState()
{
    // change of actor state always influences the PM behaviour
    // if the actor state is different to current PM state
    GroupObject *lKo = getKo(PM_KoKOpAktorState);
    bool lValue = lKo->value(DPT_Bool);

    pCurrentState &= ~STATE_KO_SET_ACTOR_STATE;

    // we leave immediately, if actor state and PM state are equal
    if (lKo->initialized() && ((bool)(pCurrentValue & PM_BIT_OUTPUT_SET)) == lValue)
        return;

    // in case of actor change we behave like Auto-On (light should go out after delay time)
    if (lValue)
        startAuto(lValue, true);
    else if (ParamPM_pActorState == VAL_PM_AS_None)
    {
        startLeaveRoom(true);

        // but we do not send anything to the knx bus
        // syncOutput();
        // but we do not enter Auto state
        pCurrentState &= ~STATE_AUTO;
        // and we do not disable brightness handling
        pCurrentValue &= ~PM_BIT_DISABLE_BRIGHTNESS;
    }
    else if (ParamPM_pActorState == VAL_PM_AS_AutoOff)
    {
        // if actor state is manual, we start manual mode
        startAuto(false, true);
    }
    else if (ParamPM_pActorState == VAL_PM_AS_LeaveRoom)
    {
        // if actor state is auto, we start auto mode
        startLeaveRoom(true);
    }
}

void PresenceChannel::startBrightnessOff()
{
    pCurrentState |= STATE_KO_LUX_ON;
}

void PresenceChannel::processBrightnessOff()
{
    // calculate brightness to turn off light
    // we have to differ between absolute and adaptive turn off light
    uint8_t lLuxAutoOff = paramByte(PM_pABrightnessAuto, PM_pABrightnessAutoMask, PM_pABrightnessAutoShift, true);
    uint32_t lBrightness;
    pCurrentState &= ~STATE_KO_LUX_ON;
    switch (lLuxAutoOff)
    {
        case VAL_PM_LuxAdaptiveOff:
            // for adaptive off the calculation is async
            startAdaptiveBrightness();
            break;
        case VAL_PM_LuxAbsoluteOff:
            // for absolute off we simply add the offset to current on limit
            lBrightness = getKo(PM_KoKOpLuxOn)->value(DPT_Value_Lux);
            lBrightness += paramWord(PM_pABrightnessDelta, true);
            getKo(PM_KoKOpLuxOff)->value(lBrightness, DPT_Value_Lux);
            break;
        default:
            // do nothing
            break;
    }
}

void PresenceChannel::startBrightnessPrepare()
{
    pCurrentState |= STATE_KO_LUX;
}

void PresenceChannel::processBrightnessPrepare()
{
    pCurrentState &= ~STATE_KO_LUX;
    startBrightness();
}

void PresenceChannel::startBrightness()
{
    // during leave room we ignore any brightness processing
    if (isLeaveRoom())
        return;

    // should we suppress brightness evaluation?
    bool lEvalBrightness = !(pCurrentValue & PM_BIT_DISABLE_BRIGHTNESS);
    // or are we working brightness independent?
    lEvalBrightness = lEvalBrightness && (!ParamPM_pBrightnessIndependent);
    // or are we in manual mode?
    lEvalBrightness = lEvalBrightness && ((pCurrentState & (STATE_MANUAL | STATE_LOCK)) == 0);
    // first check for upper value, if higher, then switch off
    float lBrightness = getRawBrightness();
    if (lEvalBrightness && lBrightness != NO_NUM)
    {
        // but only, if we are not calculating a new off value
        // and only, if the switch "manual actions suppress brightness off" is not set
        if (!(pCurrentState & STATE_ADAPTIVE) && lBrightness >= (float)getKo(PM_KoKOpLuxOff)->value(DPT_Value_Lux) && (!((pCurrentState & STATE_AUTO) && paramBit(PM_pABrightnessSuppress, PM_pABrightnessSuppressMask, true))))
        {
            // we start timer off delay
            if (pBrightnessOffDelayTime == 0 && paramByte(PM_pABrightnessAuto, PM_pABrightnessAutoMask, PM_pABrightnessAutoShift, true) > 0)
                pBrightnessOffDelayTime = delayTimerInit();
        }
        else
        {
            // deactivate brightness off
            pBrightnessOffDelayTime = 0;
        }
        // now check lower value, if below, turn light on
        if ((float)lBrightness < (float)getKo(PM_KoKOpLuxOn)->value(DPT_Value_Lux))
        {
            // its getting dark, if we are in presence state, we turn light on
            // except we are in auto state (technically here it is Auto-Off), we should not turn on
            if ((pCurrentState & STATE_PRESENCE) && (pCurrentState & STATE_AUTO) == 0)
                onPresenceChange(true);
        }
    }
}

void PresenceChannel::processBrightness()
{
    if (pBrightnessOffDelayTime > 0)
    {
        if (delayCheck(pBrightnessOffDelayTime, paramTimeDelay(PM_pABrightnessOffDelayBase, true)))
        {
            pBrightnessOffDelayTime = 0;
            // we have to turn off light but not presence
            onPresenceChange(false);
            // if brightness handling is allowed, we have to disable auto mode
            pCurrentState &= ~STATE_AUTO;
        }
    }
}

void PresenceChannel::disableBrightness(bool iOn)
{
    // are we brightness dependant
    if (!ParamPM_pBrightnessIndependent)
    {
        // get current brightness
        float lBrightness = getRawBrightness();
        if (lBrightness != NO_NUM)
        {
            // we disable brightness handling according to current brightness and current output state
            // turn on even though there is enough light
            bool lDisable1 = iOn && (lBrightness >= (float)getKo(PM_KoKOpLuxOff)->value(DPT_Value_Lux));
            // turn off even though it is too dark
            bool lDisable2 = !iOn && (lBrightness <= (float)getKo(PM_KoKOpLuxOff)->value(DPT_Value_Lux));
            if (lDisable1 || lDisable2) 
            {
                pCurrentValue |= PM_BIT_DISABLE_BRIGHTNESS;
                // deactivate any pending brightness off
                pBrightnessOffDelayTime = 0;
            }
            else
                pCurrentValue &= ~PM_BIT_DISABLE_BRIGHTNESS;
        }
    }
}

void PresenceChannel::startAdaptiveBrightness()
{
    // Adaptive brightness calculation is done each time the intended light situation changes
    // This usually happens by turning other lights on or off or dimming them
    if (paramByte(PM_pABrightnessAuto, PM_pABrightnessAutoMask, PM_pABrightnessAutoShift, true) == VAL_PM_LuxAdaptiveOff)
    {
        pCurrentState |= STATE_ADAPTIVE;
        pCurrentState &= ~STATE_ADAPTIVE_READ;
        pAdaptiveDelayTime = delayTimerInit();
        // if we have to calculate a new brightness off value, we should prevent an off due to new light value
        pBrightnessOffDelayTime = 0;
    }
}

void PresenceChannel::processAdaptiveBrightness()
{
    // should we calculate adaptive brightness
    if (pCurrentState & STATE_ADAPTIVE)
    {
        if (!(pCurrentState & STATE_ADAPTIVE_READ) && pAdaptiveDelayTime > 0 && delayCheck(pAdaptiveDelayTime, paramTimeDelay(PM_pAdaptiveDelayBase, false)))
        {
            // we waited the whole adaptive delay time for an according brightness update
            pAdaptiveDelayTime = 0;
            // should we send now a read request?
            if (paramBit(PM_pBrightnessRead, PM_pBrightnessReadMask, false))
            {
                // send a read request
                getKo(PM_KoKOpLux)->requestObjectRead();
                // and wait for an other second
                pCurrentState |= STATE_ADAPTIVE_READ;
                pAdaptiveDelayTime = delayTimerInit();
            }
        }
        else if (pCurrentState & STATE_ADAPTIVE_READ && delayCheck(pAdaptiveDelayTime, 1500))
        {
            // we waited one second for the read and continue processing with current lux value
            pCurrentState &= ~STATE_ADAPTIVE_READ;
            pAdaptiveDelayTime = 0;
        }
        else if (pAdaptiveDelayTime == 0)
        {
            // take current lux value, but not less that turn on limit
            uint32_t lBrightnessOff = MAX((uint32_t)getRawBrightness(), (uint32_t)(paramWord(PM_pABrightnessOn, true)));
            // add adaptive offset and set it as new off limit
            lBrightnessOff += paramWord(PM_pABrightnessDelta, true);
            getKo(PM_KoKOpLuxOff)->value(lBrightnessOff, DPT_Value_Lux);
            // and we stop adaptive calculation
            pCurrentState &= ~(STATE_ADAPTIVE | STATE_ADAPTIVE_READ);
        }
    }
}

void PresenceChannel::startOutput(bool iOn)
{
    if (iOn)
        pCurrentValue |= PM_BIT_OUTPUT_SET;
    else
        pCurrentValue &= ~PM_BIT_OUTPUT_SET;
}

void PresenceChannel::forceOutput(bool iOn, uint32_t iForceFilter)
{
    if (iOn) 
        pCurrentValue |= (PM_BIT_OUTPUT_FORCE | (iForceFilter << PM_VAL_OUTPUT_FORCE_SHIFT));
    else
        pCurrentValue &= ~(PM_BIT_OUTPUT_FORCE | PM_VAL_OUTPUT_FORCE_MASK);
}

void PresenceChannel::syncOutput()
{
    bool lOn = pCurrentValue & PM_BIT_OUTPUT_SET;
    if (lOn)
        pCurrentValue |= PM_BIT_OUTPUT_WRITTEN;
    else
        pCurrentValue &= ~PM_BIT_OUTPUT_WRITTEN;
}

void PresenceChannel::processOutput()
{
    uint8_t lOutput = 0;
    if (!(pCurrentState & STATE_LOCK))
    {
        if (pCurrentValue & PM_BIT_OUTPUT_FORCE)
        {
            // eval according force filter
            uint8_t lFilter = (pCurrentValue & PM_VAL_OUTPUT_FORCE_MASK) >> PM_VAL_OUTPUT_FORCE_SHIFT;
            if (lFilter == VAL_PM_Force_Both || lFilter == VAL_PM_Force_None)
                lOutput = 3;
            else { 
                if (lFilter & ParamPM_pOutput1SendAdditional) lOutput = 1;
                if (lFilter & ParamPM_pOutput2SendAdditional) lOutput |= 2;
            }
        } else {
            // check for send because of output state change
            uint8_t lValue = pCurrentValue & (PM_BIT_OUTPUT_SET | PM_BIT_OUTPUT_WRITTEN);
            if (lValue > 0 && lValue < (PM_BIT_OUTPUT_SET | PM_BIT_OUTPUT_WRITTEN))
                lOutput = 3;
        }
    }
    if (lOutput)
    {
        // we have to send something
        bool lOn = (pCurrentValue & PM_BIT_OUTPUT_SET);
        if (lOutput & 1)
        {
            onOutput(VAL_PM_Output1Index, lOn);
            // pOutput1CyclicTime = delayTimerInit();
        }
        if (lOutput & 2)
        {
            onOutput(VAL_PM_Output2Index, lOn);
            // pOutput2CyclicTime = delayTimerInit();
        }
        syncOutput();
        forceOutput(false);
    }
}

void PresenceChannel::onOutput(bool iOutputIndex, bool iOn)
{
    static Dpt sTypeToDpt[5] = {DPT_Value_TemperatureDifference, DPT_Bool, DPT_Value_1_Ucount, DPT_SceneNumber, DPT_Scaling};

    // first check, if output is active
    // get output DPT
    PT_OutputType lType = iOutputIndex ? ParamPM_pOutput2Type : ParamPM_pOutput1Type;
    if (lType == PT_OutputType::Inaktiv)
        return;
    uint8_t lFilter = iOutputIndex ? paramByte(PM_pAOutput2Filter, PM_pAOutput2FilterMask, PM_pAOutput2FilterShift, true) : paramByte(PM_pAOutput1Filter, PM_pAOutput1FilterMask, PM_pAOutput1FilterShift, true);
    // get correct value to send
    uint8_t lValue;
    bool lSend = false;
    if (iOn && (lFilter & 1))
    {
        // send on value if allowed
        lValue = iOutputIndex ? paramByte(PM_pAOutput2On, true) : paramByte(PM_pAOutput1On, true);
        lSend = true;
    }
    else if (!iOn && (lFilter & 2))
    {
        // send off value if allowed
        lValue = iOutputIndex ? paramByte(PM_pAOutput2Off, true) : paramByte(PM_pAOutput1Off, true);
        lSend = true;
    }
    if (lSend) 
    {
        if (sTypeToDpt[(uint8_t)lType] == DPT_SceneNumber) lValue--;
        GroupObject* lKo = getKo(iOutputIndex ? PM_KoKOpOutput2 : PM_KoKOpOutput);
        lKo->value(lValue, sTypeToDpt[(uint8_t)lType]);
        // seems to have side effects, currently disabled
        // // startup fix: the very fist Telegram should not be sent, because it turns light on or off after startup, which is not intended
        // if (lKo->initialized())
        // {
        //     lKo->value(lValue, getDPT(sTypeToDpt[lType]));
        // }
        // else
        // {
        //     // we do not send the telegram, but we set the value
        //     lKo->valueNoSend(lValue, getDPT(sTypeToDpt[lType]));
        // }
    }
}

void PresenceChannel::loop()
{
    if (!knx.configured())
        return;

    if (ParamPM_pChannelActive != PT_ChannelActive::Aktiv)
        return;

    // here we do the things after setup, but only once in the loop()
    if ((pCurrentState & (STATE_STARTUP | STATE_RUNNING | STATE_READ_REQUESTS)) == 0)
    {
        // currently nothing else to do
        // last but not least...
        startStartupDelay();
    }

    if (pCurrentState & STATE_STARTUP)
        processStartupDelay();

    // do no further processing until channel passed its startup time
    if (pCurrentState & (STATE_RUNNING | STATE_READ_REQUESTS))
    {
        processReadRequests();
        // handle presence hardware
        startHardwarePresence();
        // handle brightness hardware
        startHardwareBrightness();

        // we revert the processing order for pipeline events
        // this reduces the chance to have a long running
        // sequence of functions because of according pipeline settings
        // On/Off repeat pipeline
        if (pCurrentState & (STATE_PRESENCE))
            processPresence();
        if (pCurrentState & (STATE_PRESENCE_SHORT))
            processPresenceShort();
        if (pCurrentState & (STATE_MANUAL))
            processManual();
        if (pCurrentState & (STATE_DAY_PHASE_CHANGE))
            processDayPhase();
        if (pCurrentState & (STATE_ADAPTIVE))
            processAdaptiveBrightness();
        if (pCurrentState & (STATE_LEAVE_ROOM))
            processLeaveRoom();
        if (pCurrentState & (STATE_DOWNTIME))
            processDowntime();
        if (pCurrentState & (STATE_LOCK))
            processLock();

        // we process now ko states to decouple processing from KO callbacks
        if (pCurrentState & (STATE_KO_LUX_ON))
            processBrightnessOff();
        if (pCurrentState & (STATE_KO_LUX))
            processBrightnessPrepare();
        if (pCurrentState & (STATE_KO_PRESENCE1))
            processPresencePrepare(STATE_KO_PRESENCE1);
        if (pCurrentState & (STATE_KO_PRESENCE2))
            processPresencePrepare(STATE_KO_PRESENCE2);
        if (pCurrentState & (STATE_KO_SET_AUTO))
            processAutoPrepare();
        if (pCurrentState & (STATE_KO_SET_MANUAL))
            processManualPrepare();
        if (pCurrentState & (STATE_KO_SET_ACTOR_STATE))
            processActorState();
        if (pCurrentState & (STATE_KO_LOCK))
            processLockPrepare();
        if (pCurrentState & (STATE_KO_RESET))
            processReset();

        if (pCurrentState & (STATE_KO_DAY_PHASE))
            processDayPhasePrepare();
        if (pCurrentState & (STATE_KO_SCENE))
            processSceneCommand();
    }
    if (pCurrentState & STATE_RUNNING)
    {
        // brightness is always evaluated
        processBrightness();
        // output is always evaluated
        processOutput();
    }
}

void PresenceChannel::prepareInternalKo()
{
    // we know the indexes of internal KO
    for (uint8_t lIndex = PM_pNumLux; lIndex <= PM_pNumScene; lIndex = lIndex + 2)
    {
        uint16_t lInternalKo = paramWord(lIndex);
        if (lInternalKo & 0x8000)
        {
            uint8_t lKoIndex = (lIndex < PM_pNumScene) ? (lIndex - PM_pNumLux) / 2 + PM_KoKOpLux : PM_KoKOpScene;
            openknxPresenceModule.addKoMap(lInternalKo & 0x7FFF, channelIndex(), lKoIndex);
        }
    }
}

void PresenceChannel::setup()
{
    // Skip setup if Channel is not active
    if (ParamPM_pChannelActive != PT_ChannelActive::Aktiv)
        return;

    prepareInternalKo();
    onDayPhase(0, true);

    // init output
    startOutput(false);
    forceOutput(false);
    syncOutput();
}

const std::string PresenceChannel::name()
{
    return "VPM";
}