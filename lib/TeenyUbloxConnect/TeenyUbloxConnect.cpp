/*
TeenyUbloxConnect.h - Class file for the TeenyUbloxConnect Arduino Library.
Copyright (C) *** Need copyright statement here ***

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include "TeenyUbloxConnect.h"

/********************************************************************/
TeenyUbloxConnect::TeenyUbloxConnect() { }

TeenyUbloxConnect::~TeenyUbloxConnect() { }

/********************************************************************/
/********************************************************************/
// begin
/********************************************************************/
/********************************************************************/
bool TeenyUbloxConnect::begin(Stream &serialPort_, uint16_t maxWait_) {
  serialPort = &serialPort_;
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_PRT;
  commandPacket.payloadLength = 1;
  commandPacket.payload[0] = COM_PORT_UART1;
  commandPacket.validPacket = true;
  // Try three times
  while(serialPort->available()) serialPort->read();
  if(sendCommandPacket(true, true, maxWait_)) return true;
  while(serialPort->available()) serialPort->read();
  if(sendCommandPacket(true, true, maxWait_)) return true;
  while(serialPort->available()) serialPort->read();
  if(sendCommandPacket(true, true, maxWait_)) return true;
  return false;
}

/********************************************************************/
/********************************************************************/
// ublox commands
/********************************************************************/
/********************************************************************/
void TeenyUbloxConnect::setSerialRate(uint32_t baudrate_, uint8_t uartPort_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_PRT;
  commandPacket.payloadLength = 1;
  commandPacket.payload[0] = uartPort_;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    commandPacket = responsePacket;
    commandPacket.payload[8] = baudrate_;
    commandPacket.payload[9] = baudrate_ >> 8;
    commandPacket.payload[10] = baudrate_ >> 16;
    commandPacket.payload[11] = baudrate_ >> 24;
    commandPacket.validPacket = true;
    sendCommandPacket(false, true, maxWait_);
  }
}

/********************************************************************/
bool TeenyUbloxConnect::saveConfiguration(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_CFG;
  commandPacket.payloadLength = 12;
  memset(commandPacket.payload, 0, commandPacket.payloadLength);
  commandPacket.payload[4] = 0xFF;
  commandPacket.payload[5] = 0xFF;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::getProtocolVersion(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_MON;
  commandPacket.messageID = UBX_MON_VER;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, false, maxWait_)) {
    protocolVersionHigh = ((responsePacket.payload[78] - '0') * 10) +
                          (responsePacket.payload[79] - '0');
    protocolVersionLow = ((responsePacket.payload[81] - '0') * 10) +
                         (responsePacket.payload[82] - '0');
    return true;
  }
  return false;
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getProtocolVersionHigh(uint16_t maxWait_) {
  if(getProtocolVersion(maxWait_)) {
    return protocolVersionHigh;
  }
  return 0;
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getProtocolVersionLow(uint16_t maxWait_) {
  if(getProtocolVersion(maxWait_)) {
    return protocolVersionLow;
  }
  return 0;
}

/********************************************************************/
bool TeenyUbloxConnect::setPortOutput(uint8_t portID_, uint8_t comSettings_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_PRT;
  commandPacket.payloadLength = 1;
  commandPacket.payload[0] = portID_;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    commandPacket = responsePacket;
    commandPacket.payload[14] = comSettings_;
    commandPacket.validPacket = true;
    return sendCommandPacket(false, true, maxWait_);
  }
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::setMeasurementRate(uint16_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_RATE;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    commandPacket = responsePacket;
    commandPacket.payload[0] = rate_;
    commandPacket.payload[1] = rate_ >> 8;
    commandPacket.validPacket = true;
    return sendCommandPacket(false, true, maxWait_);
  }
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::setNavigationRate(uint16_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_RATE;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, true, maxWait_)) {
    commandPacket = responsePacket;
    commandPacket.payload[2] = rate_;
    commandPacket.payload[3] = rate_ >> 8;
    commandPacket.validPacket = true;
    return sendCommandPacket(false, true, maxWait_);
  }
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVPVT(bool enable_, uint16_t maxWait_) {
  return setAutoNAVPVTrate(enable_ ? 1 : 0, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVPVTrate(uint8_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_MSG;
  commandPacket.payloadLength = 3;
  commandPacket.payload[0] = UBX_CLASS_NAV;
  commandPacket.payload[1] = UBX_NAV_PVT;
  commandPacket.payload[2] = (rate_ <= 127) ? rate_ : 127;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoPVT(bool enable_, uint16_t maxWait_) {
  return setAutoNAVPVTrate(enable_ ? 1 : 0, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoPVTrate(uint8_t rate_, uint16_t maxWait_) {
  return setAutoNAVPVTrate(rate_, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVSAT(bool enable_, uint16_t maxWait_) {
  return setAutoNAVSATrate(enable_ ? 1 : 0, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::setAutoNAVSATrate(uint8_t rate_, uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_CFG;
  commandPacket.messageID = UBX_CFG_MSG;
  commandPacket.payloadLength = 3;
  commandPacket.payload[0] = UBX_CLASS_NAV;
  commandPacket.payload[1] = UBX_NAV_SAT;
  commandPacket.payload[2] = (rate_ <= 127) ? rate_ : 127;
  commandPacket.validPacket = true;
  return sendCommandPacket(false, true, maxWait_);
}

/********************************************************************/
bool TeenyUbloxConnect::pollNAVPVT(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_NAV;
  commandPacket.messageID = UBX_NAV_PVT;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, false, maxWait_)) {
    if(ubloxNAVPVTPacketBuffer.validPacket) {
      // Lost rx packet
      lostNAVPVTPacketCount += (lostNAVPVTPacketCount < 99) ? 1 : 0;
      processNAVPVTPacket(); // processing clears validPacket
      return false;
    }
    ubloxNAVPVTPacketBuffer = responsePacket;
    ubloxNAVPVTPacketBuffer.validPacket = true;
    return processNAVPVTPacket();
  }
  return false;
}

/********************************************************************/
bool TeenyUbloxConnect::pollNAVSAT(uint16_t maxWait_) {
  commandPacket.messageClass = UBX_CLASS_NAV;
  commandPacket.messageID = UBX_NAV_SAT;
  commandPacket.payloadLength = 0;
  commandPacket.validPacket = true;
  if(sendCommandPacket(true, false, maxWait_)) {
    if(ubloxNAVSATPacketBuffer.validPacket) {
      // Lost rx packet
      lostNAVSATPacketCount += (lostNAVSATPacketCount < 99) ? 1 : 0;
      processNAVSATPacket(); // processing clears validPacket
      return false;
    }
    ubloxNAVSATPacketBuffer = responsePacket;
    ubloxNAVSATPacketBuffer.validPacket = true;
    return processNAVSATPacket();
  }
  return false;
}

/********************************************************************/
/********************************************************************/
// send command packet
/********************************************************************/
/********************************************************************/
bool TeenyUbloxConnect::sendCommandPacket(bool expectResp_, bool expectAck_, uint16_t maxWait_) {
  if(!commandPacket.validPacket) return false;
  calcCommandPacketChecksum();
  serialPort->write(commandPacket.synch1);
  serialPort->write(commandPacket.synch2);
  serialPort->write(commandPacket.messageClass);
  serialPort->write(commandPacket.messageID);
  serialPort->write(commandPacket.payloadLength & 0xFF);
  serialPort->write(commandPacket.payloadLength >> 8);
  //for(uint16_t i = 0; i < commandPacket.payloadLength; i++) {
  //  serialPort->write(commandPacket.payload[i]);
  //}
  serialPort->write(commandPacket.payload, commandPacket.payloadLength);
  serialPort->write(commandPacket.checksumA);
  serialPort->write(commandPacket.checksumB);
  commandPacket.validPacket = false;
  if(!(expectResp_ || expectAck_)) {
    return true;
  }
  processingUbloxCommand = true;
  responsePacket.validPacket = false;
  acknowledgePacket.validPacket = false;
  uint32_t startTime = millis();
  while((millis() - startTime) < maxWait_) {
    checkUbloxInternal();
    processIncomingPacket(commandPacket.messageClass, commandPacket.messageID);
    if(expectResp_ && expectAck_ && responsePacket.validPacket && acknowledgePacket.validPacket) {
      processingUbloxCommand = false;
      return acknowledgePacket.messageID;
    } else if(expectResp_ && (!expectAck_) && responsePacket.validPacket) {
      processingUbloxCommand = false;
      return true;
    } else if((!expectResp_) && expectAck_ && acknowledgePacket.validPacket) {
      processingUbloxCommand = false;
      return acknowledgePacket.messageID;
    }
  }
  processingUbloxCommand = false;
  return false;
}

/********************************************************************/
/********************************************************************/
// checksum
/********************************************************************/
/********************************************************************/
// Given a message, calc and store the two byte "8-Bit Fletcher"
// checksum over the entirety of the message
// This is called before we send a command packet
void TeenyUbloxConnect::calcCommandPacketChecksum() {
  commandPacket.checksumA = 0;
  commandPacket.checksumB = 0;

  commandPacket.checksumA += commandPacket.messageClass;
  commandPacket.checksumB += commandPacket.checksumA;

  commandPacket.checksumA += commandPacket.messageID;
  commandPacket.checksumB += commandPacket.checksumA;

  commandPacket.checksumA += (commandPacket.payloadLength & 0xFF);
  commandPacket.checksumB += commandPacket.checksumA;

  commandPacket.checksumA += (commandPacket.payloadLength >> 8);
  commandPacket.checksumB += commandPacket.checksumA;

  for(uint16_t i = 0; i < commandPacket.payloadLength; i++) {
    commandPacket.checksumA += commandPacket.payload[i];
    commandPacket.checksumB += commandPacket.checksumA;
  }
}

/********************************************************************/
/********************************************************************/
// Methods for receiving ublox packets
/********************************************************************/
/********************************************************************/
void TeenyUbloxConnect::checkUblox() {
  if(!processingUbloxCommand) {
    checkUbloxInternal();
  }
}

/********************************************************************/
void TeenyUbloxConnect::checkUbloxInternal() {
  while(serialPort->available()) {
    if(incomingPacket.validPacket) return;
    processIncomingByte(serialPort->read());
  }
}

/********************************************************************/
void TeenyUbloxConnect::processIncomingByte(uint8_t incomingByte_) {

  // synch1
  if(!incomingPacket.receivingPacket) {
    if(incomingByte_ == incomingPacket.synch1) {
      // This is the start of a binary sentence. Reset flags.
      // Reset the packet byte counter
      incomingPacket.receivingPacket = true;
      incomingPacket.packetCounter = 1;
    } else {
      // This character is unknown or we missed the previous start of a sentence
    }

  // synch2
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == 1)) {
    if(incomingByte_ == incomingPacket.synch2) {
      incomingPacket.packetCounter++;
      incomingPacket.rollingChecksumA = 0;
      incomingPacket.rollingChecksumB = 0;
    } else {
      // This character is unknown or we missed the previous start of a sentence
      incomingPacket.receivingPacket = false;
    }

  // messageClass, messageID, payloadLength
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter < 6)) {
    if(incomingPacket.packetCounter == 2) {
      incomingPacket.messageClass = incomingByte_;
    } else if(incomingPacket.packetCounter == 3) {
      incomingPacket.messageID = incomingByte_;
    } else if(incomingPacket.packetCounter == 4) {
      incomingPacket.payloadLength = incomingByte_;
    } else if(incomingPacket.packetCounter == 5) {
      incomingPacket.payloadLength |= (incomingByte_ << 8);
      incomingPacket.payloadCounter = 0;
      memset(incomingPacket.payload, 0, UBX_MAXPAYLOADLENGTH);
    }
    incomingPacket.packetCounter++;
    incomingPacket.rollingChecksumA += incomingByte_;
    incomingPacket.rollingChecksumB += incomingPacket.rollingChecksumA;

  // payload
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.payloadCounter < incomingPacket.payloadLength)) {
    if(incomingPacket.payloadCounter < UBX_MAXPAYLOADLENGTH) {
      // Not storing packet data in excess of UBX_MAXPAYLOADLENGTH - Just deal with it!
      incomingPacket.payload[incomingPacket.payloadCounter] = incomingByte_;
    }
    incomingPacket.payloadCounter++;
    incomingPacket.packetCounter++;
    incomingPacket.rollingChecksumA += incomingByte_;
    incomingPacket.rollingChecksumB += incomingPacket.rollingChecksumA;

  // checksumA
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == (incomingPacket.payloadLength + 6))) {
    incomingPacket.checksumA = incomingByte_;
    incomingPacket.packetCounter++;

  // checksumB
  } else if(incomingPacket.receivingPacket &&
            (incomingPacket.packetCounter == (incomingPacket.payloadLength + 7))) {
    incomingPacket.checksumB = incomingByte_;
    incomingPacket.packetCounter++;
    if((incomingPacket.checksumA == incomingPacket.rollingChecksumA) &&
       (incomingPacket.checksumB == incomingPacket.rollingChecksumB)) {
      incomingPacket.validPacket = true;
    }
    incomingPacket.receivingPacket = false;
  }
}

/********************************************************************/
void TeenyUbloxConnect::processIncomingPacket(uint8_t requestedClass_, uint8_t requestedID_) {
  if(incomingPacket.validPacket) {
    if(receivedPacket.validPacket) {
      // Lost rx packet
      lostRxPacketCount += (lostRxPacketCount < 99) ? 1 : 0;
    } else {
      receivedPacket = incomingPacket;
    }
    incomingPacket.validPacket = false;
  }

  if(receivedPacket.validPacket) {

    // Command response and/or acknowledge
    if(requestedClass_) {
      if((receivedPacket.messageClass == requestedClass_) &&
         (receivedPacket.messageID == requestedID_)) {
        responsePacket = receivedPacket;
        receivedPacket.validPacket = false;
      } else if((receivedPacket.messageClass == UBX_CLASS_ACK) &&
                ((receivedPacket.messageID == UBX_ACK_ACK) ||
                 (receivedPacket.messageID == UBX_ACK_NAK)) &&
                (receivedPacket.payloadLength == UBX_ACKNAK_PAYLOADLENGTH) &&
                (receivedPacket.payload[0] == requestedClass_) &&
                (receivedPacket.payload[1] == requestedID_)) {
        acknowledgePacket.messageID = receivedPacket.messageID;
        acknowledgePacket.payload[0] = receivedPacket.payload[0];
        acknowledgePacket.payload[1] = receivedPacket.payload[1];
        acknowledgePacket.checksumA = receivedPacket.checksumA;
        acknowledgePacket.checksumB = receivedPacket.checksumB;
        acknowledgePacket.validPacket = receivedPacket.validPacket;
        receivedPacket.validPacket = false;
      } else {
        // Drop any other received packets that happened by
        receivedPacket.validPacket = false;
      }

    // Automated UBX packets
    } else {

      // autoPVT - NAV-PVT packet
      if((receivedPacket.messageClass == UBX_CLASS_NAV) &&
         (receivedPacket.messageID == UBX_NAV_PVT) &&
         (receivedPacket.payloadLength == UBX_NAV_PVT_PAYLOADLENGTH)) {
        if(ubloxNAVPVTPacketBuffer.validPacket) {
          // Lost rx packet
          lostNAVPVTPacketCount++;
        } else {
          ubloxNAVPVTPacketBuffer = receivedPacket;
        }
        receivedPacket.validPacket = false;
      }

      // autoNAVSAT - NAV-SAT packet
      if((receivedPacket.messageClass == UBX_CLASS_NAV) &&
         (receivedPacket.messageID == UBX_NAV_SAT) &&
         (receivedPacket.payloadLength >= UBX_NAV_SAT_MINPAYLOADLENGTH) &&
         (receivedPacket.payloadLength <= UBX_NAV_SAT_MAXPAYLOADLENGTH)) {
        if(ubloxNAVSATPacketBuffer.validPacket) {
          // Lost rx packet
          lostNAVSATPacketCount++;
        } else {
          ubloxNAVSATPacketBuffer = receivedPacket;
        }
        receivedPacket.validPacket = false;
      }

    }
  }
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getLostRxPacketCount() {
  return lostRxPacketCount;
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getLostNAVPVTPacketCount() {
  return lostNAVPVTPacketCount;
}

/********************************************************************/
uint8_t TeenyUbloxConnect::getLostNAVSATPacketCount() {
  return lostNAVSATPacketCount;
}


/********************************************************************/
/********************************************************************/
// Methods for NAVPVT packet processing
/********************************************************************/
/********************************************************************/
bool TeenyUbloxConnect::getNAVPVT() {
  processIncomingPacket();
  return processNAVPVTPacket();
}

/********************************************************************/
// For compatibility with SparkFun_u-blox_GNSS_Arduino_Library
bool TeenyUbloxConnect::getPVT() {
  return getNAVPVT();
}

/********************************************************************/
bool TeenyUbloxConnect::processNAVPVTPacket() {
  if(ubloxNAVPVTPacketBuffer.validPacket) {
    // Save buffer to packet
    ubloxNAVPVTPacket[0] = ubloxNAVPVTPacketBuffer.synch1;
    ubloxNAVPVTPacket[1] = ubloxNAVPVTPacketBuffer.synch2;
    ubloxNAVPVTPacket[2] = ubloxNAVPVTPacketBuffer.messageClass;
    ubloxNAVPVTPacket[3] = ubloxNAVPVTPacketBuffer.messageID;
    ubloxNAVPVTPacket[4] = ubloxNAVPVTPacketBuffer.payloadLength;
    ubloxNAVPVTPacket[5] = ubloxNAVPVTPacketBuffer.payloadLength >> 8;
    memcpy(ubloxNAVPVTPacket+6, ubloxNAVPVTPacketBuffer.payload, 92);
    ubloxNAVPVTPacket[98] = ubloxNAVPVTPacketBuffer.checksumA;
    ubloxNAVPVTPacket[99] = ubloxNAVPVTPacketBuffer.checksumB;
    // Parse packet fields
    setNAVPVTPacketInfo();
    ubloxNAVPVTPacketBuffer.validPacket = false;
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyUbloxConnect::setNAVPVTPacketInfo() {
  ubxNAVPVTInfo.year = ubloxNAVPVTPacketBuffer.payload[4];
  ubxNAVPVTInfo.year |= ubloxNAVPVTPacketBuffer.payload[5] << 8;
  ubxNAVPVTInfo.month = ubloxNAVPVTPacketBuffer.payload[6];
  ubxNAVPVTInfo.day = ubloxNAVPVTPacketBuffer.payload[7];
  ubxNAVPVTInfo.hour = ubloxNAVPVTPacketBuffer.payload[8];
  ubxNAVPVTInfo.minute = ubloxNAVPVTPacketBuffer.payload[9];
  ubxNAVPVTInfo.second = ubloxNAVPVTPacketBuffer.payload[10];
  ubxNAVPVTInfo.dateValid = ubloxNAVPVTPacketBuffer.payload[11] & 0x01;
  ubxNAVPVTInfo.timeValid = (ubloxNAVPVTPacketBuffer.payload[11] & 0x02) >> 1;
  ubxNAVPVTInfo.tAcc = ubloxNAVPVTPacketBuffer.payload[12];
  ubxNAVPVTInfo.tAcc |= ubloxNAVPVTPacketBuffer.payload[13] << 8;
  ubxNAVPVTInfo.tAcc |= ubloxNAVPVTPacketBuffer.payload[14] << 16;
  ubxNAVPVTInfo.tAcc |= ubloxNAVPVTPacketBuffer.payload[15] << 24;
  ubxNAVPVTInfo.fixType = ubloxNAVPVTPacketBuffer.payload[20];
  ubxNAVPVTInfo.locationValid = ubloxNAVPVTPacketBuffer.payload[21] & 0x01;
  ubxNAVPVTInfo.numSV = ubloxNAVPVTPacketBuffer.payload[23];
  ubxNAVPVTInfo.longitude = ubloxNAVPVTPacketBuffer.payload[24];
  ubxNAVPVTInfo.longitude |= ubloxNAVPVTPacketBuffer.payload[25] << 8;
  ubxNAVPVTInfo.longitude |= ubloxNAVPVTPacketBuffer.payload[26] << 16;
  ubxNAVPVTInfo.longitude |= ubloxNAVPVTPacketBuffer.payload[27] << 24;
  ubxNAVPVTInfo.latitude = ubloxNAVPVTPacketBuffer.payload[28];
  ubxNAVPVTInfo.latitude |= ubloxNAVPVTPacketBuffer.payload[29] << 8;
  ubxNAVPVTInfo.latitude |= ubloxNAVPVTPacketBuffer.payload[30] << 16;
  ubxNAVPVTInfo.latitude |= ubloxNAVPVTPacketBuffer.payload[31] << 24;
  ubxNAVPVTInfo.altitudeMSL = ubloxNAVPVTPacketBuffer.payload[36];
  ubxNAVPVTInfo.altitudeMSL |= ubloxNAVPVTPacketBuffer.payload[37] << 8;
  ubxNAVPVTInfo.altitudeMSL |= ubloxNAVPVTPacketBuffer.payload[38] << 16;
  ubxNAVPVTInfo.altitudeMSL |= ubloxNAVPVTPacketBuffer.payload[39] << 24;
  ubxNAVPVTInfo.hAcc = ubloxNAVPVTPacketBuffer.payload[40];
  ubxNAVPVTInfo.hAcc |= ubloxNAVPVTPacketBuffer.payload[41] << 8;
  ubxNAVPVTInfo.hAcc |= ubloxNAVPVTPacketBuffer.payload[42] << 16;
  ubxNAVPVTInfo.hAcc |= ubloxNAVPVTPacketBuffer.payload[43] << 24;
  ubxNAVPVTInfo.vAcc = ubloxNAVPVTPacketBuffer.payload[44];
  ubxNAVPVTInfo.vAcc |= ubloxNAVPVTPacketBuffer.payload[45] << 8;
  ubxNAVPVTInfo.vAcc |= ubloxNAVPVTPacketBuffer.payload[46] << 16;
  ubxNAVPVTInfo.vAcc |= ubloxNAVPVTPacketBuffer.payload[47] << 24;
  ubxNAVPVTInfo.headMot = ubloxNAVPVTPacketBuffer.payload[64];
  ubxNAVPVTInfo.headMot |= ubloxNAVPVTPacketBuffer.payload[65] << 8;
  ubxNAVPVTInfo.headMot |= ubloxNAVPVTPacketBuffer.payload[66] << 16;
  ubxNAVPVTInfo.headMot |= ubloxNAVPVTPacketBuffer.payload[67] << 24;
  ubxNAVPVTInfo.pDOP = ubloxNAVPVTPacketBuffer.payload[76];
  ubxNAVPVTInfo.pDOP |= ubloxNAVPVTPacketBuffer.payload[77] << 8;
}

/********************************************************************/
void TeenyUbloxConnect::getNAVPVTPacket(uint8_t *packet_) {
  memcpy(packet_, ubloxNAVPVTPacket, UBX_NAV_PVT_PACKETLENGTH);
}

/********************************************************************/
bool TeenyUbloxConnect::getDateValid() {
  return ubxNAVPVTInfo.dateValid;
}
bool TeenyUbloxConnect::getTimeValid() {
  return ubxNAVPVTInfo.timeValid;
}
uint32_t TeenyUbloxConnect::getTimeAccEst() {
  return ubxNAVPVTInfo.tAcc;
}
uint16_t TeenyUbloxConnect::getYear() {
  return ubxNAVPVTInfo.year;
}
uint8_t TeenyUbloxConnect::getMonth() {
  return ubxNAVPVTInfo.month;
}
uint8_t TeenyUbloxConnect::getDay() {
  return ubxNAVPVTInfo.day;
}
uint8_t TeenyUbloxConnect::getHour() {
  return ubxNAVPVTInfo.hour;
}
uint8_t TeenyUbloxConnect::getMinute() {
  return ubxNAVPVTInfo.minute;
}
uint8_t TeenyUbloxConnect::getSecond() {
  return ubxNAVPVTInfo.second;
}
/********************************************************************/
uint8_t TeenyUbloxConnect::getFixType() {
  return ubxNAVPVTInfo.fixType;
}
bool TeenyUbloxConnect::getGnssFixOk() {
  return ubxNAVPVTInfo.locationValid;
}
uint8_t TeenyUbloxConnect::getSIV() {
  return ubxNAVPVTInfo.numSV;
}
int32_t TeenyUbloxConnect::getLongitude() {
  return ubxNAVPVTInfo.longitude;
}
int32_t TeenyUbloxConnect::getLatitude() {
  return ubxNAVPVTInfo.latitude;
}
int32_t TeenyUbloxConnect::getAltitudeMSL() {
  return ubxNAVPVTInfo.altitudeMSL;
}
uint32_t TeenyUbloxConnect::getHorizontalAccEst() {
  return ubxNAVPVTInfo.hAcc;
}
uint32_t TeenyUbloxConnect::getVerticalAccEst() {
  return ubxNAVPVTInfo.vAcc;
}
int32_t TeenyUbloxConnect::getHeading() {
  return ubxNAVPVTInfo.headMot;
}
uint16_t TeenyUbloxConnect::getPDOP() {
  return ubxNAVPVTInfo.pDOP;
}


/********************************************************************/
/********************************************************************/
// Methods for NAVSAT packet processing
/********************************************************************/
/********************************************************************/
bool TeenyUbloxConnect::getNAVSAT() {
  processIncomingPacket();
  return processNAVSATPacket();
}

/********************************************************************/
bool TeenyUbloxConnect::processNAVSATPacket() {
  if(ubloxNAVSATPacketBuffer.validPacket) {
    // Save buffer to packet
    ubloxNAVSATPacket = ubloxNAVSATPacketBuffer;
    // Parse packet fields
    setNAVSATPacketInfo();
    ubloxNAVSATPacketBuffer.validPacket = false;
    return true;
  }
  return false;
}

/********************************************************************/
void TeenyUbloxConnect::setNAVSATPacketInfo() {
  ubxNAVSATInfo.numSvs = ubloxNAVSATPacketBuffer.payload[5];
  ubxNAVSATInfo.numSvsHealthy = 0;
  ubxNAVSATInfo.numSvsUsed = 0;
  // Reset sort list
  for(uint8_t i=0; i<32; i++) {
    ubxNAVSATInfo.svSortList[i].gnssId = 0;
    ubxNAVSATInfo.svSortList[i].svId = 0;
    ubxNAVSATInfo.svSortList[i].cno = 0;
    ubxNAVSATInfo.svSortList[i].healthy = false;
    ubxNAVSATInfo.svSortList[i].svUsed = false;
  }
  // Using cno to filter unusable or filtered svs
  // Zero out cno of satellites with no signal or unknown/unhealthy signal
  for(uint8_t i=0; i<ubxNAVSATInfo.numSvs; i++) {
    if((ubloxNAVSATPacketBuffer.payload[(i*12)+16] & 0x30) != 0x10) {
      ubloxNAVSATPacketBuffer.payload[(i*12)+10] = 0;
    }
  }
  // Find and sort up to 32 usable satellites
  for(uint8_t i=0; i<32; i++) {
    bool foundSat = false;
    uint8_t foundSatIndex;
    ubloxNAVSATSVInfo_t foundSatInfo, compareSatInfo; 
    for(uint8_t j=0; j<ubxNAVSATInfo.numSvs; j++) {
      if(ubloxNAVSATPacketBuffer.payload[(j*12)+10]) {
        if(!foundSat) {
          foundSat = true;
          foundSatIndex = j;
          foundSatInfo.gnssId = ubloxNAVSATPacketBuffer.payload[(j*12)+8];
          foundSatInfo.svId = ubloxNAVSATPacketBuffer.payload[(j*12)+9];
          foundSatInfo.cno = ubloxNAVSATPacketBuffer.payload[(j*12)+10];
          foundSatInfo.healthy = ((ubloxNAVSATPacketBuffer.payload[(j*12)+16] & 0x30) == 0x10) ? true : false;
          foundSatInfo.svUsed = (ubloxNAVSATPacketBuffer.payload[(j*12)+16] & 0x08) ? true : false;
        } else {
          // compare all the fields to see which is a 'better' satellite and replace if better
          compareSatInfo.gnssId = ubloxNAVSATPacketBuffer.payload[(j*12)+8];
          compareSatInfo.svId = ubloxNAVSATPacketBuffer.payload[(j*12)+9];
          compareSatInfo.cno = ubloxNAVSATPacketBuffer.payload[(j*12)+10];
          compareSatInfo.healthy = ((ubloxNAVSATPacketBuffer.payload[(j*12)+16] & 0x30) == 0x10) ? true : false;
          compareSatInfo.svUsed = (ubloxNAVSATPacketBuffer.payload[(j*12)+16] & 0x08) ? true : false;
          if((compareSatInfo.svUsed && (!foundSatInfo.svUsed)) ||
             ((compareSatInfo.svUsed == foundSatInfo.svUsed) &&
              (compareSatInfo.cno > foundSatInfo.cno))) {
            foundSatIndex = j;
            foundSatInfo = compareSatInfo;
          }
        }
      }
    }
    if(foundSat) {
      // remove satellite from buffer
      ubloxNAVSATPacketBuffer.payload[(foundSatIndex*12)+10] = 0;
      ubxNAVSATInfo.svSortList[i] = foundSatInfo;
      ubxNAVSATInfo.numSvsHealthy++;
      if(foundSatInfo.svUsed) ubxNAVSATInfo.numSvsUsed++;
    } else {
      break;
    }
  }
}

/********************************************************************/
void TeenyUbloxConnect::getNAVSATPacket(ubloxPacket_t &packet_) {
  packet_ = ubloxNAVSATPacket;
}

/********************************************************************/
void TeenyUbloxConnect::getNAVSATInfo(ubloxNAVSATInfo_t &info_) {
  info_ = ubxNAVSATInfo;
}

