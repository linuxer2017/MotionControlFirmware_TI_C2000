/******************************************************************************
 * Copyright (C) 2017 by Yifan Jiang                                          *
 * jiangyi@student.ethz.com                                                   *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 ******************************************************************************/

/*
* implement the current controller class
*/

#include "CurrentLoopController.h"
#include "PwmModulation.h"

CurrentLoopController::CurrentLoopController(ControlProcessData * ControlProcessDataPtr):
  _Kp(100.5),
  _Ki(0),
  _Ki_dt(0),
  _Error_Ia(0),
  _Error_Ib(0),
  _Integral_Ia(0),
  _Integral_Ib(0),
  _OutputOffset(0)
{
  _ControlProcessData = ControlProcessDataPtr;
  _OutputVoltage.A = 0;
  _OutputVoltage.B = 0;
  _OutputVoltage.C = 0;
}

/**
 *  execute the current controller
 */
#pragma CODE_SECTION(".TI.ramfunc");
PwmDutyVec CurrentLoopController::Execute(PhaseCurrentVec * CurrentDemand, PhaseCurrentVec * CurrentActual){

  PwmDutyVec Pwm;

  float32_t VoltageDcLine = _ControlProcessData->_VoltageValueDcLine;
  float32_t VoltToPwmScaleFactor = PWM_MAX_DUTY / VoltageDcLine;
  float32_t OutputVoltageLimit = VoltageDcLine * PWM_MAX_PERCENTAGE * 0.57f;
  //float32_t OutputVoltageMinimum = VoltageDcLine * PWM_MIN_PERCENTAGE;

  // execute PI controller
  _Error_Ia = CurrentDemand->A - CurrentActual->A;
  _Error_Ib = CurrentDemand->B - CurrentActual->B;
  _Integral_Ia += _Ki_dt * _Error_Ia;
  _Integral_Ib += _Ki_dt * _Error_Ib;
  _OutputVoltage.A = _Kp * _Error_Ia + _Integral_Ia;
  _OutputVoltage.B = _Kp * _Error_Ib + _Integral_Ib;

  // anti-windup and output voltage limiting, phase A
  if(_OutputVoltage.A > OutputVoltageLimit){
    _Integral_Ia -= _OutputVoltage.A - OutputVoltageLimit;
    _OutputVoltage.A = OutputVoltageLimit;
  } else if(_OutputVoltage.A < -OutputVoltageLimit) {
    _Integral_Ia -= _OutputVoltage.A + OutputVoltageLimit;
    _OutputVoltage.A = -OutputVoltageLimit;
  }

  // anti-windup and output voltage limiting, phase B
  if(_OutputVoltage.B > OutputVoltageLimit){
    _Integral_Ib -= _OutputVoltage.B - OutputVoltageLimit;
    _OutputVoltage.B = OutputVoltageLimit;
  } else if(_OutputVoltage.B < -OutputVoltageLimit) {
    _Integral_Ib -= _OutputVoltage.B + OutputVoltageLimit;
    _OutputVoltage.B = -OutputVoltageLimit;
  }

  _OutputVoltage.C = -_OutputVoltage.A -_OutputVoltage.B;

  PwmModulation(&_OutputVoltage, &Pwm, VoltToPwmScaleFactor);

  return Pwm;
}

void CurrentLoopController::Reset(void){
  _Integral_Ia = 0;
  _Integral_Ib = 0;
}


void CurrentLoopController::AccessCurrentControlFrequency(ObdAccessHandle * handle){
  switch (handle->AccessType) {
    case SDO_CSS_WRITE:
      break;
    case SDO_CSS_READ:
      handle->Data.DataUint16[0] = 32000;
      handle->AccessResult = OBD_ACCESS_SUCCESS;
      break;
    default:
      break;
  }
}

void CurrentLoopController::AccessCurrentLoopGains_Kp(ObdAccessHandle * handle){
  switch (handle->AccessType) {
    case SDO_CSS_WRITE:
      _Kp = handle->Data.DataFloat32;
      handle->AccessResult = OBD_ACCESS_SUCCESS;
      break;
    case SDO_CSS_READ:
      handle->Data.DataFloat32 = _Kp;
      handle->AccessResult = OBD_ACCESS_SUCCESS;
      break;
    default:
      break;
  }
}

void CurrentLoopController::AccessCurrentLoopGains_Ki(ObdAccessHandle * handle){
  switch (handle->AccessType) {
    case SDO_CSS_WRITE:
      _Ki = handle->Data.DataFloat32;
      _Ki_dt = _Ki * CURRENT_CTRL_TIMEBASE;
      handle->AccessResult = OBD_ACCESS_SUCCESS;
      break;
    case SDO_CSS_READ:
      handle->Data.DataFloat32 = _Ki;
      handle->AccessResult = OBD_ACCESS_SUCCESS;
      break;
    default:
      break;
  }
}
