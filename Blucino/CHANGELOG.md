//==================================================================================================
//  CHANGELOG
//  change log of Blucino library interface to Bluenetics Bluetooth Mesh module
//
//  Created by Hugo Pristauz on 30.03.2019
//  Copyright © 2019 Bluenetics GmbH. All rights reserved.
//==================================================================================================
/*

CHANGELOG
=========
- ### 'start with BlucinoListen and BlucinoReceive - Blucino.V1A.Bak1'
- all Blucino stuff moved into classes
- ### 'separate C++ class files (.cpp,.h) - Blucino.V1A.Bak2'
- event callback implemented
- tracing incorporated int Blucino
- ### 'implement Blucino callback - Blucino.V1A.Bak3'
- initialized flag added and Blucino::error() method
- all classes reorganized
- mesh class introduced
- ### 'add initialized flag and Blucino::error() method - Blucino.V1A.Bak4'
- BlucinoReceive1 fine tuned (no whistles and bells)
- ### 'crearte BlucinoReceive1.ino demo (without whistles and bells) - Blucino.V1A.Bak5'
- ### 'make demos BlucinoReceive0.ino, ..., BlucinoReceive3.ino - Blucino.V1A.Bak6'
- made Mesh::loop running
- ### 'make Mesh::loop() running - Blucino.V1A.Bak7'
- mesh models added
- added model validation in BlucinoLoop
- ### 'make Mesh::loop() running - Blucino.V1A.Bak8'
- all BlucinoReceive<n> demos converted to library supported versions
- ### 'use Blucino library for all BlucinoReceive<n> demos - Blucino.V1A.Bak9'
- get original SILVAIR MCU_SERVER running
- some slight mods to eliminate compiler warnings
- ### 'get original SILVAIR MCU server on Teennsy Arduino running - Blucino.V1A.Bak10'
- ### 'modify SILVAR MCU_Server => iHuxServer for analysis with iHux traces - Blucino.V1A.Bak11'
- ### 'complete full analysis of iHuxServer/Silvair-modem communication - Blucino.V1A.Bak12'
- ### 'make small version iHuxServerBasic - Blucino.V1A.Bak13'

Known Bugs
==========
- none so far


ToDo
====
- get onoffserver into mesh
- loop() method to be called in Arduino main loop
- blink sequence
- fading

*/
