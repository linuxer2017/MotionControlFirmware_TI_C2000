/******************************************************************************
 * Copyright (C) 2017 by Yifan Jiang                                          *
 * jiangyi@student.ethz.com                                                   *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                       *
 ******************************************************************************/

/*
* communication interface
* collectively handle all communication traffic
*/

#include "CommunicationInterface.h"

/**
 *  initialize the CiA message buffers
 *  @param msgptr    pointer to CiA_Message
 *  @param sdoptr
 *  @param pdoptr
 *  @retval None
 */
void CommunicationInterface::SetCiaMsgBuffer(CiA_Message * msgptr,
                                             CiA_SdoMessage * sdoptr,
                                             CiA_PdoMessage * pdoptr)
{
  ciamsg = msgptr;
  sdomsg = sdoptr;
  pdomsg = pdoptr;
}

/**
 *  transmits message(s) over communication interface
 */
void CommunicationInterface::ExecuteTransmission(void){

}

/**
 *  check and process messages received over communication interface
 */
#pragma CODE_SECTION(".TI.ramfunc");
void CommunicationInterface::ExecuteReception(void){

  uint16_t msgcnt = _UartDriver->ExecuteParsing(ciamsg);
  if(msgcnt > 0 ){

    if(ciamsg->CANID == CANID_NMT){
      // handle CANOpen NMT protocol
      _NmtNewState = __byte_uint16_t(ciamsg->Data, 1);
      _NmtUpdated = true;

    } else if((ciamsg->CANID-NODE_ID)==CANID_SDO_RX) {
      // handle CANOpen SDO
      memcpy(&(sdomsg->Ctrl), &(ciamsg->Data[0]), sizeof(uint32_t));
      memcpy(&(sdomsg->Data[0]), &(ciamsg->Data[2]), 4*sizeof(uint16_t));

    } else if((ciamsg->CANID-NODE_ID)==CANID_PDO_RX) {
      // handle CANOpen PDO
      memcpy(&(pdomsg->Data[0]), &(ciamsg->Data[0]), 6*sizeof(uint16_t));
      _PdoUpdated = true;
    } else {

    }
  }

  ciamsg->reserve = 0x0A;
}

/**
 *  retrive state transition cmd from NMT message
 *  @param mnt_state  buffer to hold new state
 *  @retval true if new NMT message is received, false otherwise
 */
bool CommunicationInterface::CheckNmtUpdate(void){
  if(_NmtUpdated == true){
    _NmtUpdated = false;
    return true;
  }

  return false;
}