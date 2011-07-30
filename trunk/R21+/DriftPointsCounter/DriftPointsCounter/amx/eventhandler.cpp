// Copyright (c) 2011 Zeex
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "eventhandler.h"

class StaticData {
public:
    static StaticData *GetInstance();
    sampgdk::EventHandler *first;
private:
    StaticData();
};

StaticData::StaticData() 
    : first(0)
{
}

StaticData *StaticData::GetInstance() {
    static StaticData sd;
    return &sd;
}

namespace sampgdk { 

EventHandler::EventHandler()
    : registered_(false), next_(0), prev_(0)
{
}

EventHandler::~EventHandler() {
    this->Unregister();
}

void EventHandler::Register() {
    EventHandler *first = StaticData::GetInstance()->first;
    next_ = first;
    if (first != 0) {
        first->prev_ = this;
    }
    StaticData::GetInstance()->first = this;
    registered_ = true;
}

void EventHandler::Unregister() {
    if (registered_) {
        if (StaticData::GetInstance()->first == this) {
            StaticData::GetInstance()->first = next_;
            next_->prev_ = 0;
        } else {
            next_->prev_ = prev_;
            prev_->next_ = next_;
        }
        registered_ = false;
    }
}

EventHandler *EventHandler::GetNext() const {
    return next_;
}

EventHandler *EventHandler::GetFirstEventHandler() {
    return StaticData::GetInstance()->first;
}

bool EventHandler::IsRegistered() const {
    return registered_;
}

void EventHandler::OnGameModeInit() { 
}

void EventHandler::OnGameModeExit() { 
}

bool EventHandler::OnPlayerConnect(int playerid) { 
    return true; 
}

bool EventHandler::OnPlayerDisconnect(int playerid, int reason) { 
    return true; 
}

bool EventHandler::OnPlayerSpawn(int playerid) {
    return true;
}

bool EventHandler::OnPlayerDeath(int playerid, int killerid, int reason) {
    return true;
}

bool EventHandler::OnVehicleSpawn(int vehicleid) {
    return true;
}

bool EventHandler::OnVehicleDeath(int vehicleid, int killerid) {
    return true;
}

bool EventHandler::OnPlayerText(int playerid, const std::string &text) {
    return true;
}

bool EventHandler::OnPlayerCommandText(int playerid, const std::string &cmdtext) {
    return false;
}

bool EventHandler::OnPlayerRequestClass(int playerid, int classid) {
    return true;
}

bool EventHandler::OnPlayerEnterVehicle(int playerid, int vehicleid, bool ispassenger) {
    return true;
}

bool EventHandler::OnPlayerExitVehicle(int playerid, int vehicleid) {
    return true;
}

bool EventHandler::OnPlayerStateChange(int playerid, int newstate, int oldstate) {
    return true;
}

bool EventHandler::OnPlayerEnterCheckpoint(int playerid) {
    return true;
}

bool EventHandler::OnPlayerLeaveCheckpoint(int playerid) {
    return true;
}

bool EventHandler::OnPlayerEnterRaceCheckpoint(int playerid) {
    return true;
}

bool EventHandler::OnPlayerLeaveRaceCheckpoint(int playerid) {
    return true;
}

bool EventHandler::OnRconCommand(const std::string &cmd) {
    return false;
}

bool EventHandler::OnPlayerRequestSpawn(int playerid) {
    return true;
}

bool EventHandler::OnObjectMoved(int objectid) {
    return true;
}

bool EventHandler::OnPlayerObjectMoved(int playerid, int objectid) {
    return true;
}

bool EventHandler::OnPlayerPickUpPickup(int playerid, int pickupid) {
    return true;
}

bool EventHandler::OnVehicleMod(int playerid, int vehicleid, int componentid) {
    return true;
}

bool EventHandler::OnEnterExitModShop(int playerid, bool enterexit, int interiorid) {
    return true;
}

bool EventHandler::OnVehiclePaintjob(int playerid, int vehicleid, int paintjobid) {
    return true;
}

bool EventHandler::OnVehicleRespray(int playerid, int vehicleid, int color1, int color2) {
    return true;
}

bool EventHandler::OnVehicleDamageStatusUpdate(int vehicleid, int playerid) {
    return true;
}

bool EventHandler::OnPlayerSelectedMenuRow(int playerid, int row) {
    return true;
}

bool EventHandler::OnPlayerExitedMenu(int playerid) {
    return true;
}

bool EventHandler::OnPlayerInteriorChange(int playerid, int newinteriorid, int oldinteriorid) {
    return true;
}

bool EventHandler::OnPlayerKeyStateChange(int playerid, int newkeys, int oldkeys) {
    return true;
}

bool EventHandler::OnRconLoginAttempt(const std::string &ip, const std::string &password, bool success) {
    return true;
}

bool EventHandler::OnPlayerUpdate(int playerid) {
    return true;
}

bool EventHandler::OnPlayerStreamIn(int playerid, int forplayerid) {
    return true;
}

bool EventHandler::OnPlayerStreamOut(int playerid, int forplayerid) {
    return true;
}

bool EventHandler::OnVehicleStreamIn(int vehicleid, int forplayerid) {
    return true;
}

bool EventHandler::OnVehicleStreamOut(int vehicleid, int forplayerid) {
    return true;
}

bool EventHandler::OnDialogResponse(int playerid, int dialogid, bool response, int listitem, const std::string &inputtext) {
    return true;
}

bool EventHandler::OnPlayerClickPlayer(int playerid, int clickedplayerid, int source) {
    return true;
}

} // namespace sampgdk

